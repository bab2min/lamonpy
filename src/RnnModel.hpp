#pragma once

#include "layers.hpp"
#include "LatinFeat.h"

namespace ll
{
    class RnnCell
    {
        LSTMCell cell;
        LayerNorm layernorm;
        Dense token_proj;
        union { std::array<Dense, 8> feat_proj; };
        size_t approx_size, unk_token;
    public:
        struct State
        {
            Eigen::VectorXf input_h, c_state;

            State() = default;

            State(size_t input_size, size_t hidden_size) :
                input_h{ Eigen::VectorXf::Zero(input_size + hidden_size) },
                c_state{ Eigen::VectorXf::Zero(hidden_size) }
            {
            }

            Eigen::Block<Eigen::VectorXf, -1, 1, false> input_view()
            {
                return input_h.topRows(input_h.rows() - c_state.rows());
            }
        };

        struct LogitReference
        {
            const Eigen::VectorXf& hidden;
            const Dense& proj;
            float sum = 0;
            size_t estim_size = 0;
            size_t unk_token = 0;
            LogitReference(const Eigen::VectorXf& _hidden, const Dense& _proj, 
                float _sum = 0, size_t _estim_size = 0, size_t _unk_token = 0
            )
                : hidden{ _hidden }, proj{ _proj }, 
                sum{ _sum }, estim_size{ _estim_size }, unk_token{ _unk_token }
            {
            }

            float operator[](size_t idx) const
            {
                if (idx >= proj.bias.size()) idx = unk_token;
                if (idx < estim_size) return proj.partial(hidden, idx) - sum;
                return -std::log(std::exp(sum - proj.partial(hidden, idx)) + 1);
            }
        };

        struct Output
        {
            LogitReference token_logits;
            std::array<const float*, 8> feat_logits;
            Output(const Eigen::VectorXf& _hidden, const Dense& _proj,
                float _sum = 0, size_t _estim_size = 0, size_t _unk_token = 0)
                : token_logits{ _hidden, _proj, _sum, _estim_size, _unk_token }
            {
            }
        };

        RnnCell(const utils::ObjectCollection& objs, const std::string& key,
            size_t _approx_size = 2048, size_t _unk_token = 1
        ) :
            cell{ objs, key + "/layer_0/rnn/lstm_cell" },
            layernorm{ objs, key + "/LayerNorm" },
            token_proj{ objs, key + "/output_token_proj" },
            approx_size{ _approx_size },
            unk_token{ _unk_token }
        {
            for (size_t i = 0; i < feat_proj.size(); ++i)
            {
                new (&feat_proj[i]) Dense{ objs, key + "/output_feature_" + std::to_string(i) + "_proj" };
            }
        }

        ~RnnCell()
        {
            for (size_t i = 0; i < feat_proj.size(); ++i)
            {
                if(*(int64_t*)&feat_proj[i]) feat_proj[i].~Dense();
            }
        }

        State get_initial_state() const
        {
            return { cell.input_size(), cell.h_size() };
        }

        Output apply(State& state) const
        {
            thread_local Eigen::VectorXf hidden;
            thread_local Eigen::ArrayXf token_logits;
            thread_local std::array<Eigen::ArrayXf, 8> feat_logits;

            if(hidden.size() < cell.h_size()) hidden.resize(cell.h_size());
            layernorm.apply(hidden, cell(state.input_h, state.c_state).bottomRows(cell.h_size()));
            
            Eigen::VectorXf t_logits = token_proj.partial(hidden, 0, approx_size);
            float t_max = t_logits.maxCoeff();
            float t_normalizer = std::log((t_logits.array() - t_max).exp().sum()) + t_max;

            for (size_t i = 0; i < feat_logits.size(); ++i)
            {
                feat_logits[i] = feat_proj[i](hidden).array();
                feat_logits[i] = logsoftmax(feat_logits[i]);
            }
            Output ret{ hidden, token_proj, t_normalizer, approx_size, unk_token };
            for(size_t i = 0; i < feat_logits.size(); ++i) ret.feat_logits[i] = feat_logits[i].data();
            return ret;
        }
    };

    class LatinRnnModel
    {
        utils::MMap mmap;
        utils::ObjectCollection objs;
        EmbeddingLookup token_emb;
        union { std::array<EmbeddingLookup, 8> feat_emb; };
        LayerNorm emb_layernorm;
        RnnCell cell, cell_bw;
        RnnCell::State begin_state;
        size_t unk_token = 0, bos_token = 0, eos_token = 0;

