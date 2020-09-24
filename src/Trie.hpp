#pragma once
#include <unordered_map>
#include <deque>
#include <functional>
#include <iterator>

namespace lamon
{
	template<typename _Key, typename _Value, typename _KeyStore>
	struct Trie
	{
		_KeyStore nexts = {};
		_Value val = {};

		Trie() {}
		~Trie() {}

		Trie* getNext(_Key i) const
		{
			return nexts[i] ? (Trie*)this + nexts[i] : nullptr;
		}

		template<typename _AllocFunc>
		void build(const _Key* keys, size_t len, const _Value& _val, _AllocFunc alloc)
		{
			if (!len)
			{
				if (!val) val = _val;
				return;
			}

			if (!getNext(*keys))
			{
				nexts[*keys] = alloc() - this;
			}

			getNext(*keys)->build(keys + 1, len - 1, _val, alloc);
		}

		template<typename _Iterator>
		Trie* findNode(_Iterator begin, _Iterator end)
		{
			if (begin == end) return this;
			auto n = getNext(*begin);
			if (n) return n->findNode(++begin, end);
			return nullptr;
		}

		template<typename _Func>
		void traverse(_Func func)
		{
			if (val)
			{
				if (func(val)) return;
			}
			for (auto& p : nexts)
			{
				if (getNext(p.first))
				{
					getNext(p.first)->traverse(func);
				}
			}
			return;
		}

	};
}