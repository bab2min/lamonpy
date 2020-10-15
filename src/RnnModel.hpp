#pragma once

#include "layers.hpp"
#include "LatinFeat.h"

namespace lamon
{
    class RnnCell
    {
        LSTMCell cell;
        LayerNorm layernorm;
        Dense token_proj;
        union { std::array<Dense, 8> feat_proj; };
        union { 
            Dense joint_token_feat; 
            int64_t has_joint_layer;
        };
        size_t approx_size, unk_token;
    public:
        using DecOutput = std::pair<size_t, Feature>;

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

        class Output
        {
            const RnnCell& rnn;
            const EmbeddingLookup& embs;
            const Eigen::VectorXf& hidden;
            mutable std::unordered_map<size_t, Eigen::ArrayXXf> feat_logits_by_token;
            float sum = 0;

        public:
            Output(const RnnCell& _rnn, const EmbeddingLookup& _embs,
                const Eigen::VectorXf& _hidden, 
                float _sum = 0)
                : rnn{ _rnn }, embs{ _embs },
                hidden{ _hidden },
                sum{ _sum }
            {
            }

            float token_logits(size_t idx) const
            {
                if (idx >= rnn.token_proj.bias.size()) idx = rnn.unk_token;
                if (idx < rnn.approx_size) return rnn.token_proj.partial(hidden, idx) - sum;
                return -std::log(std::exp(sum - rnn.token_proj.partial(hidden, idx)) + 1);
            }

            float operator[](DecOutput dec) const
            {
                float ret = token_logits(dec.first);
                std::unordered_map<size_t, Eigen::ArrayXXf>::iterator it;
                if (rnn.has_joint_layer)
                {
                    it = feat_logits_by_token.find(dec.first);
                    if (it == feat_logits_by_token.end())
                    {
                        Eigen::VectorXf intermediate = rnn.joint_token_feat.apply_concated(hidden, embs[dec.first]);
                        intermediate = intermediate.array().tanh();

                        Eigen::ArrayXXf feat_logits(rnn.feat_proj[0].output_size(), rnn.feat_proj.size());
                        for (size_t i = 0; i < rnn.feat_proj.size(); ++i)
                        {
                            feat_logits.col(i) = rnn.feat_proj[i](intermediate).array();
                            feat_logits.col(i) = logsoftmax(feat_logits.col(i));
                        }
                        it = feat_logits_by_token.emplace(dec.first, std::move(feat_logits)).first;
                    }
                }
                else
                {
                    it = feat_logits_by_token.find(-1);
                    if (it == feat_logits_by_token.end())
                    {
                        Eigen::ArrayXXf feat_logits(rnn.feat_proj[0].output_size(), rnn.feat_proj.size());
                        for (size_t i = 0; i < rnn.feat_proj.size(); ++i)
                        {
                            feat_logits.col(i) = rnn.feat_proj[i](hidden).array();
                            feat_logits.col(i) = logsoftmax(feat_logits.col(i));
                        }
                        it = feat_logits_by_token.emplace(dec.first, std::move(feat_logits)).first;
                    }
                }

                for (size_t i = 0; i < it->second.cols(); ++i)
                {
                    ret += it->second(dec.second[i], i);
                }
                return ret;
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

            try
            {
                new (&joint_token_feat) Dense{ objs, key + "/intermediate_feature_proj" };
            }
            catch (const exc::KeyNotFound&)
            {
                has_joint_layer = 0;
            }
        }

        ~RnnCell()
        {
            for (size_t i = 0; i < feat_proj.size(); ++i)
            {
                if(*(int64_t*)&feat_proj[i]) feat_proj[i].~Dense();
            }
            if (has_joint_layer)
            {
                joint_token_feat.~Dense();
                has_joint_layer = 0;
            }
        }

        State get_initial_state() const
        {
            return { cell.input_size(), cell.h_size() };
        }

        Output apply(State& state, const EmbeddingLookup& embs) const
        {
            thread_local Eigen::VectorXf hidden;
            thread_local Eigen::ArrayXf token_logits;

            if(hidden.size() < cell.h_size()) hidden.resize(cell.h_size());
            layernorm.apply(hidden, cell(state.input_h, state.c_state).bottomRows(cell.h_size()));
            
            Eigen::VectorXf t_logits = token_proj.partial(hidden, 0, approx_size);
            float t_max = t_logits.maxCoeff();
            float t_normalizer = std::log((t_logits.array() - t_max).exp().sum()) + t_max;

            Output ret{ *this, embs,  hidden, t_normalizer };
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
        using Candidate = std::pair<float, RnnCell::DecOutput>;
        using DecSequence = std::pair<float, std::vector<RnnCell::DecOutput>>;

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

        size_t get_unk_token() const { return unk_token; }

        template<typename _Selector>
        std::vector<DecSequence> decode(size_t length, size_t beam_size, _Selector&& selector, bool bidirection = true) const
        {
            struct BeamPath
            {
                float score = 0;
                std::vector<RnnCell::DecOutput> decoded;
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
                    std::vector<Candidate> cands = selector(t, cell.apply(path.state, token_emb));

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
                        RnnCell::Output out = cell_bw.apply(state, token_emb);
                        auto& p = path.decoded[length - t - 1];
                        score += out[p];
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
