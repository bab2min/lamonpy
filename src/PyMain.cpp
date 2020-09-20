#include <fstream>
#define MAIN_MODULE
#include "PyUtils.h"
#include "LatinLemmatizer.h"

PyObject* gModule;
using namespace std;

struct LatinLemmatizerObject
{
	PyObject_HEAD;
	ll::LatinLemmatizer lemmatizer;

	static int init(LatinLemmatizerObject* self, PyObject* args, PyObject* kwargs)
	{
		new (&self->lemmatizer) ll::LatinLemmatizer{};
		const char* model_path;
		static const char* kwlist[] = { "model_path", nullptr };
		if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s", (char**)kwlist, &model_path)) return -1;
		try
		{
			ifstream ifs{ model_path };
			if (!ifs) throw runtime_error{ string{"Cannot open '"} + model_path + "'" };
			self->lemmatizer = ll::LatinLemmatizer{};
			self->lemmatizer.load_dictionary(ifs);
		}
		catch (const exception& e)
		{
			PyErr_SetString(PyExc_Exception, e.what());
			return -1;
		}
		return 0;
	}

	static void dealloc(LatinLemmatizerObject* self)
	{
		self->lemmatizer.~LatinLemmatizer();
		Py_TYPE(self)->tp_free((PyObject*)self);
	}
};

static PyGetSetDef LatinLemmatizer_getseters[] = {
	//{ (char*)"words", (getter)Document_words, nullptr, Document_words__doc__, nullptr },
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

static PyObject* LL_lemmatize(LatinLemmatizerObject* self, PyObject* args, PyObject* kwargs)
{
	const char* text;
	size_t feature_as_str = 0;
	static const char* kwlist[] = { "text", "feature_as_str", nullptr };
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|p", (char**)kwlist, &text, &feature_as_str)) return nullptr;
	try
	{
		auto ret = self->lemmatizer.lemmatize(text);
		size_t chrs = 0, bytes = 0;
		return py::buildPyValueTransform(ret.begin(), ret.end(), [&](const ll::LatinLemmatizer::TokenInfo& info)
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
				feature_as_str ? 
				py::buildPyValueTransform(info.lemma_cands.begin(), info.lemma_cands.end(), [&](const ll::LatinLemmatizer::LemmaInfo& l)
				{
					return make_tuple(self->lemmatizer.get_lemma(l.lemma_id), self->lemmatizer.to_string(l.feature));
				}) :
				py::buildPyValueTransform(info.lemma_cands.begin(), info.lemma_cands.end(), [&](const ll::LatinLemmatizer::LemmaInfo& l)
				{
					PyObject* f = PyDict_New();
					py::setPyDictItem(f, "number", ll::extract_number(l.feature));
					py::setPyDictItem(f, "case", ll::extract_case(l.feature));
					py::setPyDictItem(f, "gender", ll::extract_gender(l.feature));
					py::setPyDictItem(f, "degree", ll::extract_degree(l.feature));
					py::setPyDictItem(f, "mood", ll::extract_mood(l.feature));
					py::setPyDictItem(f, "voice", ll::extract_voice(l.feature));
					py::setPyDictItem(f, "tense", ll::extract_tense(l.feature));
					py::setPyDictItem(f, "person", ll::extract_person(l.feature));
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

static PyMethodDef LatinLemmatizer_methods[] = {
	{ "lemmatize", (PyCFunction)LL_lemmatize, METH_VARARGS | METH_KEYWORDS, "" },
	{ nullptr },
};

PyTypeObject LatinLemmatizer_type = {
	PyVarObject_HEAD_INIT(nullptr, 0)
	"LatinLemmatizer",             /* tp_name */
	sizeof(LatinLemmatizerObject), /* tp_basicsize */
	0,                         /* tp_itemsize */
	(destructor)LatinLemmatizerObject::dealloc, /* tp_dealloc */
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
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	LatinLemmatizer_methods,             /* tp_methods */
	0,						 /* tp_members */
	LatinLemmatizer_getseters,        /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)LatinLemmatizerObject::init,      /* tp_init */
	PyType_GenericAlloc,
	PyType_GenericNew,
};


PyMODINIT_FUNC PyInit_LatinLemmatizer()
{
	import_array();

	static PyModuleDef mod =
	{
		PyModuleDef_HEAD_INIT,
		"LatinLemmatizer",
		"LatinLemmatizer",
		-1,
		nullptr,
	};

	gModule = PyModule_Create(&mod);
	if (!gModule) return nullptr;
	if (PyType_Ready(&LatinLemmatizer_type) < 0) return nullptr;
	Py_INCREF(&LatinLemmatizer_type);
	PyModule_AddObject(gModule, "LatinLemmatizer", (PyObject*)&LatinLemmatizer_type);
	return gModule;
}
