#pragma once

#include <array>
#include <unordered_map>
#include <cmath>
#include <Eigen/Dense>

#include "rfobject.hpp"
#include "mmap.hpp"

namespace ll
{
    template<typename _ArrayTy>
    auto gelu(_ArrayTy&& x) -> decltype(0.f * x * (0.f + (0.f * (x + 0.f * x * x * x)).tanh()))
    {
        const float c1 = std::sqrt(2 / 3.14159265), c2 = 0.044715;
        return 0.5f * x * (1.f + (c1 * (x + c2 * x * x * x)).tanh());
    }

    template<typename _ArrayTy>
    auto sigmoid(_ArrayTy&& x) -> decltype(((-x).exp() + 1).inverse())
    {
        return ((-x).exp() + 1).inverse();
    }

    template<typename _ArrayTy>
    auto logsoftmax(_ArrayTy&& x) -> decltype(x - 0.f)
    {
        auto max = x.maxCoeff();
        max += std::log((x - max).exp().sum());
        return x - max;
    }

    struct EmbeddingLookup
    {
        ConstMatrix<float> embs;
        EmbeddingLookup(const utils::Object& o)
            : embs{ o.template to_matrix<float>() }
        {
        }

        EmbeddingLookup(const ConstMatrix<float>& o)
            : embs{ o }
        {
        }

        size_t get_embedding_size() const { return embs.rows(); }
        size_t get_vocab_size() const { return embs.cols(); }

        auto operator[](size_t idx) const -> decltype(embs.col(idx))
        {
            return embs.col(idx);
        }
    };

    struct LayerNorm
    {
        ConstVector<float> beta;
        ConstVector<float> gamma;

        LayerNorm(const utils::ObjectCollection& objs, const std::string& keys)
            : beta{ objs[keys + "/beta:0"].template to_vector<float>() },
            gamma{ objs[keys + "/gamma:0"].template to_vector<float>() }
        {
        }

        template<typename _DestTy>
        void apply_inplace(_DestTy&& dest) const
        {
            int cnt = dest.rows();
            for (int i = 0; i < dest.cols(); ++i)
            {
                auto col = dest.col(i);
                float avg = col.sum() / cnt;
                float std = std::sqrt(((col.array() * col.array()).sum() / cnt - avg * avg) + 1e-12f);
                col = (col.array() - avg) / std * gamma.array() + beta.array();
            }
        }

        template<typename _DestTy, typename _SrcTy>
        void apply(_DestTy&& dest, _SrcTy&& src) const
        {
            int cnt = src.rows();
            for (int i = 0; i < src.cols(); ++i)
            {
                auto col = src.col(i);
                float avg = col.sum() / cnt;
                float std = std::sqrt(((col.array() * col.array()).sum() / cnt - avg * avg) + 1e-12f);
                dest.col(i) = (col.array() - avg) / std * gamma.array() + beta.array();
            }
        }
    };

    struct Dense
    {
        ConstMatrix<float> kernel;
        ConstVector<float> bias;

        Dense(const utils::ObjectCollection& objs, const std::string& keys)
            : kernel{ objs[keys + "/kernel:0"].template to_matrix<float>() },
            bias{ objs[keys + "/bias:0"].template to_vector<float>() }
        {
        }

        template<typename _EigenTy>
        auto operator()(_EigenTy&& x) const
            -> decltype((kernel.transpose()* x).colwise() + bias)
        {
            return (kernel.transpose() * x).colwise() + bias;
        }

        template<typename _EigenTy>
        auto partial(_EigenTy&& x, size_t begin, size_t size) const
            -> decltype((kernel.middleCols(begin, size).transpose()* x).colwise() + bias.segment(begin, size))
        {
            return (kernel.middleCols(begin, size).transpose() * x).colwise() + bias.segment(begin, size);
        }

        template<typename _EigenTy>
        float partial(_EigenTy&& x, size_t idx) const
        {
            return (kernel.col(idx).transpose() * x)(0) + bias(idx);
        }
    };

    struct LSTMCell : public Dense
    {
        LSTMCell(const utils::ObjectCollection& objs, const std::string& keys)
            : Dense{ objs, keys }
        {
        }

        size_t h_size() const
        {
            return bias.size() / 4;
        }

        size_t input_size() const
        {
            return kernel.rows() - h_size();
        }

        template<typename _EigenTy1, typename _EigenTy2, typename _EigenTy3>
        auto operator()(_EigenTy1&& input, _EigenTy2& c_state, _EigenTy3& h_state) const
        {
            Eigen::VectorXf gates = ((kernel.topRows(input.rows()).transpose() * input) + kernel.bottomRows(h_state.rows()).transpose() * h_state).colwise() + bias;
            const size_t gate_size = h_size();
            auto input_gate = gates.middleRows(0, gate_size).array();
            auto new_input = gates.middleRows(gate_size, gate_size).array();
            auto forget_gate = gates.middleRows(gate_size * 2, gate_size).array();
            auto output_gate = gates.middleRows(gate_size * 3, gate_size).array();

            c_state = (c_state.array() * sigmoid(forget_gate + 1)) + (sigmoid(input_gate) * new_input.tanh());
            h_state = c_state.array().tanh() * sigmoid(output_gate);
            return h_state;
        }

        template<typename _EigenTy1, typename _EigenTy2>
        _EigenTy1& operator()(_EigenTy1& input_h, _EigenTy2& c_state) const
        {
            Eigen::VectorXf gates = (kernel.transpose() * input_h).colwise() + bias;
            const size_t gate_size = h_size();
            auto input_gate = gates.middleRows(0, gate_size).array();
            auto new_input = gates.middleRows(gate_size, gate_size).array();
            auto forget_gate = gates.middleRows(gate_size * 2, gate_size).array();
            auto output_gate = gates.middleRows(gate_size * 3, gate_size).array();

            c_state = (c_state.array() * sigmoid(forget_gate + 1)) + (sigmoid(input_gate) * new_input.tanh());
            input_h.bottomRows(gate_size) = c_state.array().tanh() * sigmoid(output_gate);
            return input_h;
        }
    };
}
