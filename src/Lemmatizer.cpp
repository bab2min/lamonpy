#include <sstream>
#include <algorithm>
#include "Lemmatizer.h"
#include "serializer.hpp"

using namespace lamon;
using namespace std;

Feature Lemmatizer::parse_features(const string& str)
{
	const static unordered_map<char, Feature> map = {
		{'s', Feature::numbers(1)},
		{'p', Feature::numbers(2)},
		
		{'m', Feature::genders(1)},
		{'f', Feature::genders(2)},
		{'n', Feature::genders(3)},
		
		{'o', Feature::cases(1)},
		{'g', Feature::cases(2)},
		{'d', Feature::cases(3)},
		{'a', Feature::cases(4)},
		{'b', Feature::cases(5)},
		{'v', Feature::cases(6)},
		{'x', Feature::cases(7)},

		{'r', Feature{}},
		{'i', Feature{}},
		{'j', Feature{}},

		{'c', Feature::degrees(2)},
		{'u', Feature::degrees(3)},

		{'1', Feature::persons(1)},
		{'2', Feature::persons(2)},
		{'3', Feature::persons(3)},

		{'A', Feature::voices(1)},
		{'P', Feature::voices(2)},
		
		{'0', Feature::tenses(1)},
		{'R', Feature::tenses(3)},
		{'F', Feature::tenses(5)},

		{'M', Feature{}},
		{'E', Feature::tenses(1)},

		{'D', Feature::moods(1)},
		{'S', Feature::moods(2)},
		{'I', Feature::moods(3)},
		{'T', Feature::moods(4)},
		{'L', Feature::moods(5)},
		{'N', Feature::moods(6)},
	};

	Feature f;
	for (auto c : str)
	{
		auto it = map.find(c);
		if (it == map.end())
		{
			string err = "unknown character '";
			err.push_back(c);
			err.push_back('\'');
			throw runtime_error{ err };
		}
		f = f + it->second;
		if (c == 'T') f = f + Feature::tenses(1);
	}
	return f;
}

string Lemmatizer::to_vivens_tag(Feature f)
{
	string ret;
	if (f.mood) ret.push_back("DSITLN"[f.mood - 1]);
	if (f.tense)
	{
		ret.push_back("0RF"[(f.tense - 1) / 2]);
		ret.push_back("ME"[(f.tense - 1) % 2]);
	}
	if (f.voice) ret.push_back("AP"[f.voice - 1]);
	if (f.person) ret.push_back("123"[f.person - 1]);
	if (f.gender) ret.push_back("mfn"[f.gender - 1]);
	if (f.number) ret.push_back("sp"[f.number - 1]);
	if (f.case_) ret.push_back("ogdabvx"[f.case_ - 1]);
	if (f.degree > 1) ret.push_back("cu"[f.degree - 2]);

	if (ret.find('L') != ret.npos)
	{
		size_t p;
		if ((p = ret.find('M')) != ret.npos) ret.erase(ret.begin() + p);
		if ((p = ret.find('E')) != ret.npos) ret.erase(ret.begin() + p);
	}
	else if (ret.find('T') != ret.npos)
	{
		size_t p;
		if ((p = ret.find('0')) != ret.npos) ret.erase(ret.begin() + p);
	}

	return ret;
}

string Lemmatizer::to_perseus_tag(Feature f, char pos)
{
	const static unordered_map<char, char> pos_map = {
		{'n', 'n'},
		{'v', 'v'},
		{'a', 'a'},
		{'e', 'd'},
		{'c', 'c'},
		{'p', 'r'},
		{'r', 'p'},
		{'u', 'm'},
		{'i', 'i'},
	};
	string ret;
	auto it = pos_map.find(pos);
	ret.push_back(it == pos_map.end() ? '-' : it->second);
	ret.push_back("-123"[f.person]);
	ret.push_back("-sp"[f.number]);
	// gerundives
	if (f.mood == 6 && f.tense == 5 && f.voice == 2)
	{
		ret.push_back('-');
		ret.push_back('g');
		ret.push_back('-');
	}
	else
	{
		ret.push_back("-prilft"[f.tense]);
		ret.push_back("-ismnp-"[f.mood]);
		ret.push_back("-ap"[f.voice]);
	}
	ret.push_back("-mfn"[f.gender]);
	ret.push_back("-ngdabv-"[f.case_]);
	ret.push_back("-pcs"[pos == 'a' && f.degree == 0 ? 1 : f.degree]);
	return ret;
}

