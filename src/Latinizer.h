#pragma once
#include <array>
#include <algorithm>
#include <vector>
#include <string>
#include <unordered_map>
#include "Trie.hpp"

namespace lamon
{
	template<typename _Ty, int _size>
	class zero_array : public std::array<_Ty, _size>
	{
	public:
		zero_array() : std::array<_Ty, _size>{}
		{
			std::fill(this->begin(), this->end(), 0);
		}
	};

	class Latinizer
	{
		std::vector<Trie<unsigned char, uint32_t, zero_array<int32_t, 256>>> trie_nodes;
		std::vector<std::string> targets;
		std::unordered_map<std::string, int32_t> target_invmap;
		
		template<int _len>
		void add_char(const char(&cs)[_len], const std::string& target)
		{
			auto it = target_invmap.find(target);
			int32_t tid;
			if (it == target_invmap.end())
			{
				target_invmap.emplace(target, tid = target_invmap.size());
				targets.emplace_back(target);
			}
			else
			{
				tid = it->second;
			}

			const auto& node_alloc = [&]()
			{
				trie_nodes.emplace_back();
				return &trie_nodes.back();
			};

			trie_nodes.reserve(trie_nodes.size() + _len - 1);
			trie_nodes[0].build((unsigned char*)cs, _len - 1, tid + 1, node_alloc);
		}

