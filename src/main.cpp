#include <iostream>
#include <fstream>
#include "Lemmatizer.h"
#include "RnnModel.hpp"
#include "text.hpp"

using namespace std;

int main(int argc, char** argv)
{
	lamon::Lemmatizer lemmatizer;
	lamon::LatinRnnModel tagging_model{ "comp.bidi.uv.quat.bin" };
	
	if(0)
	{
		ifstream vocab{ "comp.vocab.txt" }, infl{ "dict.tsv" };
		lemmatizer.load_dictionary(vocab, infl);

		ofstream ofs{ "comp.dict.bin", ios_base::binary };
		lemmatizer.save_model(ofs);
	}
	else
	{
		ifstream ifs{ "comp.dict.bin", ios_base::binary };
		lemmatizer.load_model(ifs);
	}

	ifstream testset{ "D:/PythonRepo2/parallel_corpus/data/latin_gold.tsv" };
	string line, word;
	struct TestSet
	{
		string sent;
		vector<tuple<string, string, lamon::Feature>> golds;
	};
	vector<TestSet> sets;
	while (getline(testset, line))
	{
		istringstream iss{ line };
		TestSet ts;
		getline(iss, ts.sent, '\t');
		while (getline(iss, word, ' '))
		{
			istringstream issw{ word };
			string field[3];
			getline(issw, field[0], '/');
			getline(issw, field[1], '.');
			getline(issw, field[2]);

			if (field[1].empty()) continue;
			auto feat = lemmatizer.parse_features(field[2]);
			if (feat == lamon::Feature::cases(7)) feat = {};
			ts.golds.emplace_back(field[0], field[1], feat);
		}
		sets.emplace_back(move(ts));
	}

	size_t tot = 0, lcorrect = 0, tcorrect = 0, bcorrect = 0;
	ofstream output{ "D:/PythonRepo2/parallel_corpus/cpp.out" };
	for(auto& ts : sets)
	{
		auto ret = lemmatizer.tag(tagging_model, ts.sent, 20, true)[0].second;
		size_t gidx = 0, ptot = 0, pl = 0, pt = 0, pb = 0;
		for (auto& r : ret)
		{
			output << lemmatizer.get_lemma(r.lemma_id);
			if (r.feature) output << '.' << lemmatizer.to_string(r.feature);
			output << ' ';

			string form = ts.sent.substr(r.start, r.end - r.start);
			if (gidx < ts.golds.size() && form == get<0>(ts.golds[gidx]))
			{
				++ptot;
				bool lc = lemmatizer.get_lemma(r.lemma_id) == get<1>(ts.golds[gidx]);
				if (r.feature == lamon::Feature::cases(7)) r.feature = {};
				bool tc = r.feature == get<2>(ts.golds[gidx]);
				if (lc) ++pl;
				if (tc) ++pt;
				if (lc && tc) ++pb;
				++gidx;
			}
			else continue;
		}
		output << endl;
		tot += ptot;
		lcorrect += pl;
		tcorrect += pt;
		bcorrect += pb;
		/*if (pb / (double)ptot < 0.5)
		{
			cout << ts.sent << endl << "    ";
			for (auto& r : ret)
			{
				cout << lemmatizer.get_lemma(r.lemma_id);
				if (r.feature) cout << '.' << lemmatizer.to_string(r.feature);
				cout << ' ';
			}
			cout << endl;
		}*/
	}
	printf("L:%f (%zd) T:%f (%zd) B:%f (%zd) / total(%zd)\n", lcorrect / (double)tot, lcorrect, tcorrect / (double)tot, tcorrect, bcorrect / (double)tot, bcorrect, tot);
	return 0;
}
