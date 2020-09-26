#include <fstream>
#include <iostream>
#define MAIN_MODULE
#include "PyDoc.h"
#include "PyUtils.h"
#include "text.hpp"
#include "RnnModel.hpp"
#include "Lemmatizer.h"
#include "ThreadPool.hpp"


PyObject* gModule;
using namespace std;

string get_module_filename(PyObject* module)
{
	PyObject* path = PyModule_GetFilenameObject(module);
	if (!path) throw bad_exception{};
	string spath = PyUnicode_AsUTF8(path);
	Py_DECREF(path);
	return spath;
}

struct LamonObject
{
	PyObject_HEAD;
	lamon::Lemmatizer lemmatizer;
	lamon::LatinRnnModel* rnn_model;
	ThreadPool* pool;

	static int init(LamonObject* self, PyObject* args, PyObject* kwargs)
	{
		new (&self->lemmatizer) lamon::Lemmatizer{};
		self->rnn_model = nullptr;
		self->pool = nullptr;
		const char* dict_path = "dict.bin";
		const char* tagger_path = "tagger.bin";
		size_t approx_size = 2048;
		static const char* kwlist[] = { "dict_path", "tagger_path", "approx_size", nullptr };
		if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|ssi", (char**)kwlist, 
			&dict_path, &tagger_path, &approx_size)) return -1;
		try
		{
			
			string spath;
			try
			{
				PyObject* module = PyImport_AddModule("lamonpy");
				spath = get_module_filename(module);
				size_t pos1 = spath.rfind('/') + 1, pos2 = spath.rfind('\\') + 1;
				spath = spath.substr(0, max(pos1, pos2));
			}
			catch (const bad_exception&)
			{
				cerr << "[warning] cannot find the path of the module `lamonpy`. " << endl;
				PyErr_Clear();
			}

			ifstream ifs{ dict_path, ios_base::binary };
			if (!ifs)
			{
				ifstream ifs{ spath + dict_path, ios_base::binary };
				if (!ifs) throw runtime_error{ string{"Cannot find '"} + spath + dict_path + "'" };
				self->lemmatizer.load_model(ifs);
			}
			else
			{
				self->lemmatizer.load_model(ifs);
			}
			
			try
			{
				self->rnn_model = new lamon::LatinRnnModel(tagger_path, approx_size);
			}
			catch (const std::ios_base::failure&)
			{
				self->rnn_model = new lamon::LatinRnnModel(spath + tagger_path, approx_size);
			}
		}
		catch (const bad_exception&)
		{
			return -1;
		}
		catch (const exception& e)
		{
			PyErr_SetString(PyExc_Exception, e.what());
			return -1;
		}
		return 0;
	}

	static void dealloc(LamonObject* self)
	{
		self->lemmatizer.~Lemmatizer();
		if (self->rnn_model)
		{
			delete self->rnn_model;
			self->rnn_model = nullptr;
		}
		if (self->pool)
		{
			delete self->pool;
			self->pool = nullptr;
		}
		Py_TYPE(self)->tp_free((PyObject*)self);
	}
};

static PyGetSetDef Lamon_getseters[] = {
	{ nullptr },
};

inline size_t count_uchars(const char* first, const char* last)
{
	size_t n = 0;
	while (first < last)
	{
		unsigned char byte = (unsigned char)*first;
		if ((byte & 0xF8) == 0xF0) first += 4;
		else if ((byte & 0xF0) == 0xE0) first += 3;
		else if ((byte & 0xE0) == 0xC0) first += 2;
		else if ((byte & 0x80) == 0x00) first += 1;
		else throw runtime_error{ "cannot decode `str` into utf-8" };
		++n;
	}
	return n;
}