	public:
		Latinizer()
		{
			trie_nodes.emplace_back();

			add_char("\xc3\x81", "A");
			add_char("\xc4\x82", "A");
			add_char("\xe1\xba\xae", "A");
			add_char("\xe1\xba\xb6", "A");
			add_char("\xe1\xba\xb0", "A");
			add_char("\xe1\xba\xb2", "A");
			add_char("\xe1\xba\xb4", "A");
			add_char("\xc7\x8d", "A");
			add_char("\xc3\x82", "A");
			add_char("\xe1\xba\xa4", "A");
			add_char("\xe1\xba\xac", "A");
			add_char("\xe1\xba\xa6", "A");
			add_char("\xe1\xba\xa8", "A");
			add_char("\xe1\xba\xaa", "A");
			add_char("\xc3\x84", "A");
			add_char("\xc7\x9e", "A");
			add_char("\xc8\xa6", "A");
			add_char("\xc7\xa0", "A");
			add_char("\xe1\xba\xa0", "A");
			add_char("\xc8\x80", "A");
			add_char("\xc3\x80", "A");
			add_char("\xe1\xba\xa2", "A");
			add_char("\xc8\x82", "A");
			add_char("\xc4\x80", "A");
			add_char("\xc4\x84", "A");
			add_char("\xc3\x85", "A");
			add_char("\xc7\xba", "A");
			add_char("\xe1\xb8\x80", "A");
			add_char("\xc8\xba", "A");
			add_char("\xc3\x83", "A");
			add_char("\xea\x9c\xb2", "AA");
			add_char("\xc3\x86", "AE");
			add_char("\xc7\xbc", "AE");
			add_char("\xc7\xa2", "AE");
			add_char("\xea\x9c\xb4", "AO");
			add_char("\xea\x9c\xb6", "AU");
			add_char("\xea\x9c\xb8", "AV");
			add_char("\xea\x9c\xba", "AV");
			add_char("\xea\x9c\xbc", "AY");
			add_char("\xe1\xb8\x82", "B");
			add_char("\xe1\xb8\x84", "B");
			add_char("\xc6\x81", "B");
			add_char("\xe1\xb8\x86", "B");
			add_char("\xc9\x83", "B");
			add_char("\xc6\x82", "B");
			add_char("\xc4\x86", "C");
			add_char("\xc4\x8c", "C");
			add_char("\xc3\x87", "C");
			add_char("\xe1\xb8\x88", "C");
			add_char("\xc4\x88", "C");
			add_char("\xc4\x8a", "C");
			add_char("\xc6\x87", "C");
			add_char("\xc8\xbb", "C");
			add_char("\xc4\x8e", "D");
			add_char("\xe1\xb8\x90", "D");
			add_char("\xe1\xb8\x92", "D");
			add_char("\xe1\xb8\x8a", "D");
			add_char("\xe1\xb8\x8c", "D");
			add_char("\xc6\x8a", "D");
			add_char("\xe1\xb8\x8e", "D");
			add_char("\xc7\xb2", "DZ");
			add_char("\xc7\x85", "DZ");
			add_char("\xc4\x90", "D");
			add_char("\xc6\x8b", "D");
			add_char("\xc7\xb1", "DZ");
			add_char("\xc7\x84", "DZ");
			add_char("\xc3\x89", "E");
			add_char("\xc4\x94", "E");
			add_char("\xc4\x9a", "E");
			add_char("\xc8\xa8", "E");
			add_char("\xe1\xb8\x9c", "E");
			add_char("\xc3\x8a", "E");
			add_char("\xe1\xba\xbe", "E");
			add_char("\xe1\xbb\x86", "E");
			add_char("\xe1\xbb\x80", "E");
			add_char("\xe1\xbb\x82", "E");
			add_char("\xe1\xbb\x84", "E");
			add_char("\xe1\xb8\x98", "E");
			add_char("\xc3\x8b", "E");
			add_char("\xc4\x96", "E");
			add_char("\xe1\xba\xb8", "E");
			add_char("\xc8\x84", "E");
			add_char("\xc3\x88", "E");
			add_char("\xe1\xba\xba", "E");
			add_char("\xc8\x86", "E");
			add_char("\xc4\x92", "E");
			add_char("\xe1\xb8\x96", "E");
			add_char("\xe1\xb8\x94", "E");
			add_char("\xc4\x98", "E");
			add_char("\xc9\x86", "E");
			add_char("\xe1\xba\xbc", "E");
			add_char("\xe1\xb8\x9a", "E");
			add_char("\xea\x9d\xaa", "ET");
			add_char("\xe1\xb8\x9e", "F");
			add_char("\xc6\x91", "F");
			add_char("\xc7\xb4", "G");
			add_char("\xc4\x9e", "G");
			add_char("\xc7\xa6", "G");
			add_char("\xc4\xa2", "G");
			add_char("\xc4\x9c", "G");
			add_char("\xc4\xa0", "G");
			add_char("\xc6\x93", "G");
			add_char("\xe1\xb8\xa0", "G");
			add_char("\xc7\xa4", "G");
			add_char("\xe1\xb8\xaa", "H");
			add_char("\xc8\x9e", "H");
			add_char("\xe1\xb8\xa8", "H");
			add_char("\xc4\xa4", "H");
			add_char("\xe2\xb1\xa7", "H");
			add_char("\xe1\xb8\xa6", "H");
			add_char("\xe1\xb8\xa2", "H");
			add_char("\xe1\xb8\xa4", "H");
			add_char("\xc4\xa6", "H");
			add_char("\xc3\x8d", "I");
			add_char("\xc4\xac", "I");
			add_char("\xc7\x8f", "I");
			add_char("\xc3\x8e", "I");
			add_char("\xc3\x8f", "I");
			add_char("\xe1\xb8\xae", "I");
			add_char("\xc4\xb0", "I");
			add_char("\xe1\xbb\x8a", "I");
			add_char("\xc8\x88", "I");
			add_char("\xc3\x8c", "I");
			add_char("\xe1\xbb\x88", "I");
			add_char("\xc8\x8a", "I");
			add_char("\xc4\xaa", "I");
			add_char("\xc4\xae", "I");
			add_char("\xc6\x97", "I");
			add_char("\xc4\xa8", "I");
			add_char("\xe1\xb8\xac", "I");
			add_char("\xea\x9d\xb9", "D");
			add_char("\xea\x9d\xbb", "F");
			add_char("\xea\x9d\xbd", "G");
			add_char("\xea\x9e\x82", "R");
			add_char("\xea\x9e\x84", "S");
			add_char("\xea\x9e\x86", "T");
			add_char("\xea\x9d\xac", "IS");
			add_char("\xc4\xb4", "J");
			add_char("\xc9\x88", "J");
			add_char("\xe1\xb8\xb0", "K");
			add_char("\xc7\xa8", "K");
			add_char("\xc4\xb6", "K");
			add_char("\xe2\xb1\xa9", "K");
			add_char("\xea\x9d\x82", "K");
			add_char("\xe1\xb8\xb2", "K");
			add_char("\xc6\x98", "K");
			add_char("\xe1\xb8\xb4", "K");
			add_char("\xea\x9d\x80", "K");
			add_char("\xea\x9d\x84", "K");
			add_char("\xc4\xb9", "L");
			add_char("\xc8\xbd", "L");
			add_char("\xc4\xbd", "L");
			add_char("\xc4\xbb", "L");
			add_char("\xe1\xb8\xbc", "L");
			add_char("\xe1\xb8\xb6", "L");
			add_char("\xe1\xb8\xb8", "L");
			add_char("\xe2\xb1\xa0", "L");
			add_char("\xea\x9d\x88", "L");
			add_char("\xe1\xb8\xba", "L");
			add_char("\xc4\xbf", "L");
			add_char("\xe2\xb1\xa2", "L");
			add_char("\xc7\x88", "L");
			add_char("\xc5\x81", "L");
			add_char("\xc7\x87", "LJ");
			add_char("\xe1\xb8\xbe", "M");
			add_char("\xe1\xb9\x80", "M");
			add_char("\xe1\xb9\x82", "M");
			add_char("\xe2\xb1\xae", "M");
			add_char("\xc5\x83", "N");
			add_char("\xc5\x87", "N");
			add_char("\xc5\x85", "N");
			add_char("\xe1\xb9\x8a", "N");
			add_char("\xe1\xb9\x84", "N");
			add_char("\xe1\xb9\x86", "N");
			add_char("\xc7\xb8", "N");
			add_char("\xc6\x9d", "N");
			add_char("\xe1\xb9\x88", "N");
			add_char("\xc8\xa0", "N");
			add_char("\xc7\x8b", "N");
			add_char("\xc3\x91", "N");
			add_char("\xc7\x8a", "NJ");
			add_char("\xc3\x93", "O");
			add_char("\xc5\x8e", "O");
			add_char("\xc7\x91", "O");
			add_char("\xc3\x94", "O");
			add_char("\xe1\xbb\x90", "O");
			add_char("\xe1\xbb\x98", "O");
			add_char("\xe1\xbb\x92", "O");
			add_char("\xe1\xbb\x94", "O");
			add_char("\xe1\xbb\x96", "O");
			add_char("\xc3\x96", "O");
			add_char("\xc8\xaa", "O");
			add_char("\xc8\xae", "O");
			add_char("\xc8\xb0", "O");
			add_char("\xe1\xbb\x8c", "O");
			add_char("\xc5\x90", "O");
			add_char("\xc8\x8c", "O");
			add_char("\xc3\x92", "O");
			add_char("\xe1\xbb\x8e", "O");
			add_char("\xc6\xa0", "O");
			add_char("\xe1\xbb\x9a", "O");
			add_char("\xe1\xbb\xa2", "O");
			add_char("\xe1\xbb\x9c", "O");
			add_char("\xe1\xbb\x9e", "O");
			add_char("\xe1\xbb\xa0", "O");
			add_char("\xc8\x8e", "O");
			add_char("\xea\x9d\x8a", "O");
			add_char("\xea\x9d\x8c", "O");
			add_char("\xc5\x8c", "O");
			add_char("\xe1\xb9\x92", "O");
			add_char("\xe1\xb9\x90", "O");
			add_char("\xc6\x9f", "O");
			add_char("\xc7\xaa", "O");
			add_char("\xc7\xac", "O");
			add_char("\xc3\x98", "O");
			add_char("\xc7\xbe", "O");
			add_char("\xc3\x95", "O");
			add_char("\xe1\xb9\x8c", "O");
			add_char("\xe1\xb9\x8e", "O");
			add_char("\xc8\xac", "O");
			add_char("\xc6\xa2", "OI");
			add_char("\xea\x9d\x8e", "OO");
			add_char("\xc6\x90", "E");
			add_char("\xc6\x86", "O");
			add_char("\xc8\xa2", "OU");
			add_char("\xe1\xb9\x94", "P");
			add_char("\xe1\xb9\x96", "P");
			add_char("\xea\x9d\x92", "P");
			add_char("\xc6\xa4", "P");
			add_char("\xea\x9d\x94", "P");
			add_char("\xe2\xb1\xa3", "P");
			add_char("\xea\x9d\x90", "P");
			add_char("\xea\x9d\x98", "Q");
			add_char("\xea\x9d\x96", "Q");
			add_char("\xc5\x94", "R");
			add_char("\xc5\x98", "R");
			add_char("\xc5\x96", "R");
			add_char("\xe1\xb9\x98", "R");
			add_char("\xe1\xb9\x9a", "R");
			add_char("\xe1\xb9\x9c", "R");
			add_char("\xc8\x90", "R");
			add_char("\xc8\x92", "R");
			add_char("\xe1\xb9\x9e", "R");
			add_char("\xc9\x8c", "R");
			add_char("\xe2\xb1\xa4", "R");
			add_char("\xea\x9c\xbe", "C");
			add_char("\xc6\x8e", "E");
			add_char("\xc5\x9a", "S");
			add_char("\xe1\xb9\xa4", "S");
			add_char("\xc5\xa0", "S");
			add_char("\xe1\xb9\xa6", "S");
			add_char("\xc5\x9e", "S");
			add_char("\xc5\x9c", "S");
			add_char("\xc8\x98", "S");
			add_char("\xe1\xb9\xa0", "S");
			add_char("\xe1\xb9\xa2", "S");
			add_char("\xe1\xb9\xa8", "S");
			add_char("\xe1\xba\x9e", "SS");
			add_char("\xc5\xa4", "T");
			add_char("\xc5\xa2", "T");
			add_char("\xe1\xb9\xb0", "T");
			add_char("\xc8\x9a", "T");
			add_char("\xc8\xbe", "T");
			add_char("\xe1\xb9\xaa", "T");
			add_char("\xe1\xb9\xac", "T");
			add_char("\xc6\xac", "T");
			add_char("\xe1\xb9\xae", "T");
			add_char("\xc6\xae", "T");
			add_char("\xc5\xa6", "T");
			add_char("\xe2\xb1\xaf", "A");
			add_char("\xea\x9e\x80", "L");
			add_char("\xc6\x9c", "M");
			add_char("\xc9\x85", "V");
			add_char("\xea\x9c\xa8", "TZ");
			add_char("\xc3\x9a", "U");
			add_char("\xc5\xac", "U");
			add_char("\xc7\x93", "U");
			add_char("\xc3\x9b", "U");
			add_char("\xe1\xb9\xb6", "U");
			add_char("\xc3\x9c", "U");
			add_char("\xc7\x97", "U");
			add_char("\xc7\x99", "U");
			add_char("\xc7\x9b", "U");
			add_char("\xc7\x95", "U");
			add_char("\xe1\xb9\xb2", "U");
			add_char("\xe1\xbb\xa4", "U");
			add_char("\xc5\xb0", "U");
			add_char("\xc8\x94", "U");
			add_char("\xc3\x99", "U");
			add_char("\xe1\xbb\xa6", "U");
			add_char("\xc6\xaf", "U");
			add_char("\xe1\xbb\xa8", "U");
			add_char("\xe1\xbb\xb0", "U");
			add_char("\xe1\xbb\xaa", "U");
			add_char("\xe1\xbb\xac", "U");
			add_char("\xe1\xbb\xae", "U");
			add_char("\xc8\x96", "U");
			add_char("\xc5\xaa", "U");
			add_char("\xe1\xb9\xba", "U");
			add_char("\xc5\xb2", "U");
			add_char("\xc5\xae", "U");
			add_char("\xc5\xa8", "U");
			add_char("\xe1\xb9\xb8", "U");
			add_char("\xe1\xb9\xb4", "U");
			add_char("\xea\x9d\x9e", "V");
			add_char("\xe1\xb9\xbe", "V");
			add_char("\xc6\xb2", "V");
			add_char("\xe1\xb9\xbc", "V");
			add_char("\xea\x9d\xa0", "VY");
			add_char("\xe1\xba\x82", "W");
			add_char("\xc5\xb4", "W");
			add_char("\xe1\xba\x84", "W");
			add_char("\xe1\xba\x86", "W");
			add_char("\xe1\xba\x88", "W");
			add_char("\xe1\xba\x80", "W");
			add_char("\xe2\xb1\xb2", "W");
			add_char("\xe1\xba\x8c", "X");
			add_char("\xe1\xba\x8a", "X");
			add_char("\xc3\x9d", "Y");
			add_char("\xc5\xb6", "Y");
			add_char("\xc5\xb8", "Y");
			add_char("\xe1\xba\x8e", "Y");
			add_char("\xe1\xbb\xb4", "Y");
			add_char("\xe1\xbb\xb2", "Y");
			add_char("\xc6\xb3", "Y");
			add_char("\xe1\xbb\xb6", "Y");
			add_char("\xe1\xbb\xbe", "Y");
			add_char("\xc8\xb2", "Y");
			add_char("\xc9\x8e", "Y");
			add_char("\xe1\xbb\xb8", "Y");
			add_char("\xc5\xb9", "Z");
			add_char("\xc5\xbd", "Z");
			add_char("\xe1\xba\x90", "Z");
			add_char("\xe2\xb1\xab", "Z");
			add_char("\xc5\xbb", "Z");
			add_char("\xe1\xba\x92", "Z");
			add_char("\xc8\xa4", "Z");
			add_char("\xe1\xba\x94", "Z");
			add_char("\xc6\xb5", "Z");
			add_char("\xc4\xb2", "IJ");
			add_char("\xc5\x92", "OE");
			add_char("\xe1\xb4\x80", "A");
			add_char("\xe1\xb4\x81", "AE");
			add_char("\xca\x99", "B");
			add_char("\xe1\xb4\x83", "B");
			add_char("\xe1\xb4\x84", "C");
			add_char("\xe1\xb4\x85", "D");
			add_char("\xe1\xb4\x87", "E");
			add_char("\xea\x9c\xb0", "F");
			add_char("\xc9\xa2", "G");
			add_char("\xca\x9b", "G");
			add_char("\xca\x9c", "H");
			add_char("\xc9\xaa", "I");
			add_char("\xca\x81", "R");
			add_char("\xe1\xb4\x8a", "J");
			add_char("\xe1\xb4\x8b", "K");
			add_char("\xca\x9f", "L");
			add_char("\xe1\xb4\x8c", "L");
			add_char("\xe1\xb4\x8d", "M");
			add_char("\xc9\xb4", "N");
			add_char("\xe1\xb4\x8f", "O");
			add_char("\xc9\xb6", "OE");
			add_char("\xe1\xb4\x90", "O");
			add_char("\xe1\xb4\x95", "OU");
			add_char("\xe1\xb4\x98", "P");
			add_char("\xca\x80", "R");
			add_char("\xe1\xb4\x8e", "N");
			add_char("\xe1\xb4\x99", "R");
			add_char("\xea\x9c\xb1", "S");
			add_char("\xe1\xb4\x9b", "T");
			add_char("\xe2\xb1\xbb", "E");
			add_char("\xe1\xb4\x9a", "R");
			add_char("\xe1\xb4\x9c", "U");
			add_char("\xe1\xb4\xa0", "V");
			add_char("\xe1\xb4\xa1", "W");
			add_char("\xca\x8f", "Y");
			add_char("\xe1\xb4\xa2", "Z");
			add_char("\xc3\xa1", "a");
			add_char("\xc4\x83", "a");
			add_char("\xe1\xba\xaf", "a");
			add_char("\xe1\xba\xb7", "a");
			add_char("\xe1\xba\xb1", "a");
			add_char("\xe1\xba\xb3", "a");
			add_char("\xe1\xba\xb5", "a");
			add_char("\xc7\x8e", "a");
			add_char("\xc3\xa2", "a");
			add_char("\xe1\xba\xa5", "a");
			add_char("\xe1\xba\xad", "a");
			add_char("\xe1\xba\xa7", "a");
			add_char("\xe1\xba\xa9", "a");
			add_char("\xe1\xba\xab", "a");
			add_char("\xc3\xa4", "a");
			add_char("\xc7\x9f", "a");
			add_char("\xc8\xa7", "a");
			add_char("\xc7\xa1", "a");
			add_char("\xe1\xba\xa1", "a");
			add_char("\xc8\x81", "a");
			add_char("\xc3\xa0", "a");
			add_char("\xe1\xba\xa3", "a");
			add_char("\xc8\x83", "a");
			add_char("\xc4\x81", "a");
			add_char("\xc4\x85", "a");
			add_char("\xe1\xb6\x8f", "a");
			add_char("\xe1\xba\x9a", "a");
			add_char("\xc3\xa5", "a");
			add_char("\xc7\xbb", "a");
			add_char("\xe1\xb8\x81", "a");
			add_char("\xe2\xb1\xa5", "a");
			add_char("\xc3\xa3", "a");
			add_char("\xea\x9c\xb3", "aa");
			add_char("\xc3\xa6", "ae");
			add_char("\xc7\xbd", "ae");
			add_char("\xc7\xa3", "ae");
			add_char("\xea\x9c\xb5", "ao");
			add_char("\xea\x9c\xb7", "au");
			add_char("\xea\x9c\xb9", "av");
			add_char("\xea\x9c\xbb", "av");
			add_char("\xea\x9c\xbd", "ay");
			add_char("\xe1\xb8\x83", "b");
			add_char("\xe1\xb8\x85", "b");
			add_char("\xc9\x93", "b");
			add_char("\xe1\xb8\x87", "b");
			add_char("\xe1\xb5\xac", "b");
			add_char("\xe1\xb6\x80", "b");
			add_char("\xc6\x80", "b");
			add_char("\xc6\x83", "b");
			add_char("\xc9\xb5", "o");
			add_char("\xc4\x87", "c");
			add_char("\xc4\x8d", "c");
			add_char("\xc3\xa7", "c");
			add_char("\xe1\xb8\x89", "c");
			add_char("\xc4\x89", "c");
			add_char("\xc9\x95", "c");
			add_char("\xc4\x8b", "c");
			add_char("\xc6\x88", "c");
			add_char("\xc8\xbc", "c");
			add_char("\xc4\x8f", "d");
			add_char("\xe1\xb8\x91", "d");
			add_char("\xe1\xb8\x93", "d");
			add_char("\xc8\xa1", "d");
			add_char("\xe1\xb8\x8b", "d");
			add_char("\xe1\xb8\x8d", "d");
			add_char("\xc9\x97", "d");
			add_char("\xe1\xb6\x91", "d");
			add_char("\xe1\xb8\x8f", "d");
			add_char("\xe1\xb5\xad", "d");
			add_char("\xe1\xb6\x81", "d");
			add_char("\xc4\x91", "d");
			add_char("\xc9\x96", "d");
			add_char("\xc6\x8c", "d");
			add_char("\xc4\xb1", "i");
			add_char("\xc8\xb7", "j");
			add_char("\xc9\x9f", "j");
			add_char("\xca\x84", "j");
			add_char("\xc7\xb3", "dz");
			add_char("\xc7\x86", "dz");
			add_char("\xc3\xa9", "e");
			add_char("\xc4\x95", "e");
			add_char("\xc4\x9b", "e");
			add_char("\xc8\xa9", "e");
			add_char("\xe1\xb8\x9d", "e");
			add_char("\xc3\xaa", "e");
			add_char("\xe1\xba\xbf", "e");
			add_char("\xe1\xbb\x87", "e");
			add_char("\xe1\xbb\x81", "e");
			add_char("\xe1\xbb\x83", "e");
			add_char("\xe1\xbb\x85", "e");
			add_char("\xe1\xb8\x99", "e");
			add_char("\xc3\xab", "e");
			add_char("\xc4\x97", "e");
			add_char("\xe1\xba\xb9", "e");
			add_char("\xc8\x85", "e");
			add_char("\xc3\xa8", "e");
			add_char("\xe1\xba\xbb", "e");
			add_char("\xc8\x87", "e");
			add_char("\xc4\x93", "e");
			add_char("\xe1\xb8\x97", "e");
			add_char("\xe1\xb8\x95", "e");
			add_char("\xe2\xb1\xb8", "e");
			add_char("\xc4\x99", "e");
			add_char("\xe1\xb6\x92", "e");
			add_char("\xc9\x87", "e");
			add_char("\xe1\xba\xbd", "e");
			add_char("\xe1\xb8\x9b", "e");
			add_char("\xea\x9d\xab", "et");
			add_char("\xe1\xb8\x9f", "f");
			add_char("\xc6\x92", "f");
			add_char("\xe1\xb5\xae", "f");
			add_char("\xe1\xb6\x82", "f");
			add_char("\xc7\xb5", "g");
			add_char("\xc4\x9f", "g");
			add_char("\xc7\xa7", "g");
			add_char("\xc4\xa3", "g");
			add_char("\xc4\x9d", "g");
			add_char("\xc4\xa1", "g");
			add_char("\xc9\xa0", "g");
			add_char("\xe1\xb8\xa1", "g");
			add_char("\xe1\xb6\x83", "g");
			add_char("\xc7\xa5", "g");
			add_char("\xe1\xb8\xab", "h");
			add_char("\xc8\x9f", "h");
			add_char("\xe1\xb8\xa9", "h");
			add_char("\xc4\xa5", "h");
			add_char("\xe2\xb1\xa8", "h");
			add_char("\xe1\xb8\xa7", "h");
			add_char("\xe1\xb8\xa3", "h");
			add_char("\xe1\xb8\xa5", "h");
			add_char("\xc9\xa6", "h");
			add_char("\xe1\xba\x96", "h");
			add_char("\xc4\xa7", "h");
			add_char("\xc6\x95", "hv");
			add_char("\xc3\xad", "i");
			add_char("\xc4\xad", "i");
			add_char("\xc7\x90", "i");
			add_char("\xc3\xae", "i");
			add_char("\xc3\xaf", "i");
			add_char("\xe1\xb8\xaf", "i");
			add_char("\xe1\xbb\x8b", "i");
			add_char("\xc8\x89", "i");
			add_char("\xc3\xac", "i");
			add_char("\xe1\xbb\x89", "i");
			add_char("\xc8\x8b", "i");
			add_char("\xc4\xab", "i");
			add_char("\xc4\xaf", "i");
			add_char("\xe1\xb6\x96", "i");
			add_char("\xc9\xa8", "i");
			add_char("\xc4\xa9", "i");
			add_char("\xe1\xb8\xad", "i");
			add_char("\xea\x9d\xba", "d");
			add_char("\xea\x9d\xbc", "f");
			add_char("\xe1\xb5\xb9", "g");
			add_char("\xea\x9e\x83", "r");
			add_char("\xea\x9e\x85", "s");
			add_char("\xea\x9e\x87", "t");
			add_char("\xea\x9d\xad", "is");
			add_char("\xc7\xb0", "j");
			add_char("\xc4\xb5", "j");
			add_char("\xca\x9d", "j");
			add_char("\xc9\x89", "j");
			add_char("\xe1\xb8\xb1", "k");
			add_char("\xc7\xa9", "k");
			add_char("\xc4\xb7", "k");
			add_char("\xe2\xb1\xaa", "k");
			add_char("\xea\x9d\x83", "k");
			add_char("\xe1\xb8\xb3", "k");
			add_char("\xc6\x99", "k");
			add_char("\xe1\xb8\xb5", "k");
			add_char("\xe1\xb6\x84", "k");
			add_char("\xea\x9d\x81", "k");
			add_char("\xea\x9d\x85", "k");
			add_char("\xc4\xba", "l");
			add_char("\xc6\x9a", "l");
			add_char("\xc9\xac", "l");
			add_char("\xc4\xbe", "l");
			add_char("\xc4\xbc", "l");
			add_char("\xe1\xb8\xbd", "l");
			add_char("\xc8\xb4", "l");
			add_char("\xe1\xb8\xb7", "l");
			add_char("\xe1\xb8\xb9", "l");
			add_char("\xe2\xb1\xa1", "l");
			add_char("\xea\x9d\x89", "l");
			add_char("\xe1\xb8\xbb", "l");
			add_char("\xc5\x80", "l");
			add_char("\xc9\xab", "l");
			add_char("\xe1\xb6\x85", "l");
			add_char("\xc9\xad", "l");
			add_char("\xc5\x82", "l");
			add_char("\xc7\x89", "lj");
			add_char("\xc5\xbf", "s");
			add_char("\xe1\xba\x9c", "s");
			add_char("\xe1\xba\x9b", "s");
			add_char("\xe1\xba\x9d", "s");
			add_char("\xe1\xb8\xbf", "m");
			add_char("\xe1\xb9\x81", "m");
			add_char("\xe1\xb9\x83", "m");
			add_char("\xc9\xb1", "m");
			add_char("\xe1\xb5\xaf", "m");
			add_char("\xe1\xb6\x86", "m");
			add_char("\xc5\x84", "n");
			add_char("\xc5\x88", "n");
			add_char("\xc5\x86", "n");
			add_char("\xe1\xb9\x8b", "n");
			add_char("\xc8\xb5", "n");
			add_char("\xe1\xb9\x85", "n");
			add_char("\xe1\xb9\x87", "n");
			add_char("\xc7\xb9", "n");
			add_char("\xc9\xb2", "n");
			add_char("\xe1\xb9\x89", "n");
			add_char("\xc6\x9e", "n");
			add_char("\xe1\xb5\xb0", "n");
			add_char("\xe1\xb6\x87", "n");
			add_char("\xc9\xb3", "n");
			add_char("\xc3\xb1", "n");
			add_char("\xc7\x8c", "nj");
			add_char("\xc3\xb3", "o");
			add_char("\xc5\x8f", "o");
			add_char("\xc7\x92", "o");
			add_char("\xc3\xb4", "o");
			add_char("\xe1\xbb\x91", "o");
			add_char("\xe1\xbb\x99", "o");
			add_char("\xe1\xbb\x93", "o");
			add_char("\xe1\xbb\x95", "o");
			add_char("\xe1\xbb\x97", "o");
			add_char("\xc3\xb6", "o");
			add_char("\xc8\xab", "o");
			add_char("\xc8\xaf", "o");
			add_char("\xc8\xb1", "o");
			add_char("\xe1\xbb\x8d", "o");
			add_char("\xc5\x91", "o");
			add_char("\xc8\x8d", "o");
			add_char("\xc3\xb2", "o");
			add_char("\xe1\xbb\x8f", "o");
			add_char("\xc6\xa1", "o");
			add_char("\xe1\xbb\x9b", "o");
			add_char("\xe1\xbb\xa3", "o");
			add_char("\xe1\xbb\x9d", "o");
			add_char("\xe1\xbb\x9f", "o");
			add_char("\xe1\xbb\xa1", "o");
			add_char("\xc8\x8f", "o");
			add_char("\xea\x9d\x8b", "o");
			add_char("\xea\x9d\x8d", "o");
			add_char("\xe2\xb1\xba", "o");
			add_char("\xc5\x8d", "o");
			add_char("\xe1\xb9\x93", "o");
			add_char("\xe1\xb9\x91", "o");
			add_char("\xc7\xab", "o");
			add_char("\xc7\xad", "o");
			add_char("\xc3\xb8", "o");
			add_char("\xc7\xbf", "o");
			add_char("\xc3\xb5", "o");
			add_char("\xe1\xb9\x8d", "o");
			add_char("\xe1\xb9\x8f", "o");
			add_char("\xc8\xad", "o");
			add_char("\xc6\xa3", "oi");
			add_char("\xea\x9d\x8f", "oo");
			add_char("\xc9\x9b", "e");
			add_char("\xe1\xb6\x93", "e");
			add_char("\xc9\x94", "o");
			add_char("\xe1\xb6\x97", "o");
			add_char("\xc8\xa3", "ou");
			add_char("\xe1\xb9\x95", "p");
			add_char("\xe1\xb9\x97", "p");
			add_char("\xea\x9d\x93", "p");
			add_char("\xc6\xa5", "p");
			add_char("\xe1\xb5\xb1", "p");
			add_char("\xe1\xb6\x88", "p");
			add_char("\xea\x9d\x95", "p");
			add_char("\xe1\xb5\xbd", "p");
			add_char("\xea\x9d\x91", "p");
			add_char("\xea\x9d\x99", "q");
			add_char("\xca\xa0", "q");
			add_char("\xc9\x8b", "q");
			add_char("\xea\x9d\x97", "q");
			add_char("\xc5\x95", "r");
			add_char("\xc5\x99", "r");
			add_char("\xc5\x97", "r");
			add_char("\xe1\xb9\x99", "r");
			add_char("\xe1\xb9\x9b", "r");
			add_char("\xe1\xb9\x9d", "r");
			add_char("\xc8\x91", "r");
			add_char("\xc9\xbe", "r");
			add_char("\xe1\xb5\xb3", "r");
			add_char("\xc8\x93", "r");
			add_char("\xe1\xb9\x9f", "r");
			add_char("\xc9\xbc", "r");
			add_char("\xe1\xb5\xb2", "r");
			add_char("\xe1\xb6\x89", "r");
			add_char("\xc9\x8d", "r");
			add_char("\xc9\xbd", "r");
			add_char("\xe2\x86\x84", "c");
			add_char("\xea\x9c\xbf", "c");
			add_char("\xc9\x98", "e");
			add_char("\xc9\xbf", "r");
			add_char("\xc5\x9b", "s");
			add_char("\xe1\xb9\xa5", "s");
			add_char("\xc5\xa1", "s");
			add_char("\xe1\xb9\xa7", "s");
			add_char("\xc5\x9f", "s");
			add_char("\xc5\x9d", "s");
			add_char("\xc8\x99", "s");
			add_char("\xe1\xb9\xa1", "s");
			add_char("\xe1\xb9\xa3", "s");
			add_char("\xe1\xb9\xa9", "s");
			add_char("\xca\x82", "s");
			add_char("\xe1\xb5\xb4", "s");
			add_char("\xe1\xb6\x8a", "s");
			add_char("\xc8\xbf", "s");
			add_char("\xc9\xa1", "g");
			add_char("\xc3\x9f", "ss");
			add_char("\xe1\xb4\x91", "o");
			add_char("\xe1\xb4\x93", "o");
			add_char("\xe1\xb4\x9d", "u");
			add_char("\xc5\xa5", "t");
			add_char("\xc5\xa3", "t");
			add_char("\xe1\xb9\xb1", "t");
			add_char("\xc8\x9b", "t");
			add_char("\xc8\xb6", "t");
			add_char("\xe1\xba\x97", "t");
			add_char("\xe2\xb1\xa6", "t");
			add_char("\xe1\xb9\xab", "t");
			add_char("\xe1\xb9\xad", "t");
			add_char("\xc6\xad", "t");
			add_char("\xe1\xb9\xaf", "t");
			add_char("\xe1\xb5\xb5", "t");
			add_char("\xc6\xab", "t");
			add_char("\xca\x88", "t");
			add_char("\xc5\xa7", "t");
			add_char("\xe1\xb5\xba", "th");
			add_char("\xc9\x90", "a");
			add_char("\xe1\xb4\x82", "ae");
			add_char("\xc7\x9d", "e");
			add_char("\xe1\xb5\xb7", "g");
			add_char("\xc9\xa5", "h");
			add_char("\xca\xae", "h");
			add_char("\xca\xaf", "h");
			add_char("\xe1\xb4\x89", "i");
			add_char("\xca\x9e", "k");
			add_char("\xea\x9e\x81", "l");
			add_char("\xc9\xaf", "m");
			add_char("\xc9\xb0", "m");
			add_char("\xe1\xb4\x94", "oe");
			add_char("\xc9\xb9", "r");
			add_char("\xc9\xbb", "r");
			add_char("\xc9\xba", "r");
			add_char("\xe2\xb1\xb9", "r");
			add_char("\xca\x87", "t");
			add_char("\xca\x8c", "v");
			add_char("\xca\x8d", "w");
			add_char("\xca\x8e", "y");
			add_char("\xea\x9c\xa9", "tz");
			add_char("\xc3\xba", "u");
			add_char("\xc5\xad", "u");
			add_char("\xc7\x94", "u");
			add_char("\xc3\xbb", "u");
			add_char("\xe1\xb9\xb7", "u");
			add_char("\xc3\xbc", "u");
			add_char("\xc7\x98", "u");
			add_char("\xc7\x9a", "u");
			add_char("\xc7\x9c", "u");
			add_char("\xc7\x96", "u");
			add_char("\xe1\xb9\xb3", "u");
			add_char("\xe1\xbb\xa5", "u");
			add_char("\xc5\xb1", "u");
			add_char("\xc8\x95", "u");
			add_char("\xc3\xb9", "u");
			add_char("\xe1\xbb\xa7", "u");
			add_char("\xc6\xb0", "u");
			add_char("\xe1\xbb\xa9", "u");
			add_char("\xe1\xbb\xb1", "u");
			add_char("\xe1\xbb\xab", "u");
			add_char("\xe1\xbb\xad", "u");
			add_char("\xe1\xbb\xaf", "u");
			add_char("\xc8\x97", "u");
			add_char("\xc5\xab", "u");
			add_char("\xe1\xb9\xbb", "u");
			add_char("\xc5\xb3", "u");
			add_char("\xe1\xb6\x99", "u");
			add_char("\xc5\xaf", "u");
			add_char("\xc5\xa9", "u");
			add_char("\xe1\xb9\xb9", "u");
			add_char("\xe1\xb9\xb5", "u");
			add_char("\xe1\xb5\xab", "ue");
			add_char("\xea\x9d\xb8", "um");
			add_char("\xe2\xb1\xb4", "v");
			add_char("\xea\x9d\x9f", "v");
			add_char("\xe1\xb9\xbf", "v");
			add_char("\xca\x8b", "v");
			add_char("\xe1\xb6\x8c", "v");
			add_char("\xe2\xb1\xb1", "v");
			add_char("\xe1\xb9\xbd", "v");
			add_char("\xea\x9d\xa1", "vy");
			add_char("\xe1\xba\x83", "w");
			add_char("\xc5\xb5", "w");
			add_char("\xe1\xba\x85", "w");
			add_char("\xe1\xba\x87", "w");
			add_char("\xe1\xba\x89", "w");
			add_char("\xe1\xba\x81", "w");
			add_char("\xe2\xb1\xb3", "w");
			add_char("\xe1\xba\x98", "w");
			add_char("\xe1\xba\x8d", "x");
			add_char("\xe1\xba\x8b", "x");
			add_char("\xe1\xb6\x8d", "x");
			add_char("\xc3\xbd", "y");
			add_char("\xc5\xb7", "y");
			add_char("\xc3\xbf", "y");
			add_char("\xe1\xba\x8f", "y");
			add_char("\xe1\xbb\xb5", "y");
			add_char("\xe1\xbb\xb3", "y");
			add_char("\xc6\xb4", "y");
			add_char("\xe1\xbb\xb7", "y");
			add_char("\xe1\xbb\xbf", "y");
			add_char("\xc8\xb3", "y");
			add_char("\xe1\xba\x99", "y");
			add_char("\xc9\x8f", "y");
			add_char("\xe1\xbb\xb9", "y");
			add_char("\xc5\xba", "z");
			add_char("\xc5\xbe", "z");
			add_char("\xe1\xba\x91", "z");
			add_char("\xca\x91", "z");
			add_char("\xe2\xb1\xac", "z");
			add_char("\xc5\xbc", "z");
			add_char("\xe1\xba\x93", "z");
			add_char("\xc8\xa5", "z");
			add_char("\xe1\xba\x95", "z");
			add_char("\xe1\xb5\xb6", "z");
			add_char("\xe1\xb6\x8e", "z");
			add_char("\xca\x90", "z");
			add_char("\xc6\xb6", "z");
			add_char("\xc9\x80", "z");
			add_char("\xef\xac\x80", "ff");
			add_char("\xef\xac\x83", "ffi");
			add_char("\xef\xac\x84", "ffl");
			add_char("\xef\xac\x81", "fi");
			add_char("\xef\xac\x82", "fl");
			add_char("\xc4\xb3", "ij");
			add_char("\xc5\x93", "oe");
			add_char("\xef\xac\x86", "st");
			add_char("\xe2\x82\x90", "a");
			add_char("\xe2\x82\x91", "e");
			add_char("\xe1\xb5\xa2", "i");
			add_char("\xe2\xb1\xbc", "j");
			add_char("\xe2\x82\x92", "o");
			add_char("\xe1\xb5\xa3", "r");
			add_char("\xe1\xb5\xa4", "u");
			add_char("\xe1\xb5\xa5", "v");
			add_char("\xe2\x82\x93", "x");
			add_char("\xcc\x80", "");
			add_char("\xcc\x81", "");
			add_char("\xcc\x82", "");
			add_char("\xcc\x83", "");
			add_char("\xcc\x84", "");
			add_char("\xcc\x86", "");
			add_char("\xcc\x87", "");
			add_char("\xcc\x88", "");
			add_char("\xcc\x89", "");
			add_char("\xcc\x8a", "");
			add_char("\xcc\x8b", "");
			add_char("\xcc\x8c", "");
			add_char("\xcc\x8f", "");
			add_char("\xcc\x91", "");
			add_char("\xcc\x9b", "");
			add_char("\xcc\xa3", "");
			add_char("\xcc\xa4", "");
			add_char("\xcc\xa5", "");
			add_char("\xcc\xa6", "");
			add_char("\xcc\xa7", "");
			add_char("\xcc\xa8", "");
			add_char("\xcc\xad", "");
			add_char("\xcc\xae", "");
			add_char("\xcc\xb0", "");
			add_char("\xcc\xb1", "");

			target_invmap.clear();
		}

