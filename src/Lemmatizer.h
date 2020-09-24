#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include "LatinFeat.h"
#include "Latinizer.h"
#include "RnnModel.hpp"

namespace lamon
{
	class Lemmatizer
	{
	public:
		struct LemmaInfo
		{
			uint32_t lemma_id = 0;
			Feature feature;

			LemmaInfo(uint32_t _lemma_id = 0, Feature _feature = {})
				: lemma_id{ _lemma_id }, feature{ _feature }
			{
			}

			bool operator==(const LemmaInfo& o) const
			{
				return lemma_id == o.lemma_id && feature == o.feature;
			}

			void serializerRead(std::istream& istr);
			void serializerWrite(std::ostream& ostr) const;
		};

		struct TokenInfo
		{
			uint32_t start = 0, end = 0;
			std::vector<LemmaInfo> lemma_cands;
			
			TokenInfo(uint32_t _start = 0, uint32_t _end = 0)
				: start{ _start }, end{ _end }
			{
			}
		};

		struct Token : public LemmaInfo
		{
			uint32_t start = 0, end = 0;
			Token(uint32_t _start = 0, uint32_t _end = 0, uint32_t _lemma_id = 0, Feature _feature = {})
				: LemmaInfo{ _lemma_id, _feature }, start{ _start }, end{ _end }
			{
			}
		};

	private:
		std::vector<std::string> lemmas;
		std::vector<char> lemma_pos;
		std::unordered_map<std::string, uint32_t> lemma_invmap;
		std::unordered_map<std::string, std::vector<LemmaInfo>> form2lemma;
		Latinizer latinizer;

	public:
		static Feature parse_features(const std::string& str);
		static std::string to_string(Feature f);
		
		void load_dictionary(std::istream& vocab, std::istream& infl);

		const std::string& get_lemma(uint32_t lemma_id) const
		{
			return lemmas[lemma_id];
		}

		void save_model(std::ostream& ostr) const;
		void load_model(std::istream& istr);

		std::vector<TokenInfo> lemmatize(const char* str, size_t len) const;
		std::vector<TokenInfo> lemmatize(const std::string& str) const;

		using Candidate = std::pair<float, std::vector<Token>>;
		std::vector<Candidate> tag(const LatinRnnModel& tagging_model, const std::string& str, 
			size_t beam_size = 5, bool bidirection = true) const;
	};
}