static PyObject* build_tagged_result(const vector<lamon::Lemmatizer::Candidate>& res, const lamon::Lemmatizer& lemmatizer, const string& text, const string& tag_style)
{
	return py::buildPyValueTransform(res.begin(), res.end(), [&](const lamon::Lemmatizer::Candidate& c)
	{
		size_t chrs = 0, bytes = 0;

		PyObject* cands = (
			tag_style == "vivens" ?
			py::buildPyValueTransform(c.second.begin(), c.second.end(), [&](const lamon::Lemmatizer::Token& t)
			{
				if (bytes <= t.start) chrs += count_uchars(text.data() + bytes, text.data() + t.start);
				else chrs = count_uchars(text.data(), text.data() + t.start);
				uint32_t start = chrs;
				bytes = t.start;
				chrs += count_uchars(text.data() + bytes, text.data() + t.end);
				uint32_t end = chrs;
				bytes = t.end;

				string tag;
				tag.push_back(lemmatizer.get_pos(t.lemma_id));
				if (!tag.back()) tag.pop_back();
				return make_tuple(start, end, lemmatizer.get_lemma(t.lemma_id), tag, lemmatizer.to_vivens_tag(t.feature));
			}) :
			tag_style == "perseus" ?
			py::buildPyValueTransform(c.second.begin(), c.second.end(), [&](const lamon::Lemmatizer::Token& t)
			{
				if (bytes <= t.start) chrs += count_uchars(text.data() + bytes, text.data() + t.start);
				else chrs = count_uchars(text.data(), text.data() + t.start);
				uint32_t start = chrs;
				bytes = t.start;
				chrs += count_uchars(text.data() + bytes, text.data() + t.end);
				uint32_t end = chrs;
				bytes = t.end;

				return make_tuple(start, end, lemmatizer.get_lemma(t.lemma_id), lemmatizer.to_perseus_tag(t.feature, lemmatizer.get_pos(t.lemma_id)));
			}) :
			py::buildPyValueTransform(c.second.begin(), c.second.end(), [&](const lamon::Lemmatizer::Token& t)
			{
				if (bytes <= t.start) chrs += count_uchars(text.data() + bytes, text.data() + t.start);
				else chrs = count_uchars(text.data(), text.data() + t.start);
				uint32_t start = chrs;
				bytes = t.start;
				chrs += count_uchars(text.data() + bytes, text.data() + t.end);
				uint32_t end = chrs;
				bytes = t.end;

				PyObject* f = PyDict_New();
				py::setPyDictItem(f, "mood", t.feature.mood);
				py::setPyDictItem(f, "tense", t.feature.tense);
				py::setPyDictItem(f, "voice", t.feature.voice);
				py::setPyDictItem(f, "person", t.feature.person);
				py::setPyDictItem(f, "gender", t.feature.gender);
				py::setPyDictItem(f, "number", t.feature.number);
				py::setPyDictItem(f, "case", t.feature.case_);
				py::setPyDictItem(f, "degree", t.feature.degree);
				return make_tuple(start, end, lemmatizer.get_lemma(t.lemma_id), f);
			})
		);
		return make_tuple(c.first, cands);
	});
}

static PyObject* LL_list_candidates(LamonObject* self, PyObject* args, PyObject* kwargs)
{
	const char* text;
	const char* tag_style_ = "perseus";
	static const char* kwlist[] = { "text", "tag_style", nullptr };
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|s", (char**)kwlist,
		&text, &tag_style_)) return nullptr;
	try
	{
		string tag_style = tag_style_;
		if (tag_style != "perseus" && tag_style != "vivens" && tag_style != "raw")
		{
			throw runtime_error{
				lamon::text::format("`tag_style` = '%s'. `tag_style` must be 'perseus', 'vivens' or 'raw'!", tag_style_)
			};
		}

		auto ret = self->lemmatizer.lemmatize(text);
		size_t chrs = 0, bytes = 0;
		return py::buildPyValueTransform(ret.begin(), ret.end(), [&](const lamon::Lemmatizer::TokenInfo& info)
		{
			if (bytes <= info.start)
			{
				chrs += count_uchars(text + bytes, text + info.start);
			}
			else
			{
				chrs = count_uchars(text, text + info.start);
			}
			uint32_t start = chrs;
			bytes = info.start;
			chrs += count_uchars(text + bytes, text + info.end);
			uint32_t end = chrs;
			bytes = info.end;

			return make_tuple(start, end,
				tag_style == "vivens" ?
				py::buildPyValueTransform(info.lemma_cands.begin(), info.lemma_cands.end(), [&](const lamon::Lemmatizer::LemmaInfo& l)
				{
					string tag;
					tag.push_back(self->lemmatizer.get_pos(l.lemma_id));
					if (!tag.back()) tag.pop_back();
					return make_tuple(self->lemmatizer.get_lemma(l.lemma_id), tag, self->lemmatizer.to_vivens_tag(l.feature));
				}) :
				tag_style == "perseus" ?
				py::buildPyValueTransform(info.lemma_cands.begin(), info.lemma_cands.end(), [&](const lamon::Lemmatizer::LemmaInfo& l)
				{
					auto pos = self->lemmatizer.get_pos(l.lemma_id);
					return make_tuple(self->lemmatizer.get_lemma(l.lemma_id), self->lemmatizer.to_perseus_tag(l.feature, pos));
				}) :
				py::buildPyValueTransform(info.lemma_cands.begin(), info.lemma_cands.end(), [&](const lamon::Lemmatizer::LemmaInfo& l)
				{
					PyObject* f = PyDict_New();
					py::setPyDictItem(f, "mood", l.feature.mood);
					py::setPyDictItem(f, "tense", l.feature.tense);
					py::setPyDictItem(f, "voice", l.feature.voice);
					py::setPyDictItem(f, "person", l.feature.person);
					py::setPyDictItem(f, "gender", l.feature.gender);
					py::setPyDictItem(f, "number", l.feature.number);
					py::setPyDictItem(f, "case", l.feature.case_);
					py::setPyDictItem(f, "degree", l.feature.degree);
					return make_tuple(self->lemmatizer.get_lemma(l.lemma_id), f);
				})
			);
		});
	}
	catch (const bad_exception&)
	{
		return nullptr;
	}
	catch (const exception& e)
	{
		PyErr_SetString(PyExc_Exception, e.what());
		return nullptr;
	}
}