bool Lemmatizer::is_numeral(const string& token)
{
	static string numeral_chrs = "iuxlcdm";

	if (token.empty()) return false;

	if (all_of(token.begin(), token.end(), [](char c) 
	{
		return ('0' <= c && c <= '9') || c == '.' || c == ',';
	}) && any_of(token.begin(), token.end(), [](char c)
	{
		return '0' <= c && c <= '9';
	}))
	{
		return true;
	}

	if (all_of(token.begin(), token.end(), [&](char c)
	{
		return numeral_chrs.find(c) != numeral_chrs.npos;
	}))
	{
		const char* it = token.c_str();
		for (size_t i = 0; i < 4; ++i)
		{
			if (*it != 'm') break;
			++it;
		}

		if (!*it) return true;
		else if (*it == 'c')
		{
			++it;
			if (*it == 'm' || *it == 'd') ++it;
			else if (*it == 'c')
			{
				++it;
				if (*it == 'c') ++it;
			}
		}
		else if (*it == 'd')
		{
			++it;
			for (size_t i = 0; i < 3; ++i)
			{
				if (*it != 'c') break;
				++it;
			}
		}

		if (!*it) return true;
		else if (*it == 'x')
		{
			++it;
			if (*it == 'c' || *it == 'l') ++it;
			else if (*it == 'x')
			{
				++it;
				if (*it == 'x') ++it;
			}
		}
		else if (*it == 'l')
		{
			++it;
			for (size_t i = 0; i < 3; ++i)
			{
				if (*it != 'x') break;
				++it;
			}
		}

		if (!*it) return true;
		else if (*it == 'i')
		{
			++it;
			if (*it == 'x' || *it == 'u') ++it;
			else if (*it == 'i')
			{
				++it;
				if (*it == 'i') ++it;
				if (*it == 'i') ++it;
			}
		}
		else if (*it == 'u')
		{
			++it;
			for (size_t i = 0; i < 3; ++i)
			{
				if (*it != 'i') break;
				++it;
			}
		}

		if (*it) return false;
		return true;
	}

	return false;
}

void Lemmatizer::load_dictionary(istream& vocab, istream& infl)
{
	string line;
	while (getline(vocab, line))
	{
		if (line.empty()) continue;
		if (iswspace(line.back())) line.pop_back();

		if (!lemma_invmap.emplace(line, lemma_invmap.size()).second)
		{
			throw runtime_error{ "`vocab` has a duplicated entry." };
		}
		lemmas.emplace_back(line);
	}

	unordered_map<uint32_t, char> id_to_pos;
	while (getline(infl, line))
	{
		istringstream iss{ line };
		string form, lemma, features, pos;
		getline(iss, form, '\t');
		getline(iss, lemma, '\t');
		getline(iss, features, '\t');
		getline(iss, pos, '\t');

		uint32_t lemma_id;
		auto it = lemma_invmap.find(lemma);
		if (it == lemma_invmap.end())
		{
			lemma_invmap.emplace(lemma, lemma_id = lemma_invmap.size());
			lemmas.emplace_back(lemma);
		}
		else
		{
			lemma_id = it->second;
		}
		id_to_pos[lemma_id] = pos.empty() ? '\0' : pos[0];

		form.erase(
			latinizer.transform(form.begin(), form.end(), form.begin(), Latinizer::tx_integrate),
			form.end()
		);
		LemmaInfo lif{ lemma_id, parse_features(features) };
		if (find(form2lemma[form].begin(), form2lemma[form].end(), lif) == form2lemma[form].end())
		{
			form2lemma[form].emplace_back(lif);
		}
	}

	lemma_pos.resize(lemmas.size());
	for (auto& p : id_to_pos) lemma_pos[p.first] = p.second;

	auto it = lemma_invmap.find("[NUM]");
	if (it != lemma_invmap.end())
	{
		num_tok_id = it->second;
	}
}

template<typename _ChrIt>
inline std::pair<int, int> read_uchar(_ChrIt s)
{
	if ((s[0] & 0x80) == 0x00) return { s[0], 1 };
	if ((s[0] & 0xE0) == 0xC0) return {
		((s[0] & 0x1F) << 6) | (s[1] & 0x3F), 2
	};
	if ((s[0] & 0xF0) == 0xE0) return {
		((s[0] & 0x0F) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F), 3
	};
	if ((s[0] & 0xF8) == 0xF0) return {
		((s[0] & 0x07) << 18) | ((s[1] & 0x3F) << 12) | ((s[2] & 0x3F) << 6) | (s[3] & 0x3F), 4
	};
	return { s[0], 1 };
}

void Lemmatizer::LemmaInfo::serializerWrite(ostream& ostr) const
{
	serializer::writeMany(ostr, lemma_id, feature);
}

void Lemmatizer::LemmaInfo::serializerRead(istream& istr)
{
	serializer::readMany(istr, lemma_id, feature);
}

void Lemmatizer::save_model(ostream& ofs) const
{
	serializer::writeMany(ofs, lemmas, lemma_pos, form2lemma);
}

void Lemmatizer::load_model(istream& ifs)
{
	serializer::readMany(ifs, lemmas, lemma_pos, form2lemma);
	
	for (size_t i = 0; i < lemmas.size(); ++i)
	{
		lemma_invmap.emplace(lemmas[i], i);
	}

	auto it = lemma_invmap.find("[NUM]");
	if (it != lemma_invmap.end())
	{
		num_tok_id = it->second;
	}
}