		static char tx_integrate(char x)
		{
			x = std::tolower(x);
			if (x == 'j') return 'i';
			if (x == 'v') return 'u';
			return x;
		}

		template<typename _Input, typename _Output, typename _TxFunc>
		_Output transform(_Input first, _Input last, _Output result, _TxFunc tx) const
		{
			size_t buf_pos = 0;
			char buf[4];
			auto* trie = &trie_nodes[0];
			for (; first != last; ++first)
			{
				auto* next = trie->getNext(*first);
				if (next)
				{
					trie = next;
					buf[buf_pos++] = *first;
					continue;
				}

				if (trie->val)
				{
					auto& t = targets[trie->val - 1];
					result = std::transform(t.begin(), t.end(), result, tx);
				}
				else
				{
					result = std::transform(buf, buf + buf_pos, result, tx);
				}
				trie = trie_nodes[0].getNext(*first);
				buf[0] = *first;
				buf_pos = 1;
				if (!trie)
				{
					trie = &trie_nodes[0];
					buf_pos = 0;
					*result = tx(*first);
					++result;
				}
			}

			if (trie->val)
			{
				auto& t = targets[trie->val - 1];
				result = std::transform(t.begin(), t.end(), result, tx);
			}
			else
			{
				result = std::transform(buf, buf + buf_pos, result, tx);
			}
			return result;
		}