static PyObject* LL_tag(LamonObject* self, PyObject* args, PyObject* kwargs)
{
	const char* text;
	const char* tag_style = "perseus";
	size_t bidirection = 1, beam_size = 1;
	static const char* kwlist[] = { "text", "tag_style", "beam_size", "bidirection", nullptr };
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|sip", (char**)kwlist, 
		&text, &tag_style, &beam_size, &bidirection)) return nullptr;
	try
	{
		if (tag_style != string{ "perseus" } && tag_style != string{ "vivens" } && tag_style != string{ "raw" })
		{
			throw runtime_error{
				lamon::text::format("`tag_style` = '%s'. `tag_style` must be 'perseus', 'vivens' or 'raw'!", tag_style)
			};
		}

		auto ret = self->lemmatizer.tag(*self->rnn_model, text, max(beam_size, (size_t)10), !!bidirection);
		if(ret.size() > beam_size) ret.erase(ret.begin() + beam_size, ret.end());
		return build_tagged_result(ret, self->lemmatizer, text, tag_style);
	}
	catch (const bad_exception&)
	{
		return nullptr;
	}
	catch (const exception& e)
	{
		PyErr_SetString(PyExc_Exception, e.what());
		return nullptr;
	}
}

struct LamonTagMultiResultObject
{
	PyObject_HEAD;
	LamonObject* lamon;
	vector<future<pair<string, vector<lamon::Lemmatizer::Candidate>>>> futures;
	size_t position;
	string tag_style;

	static int init(LamonTagMultiResultObject* self, PyObject* args, PyObject* kwargs)
	{
		self->lamon = nullptr;
		new (&self->futures) vector<future<pair<string, vector<lamon::Lemmatizer::Candidate>>>>{};
		new (&self->tag_style) string{};
		self->position = 0;
		return 0;
	}

	static PyObject* iter(LamonTagMultiResultObject* self)
	{
		Py_INCREF(self);
		return (PyObject*)self;
	}

	static PyObject* iter_next(LamonTagMultiResultObject* self)
	{
		if (self->position >= self->futures.size()) return nullptr;
		auto p = self->futures[self->position++].get();
		auto& text = p.first;
		auto& result = p.second;
		return build_tagged_result(result, self->lamon->lemmatizer, text, self->tag_style);
	}

	static void dealloc(LamonTagMultiResultObject* self)
	{
		Py_XDECREF(self->lamon);
		self->futures.~vector();
		self->tag_style.~basic_string();
		Py_TYPE(self)->tp_free((PyObject*)self);
	}
};

PyTypeObject LamonTagMultiResult_type = {
	PyVarObject_HEAD_INIT(nullptr, 0)
	"_LamonTagMultiResult",             /* tp_name */
	sizeof(LamonTagMultiResultObject), /* tp_basicsize */
	0,                         /* tp_itemsize */
	(destructor)LamonTagMultiResultObject::dealloc, /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_reserved */
	0, /* tp_repr */
	0,                         /* tp_as_number */
	0,                         /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash  */
	0,                         /* tp_call */
	0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,   /* tp_flags */
	"",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	(getiterfunc)LamonTagMultiResultObject::iter,                         /* tp_iter */
	(iternextfunc)LamonTagMultiResultObject::iter_next,                         /* tp_iternext */
	0,             /* tp_methods */
	0,						 /* tp_members */
	0,        /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)LamonTagMultiResultObject::init,      /* tp_init */
	PyType_GenericAlloc,
	PyType_GenericNew,
};

