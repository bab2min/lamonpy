#include <sstream>
#include "LatinLemmatizer.h"
#include "serializer.hpp"

using namespace ll;
using namespace std;

Feature LatinLemmatizer::parse_features(const string& str)
{
	static unordered_map<char, Feature> map = {
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

string LatinLemmatizer::to_string(Feature f)
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

void LatinLemmatizer::load_dictionary(istream& vocab, istream& infl)
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

	while (getline(infl, line))
	{
		istringstream iss{ line };
		string form, lemma, features;
		getline(iss, form, '\t');
		getline(iss, lemma, '\t');
		getline(iss, features, '\t');

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
}


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

void LatinLemmatizer::LemmaInfo::serializerWrite(ostream& ostr) const
{
	serializer::writeMany(ostr, lemma_id, feature);
}

void LatinLemmatizer::LemmaInfo::serializerRead(istream& istr)
{
	serializer::readMany(istr, lemma_id, feature);
}

void LatinLemmatizer::save_model(ostream& ofs) const
{
	serializer::writeMany(ofs, lemmas, form2lemma);
}

void LatinLemmatizer::load_model(istream& ifs)
{
	serializer::readMany(ifs, lemmas, form2lemma);
	
	for (size_t i = 0; i < lemmas.size(); ++i)
	{
		lemma_invmap.emplace(lemmas[i], i);
	}
}

auto LatinLemmatizer::lemmatize(const char* str, size_t len) const -> vector<TokenInfo>
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
				continue;
			}
		}
		
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

auto LatinLemmatizer::lemmatize(const string& str) const -> vector<TokenInfo>
{
	return lemmatize(str.data(), str.size());
}

auto LatinLemmatizer::tag(const LatinRnnModel& tagging_model, 
	const std::string& str, size_t beam_size, 
	bool bidirection) const -> vector<Candidate>
{
	auto tokens = lemmatize(str);
	auto results = tagging_model.decode(tokens.size(), beam_size, [&](size_t t, const RnnCell::Output& r)
	{
		vector<LatinRnnModel::Candidate> ret;
		
		// unknown token
		if (tokens[t].lemma_cands.empty())
		{
			LatinRnnModel::DecOutput dec;
			dec.first = tagging_model.get_unk_token();
			for (size_t i = 0; i < 36; ++i)
			{
				dec.second.gender = i / 12 + 1;
				dec.second.number = (i / 6) % 2 + 1;
				dec.second.case_ = i % 6 + 1;

				float score = r.token_logits[dec.first];
				for (size_t i = 0; i < r.feat_logits.size(); ++i)
				{
					score += r.feat_logits[i][dec.second[i]];
				}
				ret.emplace_back(score, dec);
			}
		}
		// known token
		else
		{
			for (auto& c : tokens[t].lemma_cands)
			{
				LatinRnnModel::DecOutput dec;
				float score = r.token_logits[c.lemma_id];
				dec.first = c.lemma_id;
				dec.second = c.feature;
				for (size_t i = 0; i < r.feat_logits.size(); ++i)
				{
					score += r.feat_logits[i][dec.second[i]];
				}
				ret.emplace_back(score, dec);
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