		template<typename _Input, typename _Output>
		_Output transform(_Input first, _Input last, _Output result) const
		{
			return transform(first, last, result, [](char x) { return x; });
		}
	};


	inline bool is_whitespace(int c)
	{
		return c <= 0x20
			|| c == 0x7F
			|| c == 0xA0
			|| c == 0x1680
			|| (c >= 0x2000 && c <= 0x200A)
			|| c == 0x202F
			|| c == 0x205F
			|| c == 0x3000;
	}

	inline bool is_punc(int c)
	{
		static int codes[] = {
			/*33, 34, 35, 37, 38, 39, 40, 41, 42, 44, 45, 46, 47, 58, 59, 63,
			64, 91, 92, 93, 95, 123, 125,*/ 161, 167, 171, 182, 183, 187, 191, 894, 903,
			1370, 1371, 1372, 1373, 1374, 1375, 1417, 1418, 1470, 1472, 1475, 1478, 1523, 1524, 1545, 1546,
			1548, 1549, 1563, 1566, 1567, 1642, 1643, 1644, 1645, 1748, 1792, 1793, 1794, 1795, 1796, 1797,
			1798, 1799, 1800, 1801, 1802, 1803, 1804, 1805, 2039, 2040, 2041, 2096, 2097, 2098, 2099, 2100,
			2101, 2102, 2103, 2104, 2105, 2106, 2107, 2108, 2109, 2110, 2142, 2404, 2405, 2416, 2800, 3572,
			3663, 3674, 3675, 3844, 3845, 3846, 3847, 3848, 3849, 3850, 3851, 3852, 3853, 3854, 3855, 3856,
			3857, 3858, 3860, 3898, 3899, 3900, 3901, 3973, 4048, 4049, 4050, 4051, 4052, 4057, 4058, 4170,
			4171, 4172, 4173, 4174, 4175, 4347, 4960, 4961, 4962, 4963, 4964, 4965, 4966, 4967, 4968, 5120,
			5741, 5742, 5787, 5788, 5867, 5868, 5869, 5941, 5942, 6100, 6101, 6102, 6104, 6105, 6106, 6144,
			6145, 6146, 6147, 6148, 6149, 6150, 6151, 6152, 6153, 6154, 6468, 6469, 6686, 6687, 6816, 6817,
			6818, 6819, 6820, 6821, 6822, 6824, 6825, 6826, 6827, 6828, 6829, 7002, 7003, 7004, 7005, 7006,
			7007, 7008, 7164, 7165, 7166, 7167, 7227, 7228, 7229, 7230, 7231, 7294, 7295, 7360, 7361, 7362,
			7363, 7364, 7365, 7366, 7367, 7379, 8208, 8209, 8210, 8211, 8212, 8213, 8214, 8215, 8216, 8217,
			8218, 8219, 8220, 8221, 8222, 8223, 8224, 8225, 8226, 8227, 8228, 8229, 8230, 8231, 8240, 8241,
			8242, 8243, 8244, 8245, 8246, 8247, 8248, 8249, 8250, 8251, 8252, 8253, 8254, 8255, 8256, 8257,
			8258, 8259, 8261, 8262, 8263, 8264, 8265, 8266, 8267, 8268, 8269, 8270, 8271, 8272, 8273, 8275,
			8276, 8277, 8278, 8279, 8280, 8281, 8282, 8283, 8284, 8285, 8286, 8317, 8318, 8333, 8334, 8968,
			8969, 8970, 8971, 9001, 9002, 10088, 10089, 10090, 10091, 10092, 10093, 10094, 10095, 10096, 10097, 10098,
			10099, 10100, 10101, 10181, 10182, 10214, 10215, 10216, 10217, 10218, 10219, 10220, 10221, 10222, 10223, 10627,
			10628, 10629, 10630, 10631, 10632, 10633, 10634, 10635, 10636, 10637, 10638, 10639, 10640, 10641, 10642, 10643,
			10644, 10645, 10646, 10647, 10648, 10712, 10713, 10714, 10715, 10748, 10749, 11513, 11514, 11515, 11516, 11518,
			11519, 11632, 11776, 11777, 11778, 11779, 11780, 11781, 11782, 11783, 11784, 11785, 11786, 11787, 11788, 11789,
			11790, 11791, 11792, 11793, 11794, 11795, 11796, 11797, 11798, 11799, 11800, 11801, 11802, 11803, 11804, 11805,
			11806, 11807, 11808, 11809, 11810, 11811, 11812, 11813, 11814, 11815, 11816, 11817, 11818, 11819, 11820, 11821,
			11822, 11824, 11825, 11826, 11827, 11828, 11829, 11830, 11831, 11832, 11833, 11834, 11835, 11836, 11837, 11838,
			11839, 11840, 11841, 11842, 11843, 11844, 12289, 12290, 12291, 12296, 12297, 12298, 12299, 12300, 12301, 12302,
			12303, 12304, 12305, 12308, 12309, 12310, 12311, 12312, 12313, 12314, 12315, 12316, 12317, 12318, 12319, 12336,
			12349, 12448, 12539, 42238, 42239, 42509, 42510, 42511, 42611, 42622, 42738, 42739, 42740, 42741, 42742, 42743,
			43124, 43125, 43126, 43127, 43214, 43215, 43256, 43257, 43258, 43260, 43310, 43311, 43359, 43457, 43458, 43459,
			43460, 43461, 43462, 43463, 43464, 43465, 43466, 43467, 43468, 43469, 43486, 43487, 43612, 43613, 43614, 43615,
			43742, 43743, 43760, 43761, 44011, 64830, 64831, 65040, 65041, 65042, 65043, 65044, 65045, 65046, 65047, 65048,
			65049, 65072, 65073, 65074, 65075, 65076, 65077, 65078, 65079, 65080, 65081, 65082, 65083, 65084, 65085, 65086,
			65087, 65088, 65089, 65090, 65091, 65092, 65093, 65094, 65095, 65096, 65097, 65098, 65099, 65100, 65101, 65102,
			65103, 65104, 65105, 65106, 65108, 65109, 65110, 65111, 65112, 65113, 65114, 65115, 65116, 65117, 65118, 65119,
			65120, 65121, 65123, 65128, 65130, 65131, 65281, 65282, 65283, 65285, 65286, 65287, 65288, 65289, 65290, 65292,
			65293, 65294, 65295, 65306, 65307, 65311, 65312, 65339, 65340, 65341, 65343, 65371, 65373, 65375, 65376, 65377,
			65378, 65379, 65380, 65381, 65792, 65793, 65794, 66463, 66512, 66927, 67671, 67871, 67903, 68176, 68177, 68178,
			68179, 68180, 68181, 68182, 68183, 68184, 68223, 68336, 68337, 68338, 68339, 68340, 68341, 68342, 68409, 68410,
			68411, 68412, 68413, 68414, 68415, 68505, 68506, 68507, 68508, 69703, 69704, 69705, 69706, 69707, 69708, 69709,
			69819, 69820, 69822, 69823, 69824, 69825, 69952, 69953, 69954, 69955, 70004, 70005, 70085, 70086, 70087, 70088,
			70089, 70093, 70107, 70109, 70110, 70111, 70200, 70201, 70202, 70203, 70204, 70205, 70313, 70731, 70732, 70733,
			70734, 70735, 70747, 70749, 70854, 71105, 71106, 71107, 71108, 71109, 71110, 71111, 71112, 71113, 71114, 71115,
			71116, 71117, 71118, 71119, 71120, 71121, 71122, 71123, 71124, 71125, 71126, 71127, 71233, 71234, 71235, 71264,
			71265, 71266, 71267, 71268, 71269, 71270, 71271, 71272, 71273, 71274, 71275, 71276, 71484, 71485, 71486, 72769,
			72770, 72771, 72772, 72773, 72816, 72817, 74864, 74865, 74866, 74867, 74868, 92782, 92783, 92917, 92983, 92984,
			92985, 92986, 92987, 92996, 113823, 121479, 121480, 121481, 121482, 121483, 125278, 125279,
		};

		if (c < 128)
		{
			return (33 <= c && c <= 47)
				|| (58 <= c && c <= 64)
				|| (91 <= c && c <= 96)
				|| (123 <= c && c <= 126);
		}

		return std::binary_search(codes, codes + sizeof(codes) / sizeof(codes[0]), c);
	}
	
	inline bool isalpha(int c)
	{
		return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
	}
}