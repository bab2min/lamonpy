#pragma once
#include <array>
#include <algorithm>
#include <vector>
#include <string>
#include <unordered_map>
#include "Trie.hpp"

namespace ll
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
}