static PyObject* LL_tag_multi(LamonObject* self, PyObject* args, PyObject* kwargs)
{
	PyObject* texts;
	const char* tag_style = "perseus";
	size_t bidirection = 1, beam_size = 1, num_workers = 0;
	static const char* kwlist[] = { "texts", "tag_style", "beam_size", "bidirection", "num_workers", nullptr };
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|sipi", (char**)kwlist,
		&texts, &tag_style, &beam_size, &bidirection, &num_workers)) return nullptr;
	if (!num_workers) num_workers = thread::hardware_concurrency();

	if (!self->pool || self->pool->getNumWorkers() != num_workers)
	{
		if (self->pool) delete self->pool;
		self->pool = new ThreadPool{ num_workers };
	}

	try
	{
		if (tag_style != string{ "perseus" } && tag_style != string{ "vivens" } && tag_style != string{ "raw" })
		{
			throw runtime_error{
				lamon::text::format("`tag_style` = '%s'. `tag_style` must be 'perseus', 'vivens' or 'raw'!", tag_style)
			};
		}
		if(PyUnicode_Check(texts)) throw runtime_error{ "`texts` must be iterable of str." };
		py::UniqueObj iter = PyObject_GetIter(texts);
		if (!iter) throw runtime_error{ "`texts` must be iterable of str." };
		py::UniqueObj item;
		vector<future<pair<string, vector<lamon::Lemmatizer::Candidate>>>> futures;
		while ((item = PyIter_Next(iter)))
		{
			const char* utf8 = PyUnicode_AsUTF8(item);
			if (!utf8) throw runtime_error{ "`texts` must be iterable of str." };
			futures.emplace_back(self->pool->enqueue([=](size_t thread_id, const string& text)
			{
				auto ret = self->lemmatizer.tag(*self->rnn_model, text, max(beam_size, (size_t)10), !!bidirection);
				if(ret.size() > beam_size) ret.erase(ret.begin() + beam_size, ret.end());
				return make_pair(text, move(ret));
			}, utf8));
		}
		if (PyErr_Occurred()) throw bad_exception{};

		LamonTagMultiResultObject* ret = (LamonTagMultiResultObject*)PyObject_CallObject((PyObject*)&LamonTagMultiResult_type, nullptr);
		Py_INCREF(self);
		ret->lamon = self;
		ret->futures = move(futures);
		ret->tag_style = tag_style;
		return (PyObject*)ret;
	}
	catch (const bad_exception&)
	{
		return nullptr;
	}
	catch (const exception& e)
	{
		PyErr_SetString(PyExc_Exception, e.what());
		return nullptr;
	}
}

static PyMethodDef Lamon_methods[] = {
	{ "list_candidates", (PyCFunction)LL_list_candidates, METH_VARARGS | METH_KEYWORDS, Lamon_list_candidates__doc__ },
	{ "tag", (PyCFunction)LL_tag, METH_VARARGS | METH_KEYWORDS, Lamon_tag__doc__ },
	{ "tag_multi", (PyCFunction)LL_tag_multi, METH_VARARGS | METH_KEYWORDS, Lamon_tag_multi__doc__ },
	{ nullptr },
};

PyTypeObject Lamon_type = {
	PyVarObject_HEAD_INIT(nullptr, 0)
	"Lamon",             /* tp_name */
	sizeof(LamonObject), /* tp_basicsize */
	0,                         /* tp_itemsize */
	(destructor)LamonObject::dealloc, /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_reserved */
	0, /* tp_repr */
	0,                         /* tp_as_number */
	0,                         /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash  */
	0,                         /* tp_call */
	0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,   /* tp_flags */
	Lamon___init____doc__,           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	Lamon_methods,             /* tp_methods */
	0,						 /* tp_members */
	Lamon_getseters,        /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)LamonObject::init,      /* tp_init */
	PyType_GenericAlloc,
	PyType_GenericNew,
};


PyMODINIT_FUNC MODULE_NAME()
{
	import_array();

	static PyModuleDef mod =
	{
		PyModuleDef_HEAD_INIT,
		"_lamonpy",
		"",
		-1,
		nullptr,
	};

	gModule = PyModule_Create(&mod);
	if (!gModule) return nullptr;
	if (PyType_Ready(&Lamon_type) < 0) return nullptr;
	Py_INCREF(&Lamon_type);
	PyModule_AddObject(gModule, "Lamon", (PyObject*)&Lamon_type);
	if (PyType_Ready(&LamonTagMultiResult_type) < 0) return nullptr;
	Py_INCREF(&LamonTagMultiResult_type);
	PyModule_AddObject(gModule, "_LamonTagMultiResult", (PyObject*)&LamonTagMultiResult_type);
	return gModule;
}