    public:
        using DecOutput = std::pair<size_t, Feature>;
        using Candidate = std::pair<float, DecOutput>;
        using DecSequence = std::pair<float, std::vector<DecOutput>>;

        LatinRnnModel(const std::string& model_path, size_t approx_size = 2048,
            size_t _unk_token = 1, size_t _bos_token = 2, size_t _eos_token = 3) : 
            mmap{ model_path }, objs{ utils::ObjectCollection::read_from(mmap) },
            token_emb{ objs["emb/token_embedding:0"] },
            emb_layernorm{ objs, "emb/LayerNorm" },
            cell{ objs, "lm", approx_size, _unk_token },
            cell_bw{ objs, "lm_bw", approx_size, _unk_token },
            unk_token{ _unk_token },
            bos_token{ _bos_token },
            eos_token{ _eos_token }
        {
            for (size_t i = 0; i < feat_emb.size(); ++i)
            {
                new (&feat_emb[i]) EmbeddingLookup{ objs["emb/feat_embedding:0"].to_matrix(i) };
            }
        }

        ~LatinRnnModel()
        {
            for (size_t i = 0; i < feat_emb.size(); ++i)
            {
                if(*(int64_t*)&feat_emb[i]) feat_emb[i].~EmbeddingLookup();
            }
        }

        size_t get_unk_token() const { return unk_token;  }

        template<typename _Selector>
        std::vector<DecSequence> decode(size_t length, size_t beam_size, _Selector&& selector, bool bidirection = true) const
        {
            struct BeamPath
            {
                float score = 0;
                std::vector<DecOutput> decoded;
                RnnCell::State state;

                BeamPath() = default;
                BeamPath(RnnCell::State&& _state) : state{ _state }
                {
                }

                bool operator<(const BeamPath& o) const
                {
                    return score < o.score;
                }
            };

            std::vector<BeamPath> pathes, new_pathes;

            pathes.emplace_back(cell.get_initial_state());

            for (size_t t = 0; t < length; ++t)
            {
                for (auto& path : pathes)
                {
                    auto input = path.state.input_view();
                    if (t == 0)
                    {
                        input = token_emb[bos_token];
                    }
                    else
                    {
                        auto& p = path.decoded.back();
                        input = token_emb[p.first];
                        for (size_t f = 0; f < p.second.u8.size(); ++f)
                        {
                            if(p.second[f]) input += feat_emb[f][p.second[f] - 1];
                        }
                    }
                    emb_layernorm.apply_inplace(input);
                    std::vector<Candidate> cands = selector(t, cell.apply(path.state));

                    auto update_idx = new_pathes.size();
                    // to do: lazy state copy
                    new_pathes.emplace_back(std::move(path));
                    new_pathes.insert(new_pathes.end(), cands.size() - 1, new_pathes.back());

                    for (size_t i = 0; i < cands.size(); ++i)
                    {
                        new_pathes[update_idx + i].score += cands[i].first;
                        new_pathes[update_idx + i].decoded.emplace_back(cands[i].second);
                    }
                }
                std::sort(new_pathes.rbegin(), new_pathes.rend());
                if (new_pathes.size() > beam_size)
                {
                    new_pathes.erase(new_pathes.begin() + beam_size, new_pathes.end());
                }
                pathes = std::move(new_pathes);
                new_pathes.clear();
            }

            if (bidirection)
            {
                for (auto& path : pathes)
                {
                    RnnCell::State state = cell_bw.get_initial_state();
                    auto input = state.input_view();
                    float score = 0;
                    for (size_t t = 0; t < length; ++t)
                    {
                        if (t == 0)
                        {
                            input = token_emb[eos_token];
                        }
                        else
                        {
                            auto& p = path.decoded[length - t];
                            input = token_emb[p.first];
                            for (size_t f = 0; f < p.second.u8.size(); ++f)
                            {
                                if (p.second[f]) input += feat_emb[f][p.second[f] - 1];
                            }
                        }
                        emb_layernorm.apply_inplace(input);
                        RnnCell::Output out = cell_bw.apply(state);
                        auto& p = path.decoded[length - t - 1];
                        score += out.token_logits[p.first];
                        for (size_t f = 0; f < out.feat_logits.size(); ++f)
                        {
                            score += out.feat_logits[f][p.second[f]];
                        }
                    }
                    path.score += score;
                }

                std::sort(pathes.rbegin(), pathes.rend());
            }
            
            std::vector<DecSequence> ret;
            for (auto& p : pathes)
            {
                ret.emplace_back(p.score, std::move(p.decoded));
            }
            return ret;
        }
    };
}
