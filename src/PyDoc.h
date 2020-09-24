#pragma once
#include <Python.h>

#define DOC_SIGNATURE_EN(name, signature, en) PyDoc_STRVAR(name, signature "\n--\n\n" en)
#define DOC_VARIABLE_EN(name, en) PyDoc_STRVAR(name, en)

DOC_SIGNATURE_EN(Lamon___init____doc__,
	"Lamon(dict_path='dict.bin', tagger_path='tagger.bin', approx_size=2048)",
	u8R""(`Lamon` provides Latin POS tagger & lemmatizer.

Parameters
----------
dict_path : str

tagger_path : str

approx_size : int
)"");

DOC_SIGNATURE_EN(Lamon_list_candidates__doc__,
	"tag(self, text, tag_style='perseus')",
	u8R""(tokenizes `text` and finds candidates of lemma-tag pairs for each token.
Parameters
----------
text : str

tag_style : str

Return
------
candidates : List[Tuple[Int, Int, Tag]]

)"");

DOC_SIGNATURE_EN(Lamon_tag__doc__,
	"tag(self, text, tag_style='perseus', beam_size=1, bidirection=True)",
	u8R""(tokenizes `text` and labels the token sequence by deep model.
Parameters
----------
text : str

tag_style : str

beam_size : int

bidirection : bool

Return
------
result : List[Tuple[float, TaggedSequence]]

)"");
DOC_SIGNATURE_EN(Lamon_tag_multi__doc__,
	"tag_multi(self, texts, tag_style='perseus', beam_size=1, bidirection=True, num_workers=0)",
	u8R""(tokenizes multiple `texts` and labels the token sequences by deep model. It runs on `num_workers` threads.
Parameters
----------
texts : Iterable[str]

tag_style : str

beam_size : int

bidirection : bool

num_workers : int

Return
------
results : Iterable[List[Tuple[float, TaggedSequence]]]

)"");