auto Lemmatizer::lemmatize(const char* str, size_t len) const -> vector<TokenInfo>
{
	vector<TokenInfo> ret;
	if (!len) return ret;

	size_t bpos = 0, epos = 0;
	size_t toks = 0;
	int punc_length = 0;
	while (1)
	{
		if (punc_length)
		{
			bpos = epos;
			epos = bpos + punc_length;

			auto p = read_uchar(&str[epos]);
			punc_length = is_punc(p.first) ? p.second : 0;
		}
		else
		{
			// split by spaces
			for (bpos = epos; bpos < len; )
			{
				auto p = read_uchar(&str[bpos]);
				if (!is_whitespace(p.first)) break;
				bpos += p.second;
			}
			if (bpos >= len) break;
			for (epos = bpos; epos < len; )
			{
				auto p = read_uchar(&str[epos]);
				if (is_whitespace(p.first)) break;
				epos += p.second;
			}
			if (epos >= len) epos = len;

			// split by punctuations or chinese characters
			punc_length = 0;
			for (size_t ppos = bpos; ppos < epos; )
			{
				auto p = read_uchar(&str[ppos]);
				punc_length = is_punc(p.first) ? p.second : 0;
				if (punc_length)
				{
					epos = ppos;
					break;
				}
				ppos += p.second;
			}
		}

		if (bpos == epos) continue;
		string token;
		token.reserve(epos - bpos);
		latinizer.transform(&str[bpos], &str[epos], back_inserter(token), Latinizer::tx_integrate);

		ret.emplace_back(bpos, epos);

		{
			auto it = form2lemma.find(token);
			if (it != form2lemma.end())
			{
				ret.back().lemma_cands = it->second;
			}
		}

		if (num_tok_id && is_numeral(token))
		{
			ret.back().lemma_cands.emplace_back(num_tok_id);
		}

		if (!ret.back().lemma_cands.empty()) continue;
		
		if (token.size() > 3)
		{
			bool changed = false;
			if ((changed = std::equal(token.end() - 3, token.end(), "que")))
			{
				token.erase(token.end() - 3, token.end());
			}
			else if ((changed = std::equal(token.end() - 2, token.end(), "ne")))
			{
				token.erase(token.end() - 2, token.end());
			}
			else if ((changed = std::equal(token.end() - 2, token.end(), "ue")))
			{
				token.erase(token.end() - 2, token.end());
			}

			if (changed)
			{
				auto it = form2lemma.find(token);
				if (it != form2lemma.end()) ret.back().lemma_cands = it->second;
					continue;
			}
		}

		auto it = lemma_invmap.find(token);
		if (it != lemma_invmap.end())
		{
			ret.back().lemma_cands.emplace_back(it->second, Feature{});
		}
	}

	return ret;
}

auto Lemmatizer::lemmatize(const string& str) const -> vector<TokenInfo>
{
	return lemmatize(str.data(), str.size());
}

auto Lemmatizer::tag(const LatinRnnModel& tagging_model, 
	const string& str, size_t beam_size, 
	bool bidirection) const -> vector<Candidate>
{
	auto tokens = lemmatize(str);
	auto results = tagging_model.decode(tokens.size(), beam_size, [&](size_t t, const RnnCell::Output& r)
	{
		vector<LatinRnnModel::Candidate> ret;
		
		// unknown token
		if (tokens[t].lemma_cands.empty())
		{
			RnnCell::DecOutput dec;
			dec.first = tagging_model.get_unk_token();
			if (all_of(&str[tokens[t].start], &str[tokens[t].end], isalpha))
			{
				for (size_t i = 0; i < 36; ++i)
				{
					dec.second.gender = i / 12 + 1;
					dec.second.number = (i / 6) % 2 + 1;
					dec.second.case_ = i % 6 + 1;

					ret.emplace_back(r[dec], dec);
				}
			}
			else
			{
				ret.emplace_back(r[dec], dec);
			}
		}
		// known token
		else
		{
			for (auto& c : tokens[t].lemma_cands)
			{
				RnnCell::DecOutput dec;
				dec.first = c.lemma_id;
				dec.second = c.feature;
				ret.emplace_back(r[dec], dec);
			}
		}
		sort(ret.rbegin(), ret.rend());
		if (ret.size() > beam_size) ret.erase(ret.begin() + beam_size, ret.end());
		return ret;
	}, bidirection);

	vector<Candidate> ret;
	for (auto& r : results)
	{
		vector<Token> toks;
		for (size_t i = 0; i < r.second.size(); ++i)
		{
			toks.emplace_back(tokens[i].start, tokens[i].end,
				r.second[i].first, r.second[i].second
			);
		}
		ret.emplace_back(r.first, move(toks));
	}
	return ret;
}
