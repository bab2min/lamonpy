Lamon, The Latin POS Tagger & Lemmatizer
========================================

.. image:: https://badge.fury.io/py/lamonpy.svg
    :target: https://pypi.python.org/pypi/lamonpy


`Lamon` (LAtin MOrphological tools, pronounced /leɪmən/) is a simple POS tagger & lemmatizer for Latin written in C++ 
and `Lamonpy` is a Python package of Lamon. You can easily obtain lemma and tag of each word in given text using Lamonpy.


Getting Started
---------------
You can install Lamonpy easily using pip. (https://pypi.org/project/lamonpy/)
::

    $ pip install --upgrade pip
    $ pip install lamonpy

The supported OS and Python versions are:

* Linux (x86-64) with Python >= 3.5 
* macOS >= 10.13 with Python >= 3.5
* Windows 7 or later (x86, x86-64) with Python >= 3.5
* Other OS with Python >= 3.5: Compilation from source code required (with c++11 compatible compiler)

Here is a simple example using Lamonpy to analyze Latin texts.
::

    from lamonpy import Lamon
    lamon = Lamon()
    score, tagged = lamon.tag('In principio creavit Deus caelum et terram.')[0]
    print(tagged)
    # `tagged` is a list of tuples `(start_pos, end_pos, lemma, tag)`
    # [(0, 2, 'in', 'r--------'), 
    #  (3, 12, 'principium', 'n-s---nb-'), 
    #  (13, 20, 'creo', 'v3sria---'), 
    #  (21, 25, 'deus', 'n-s---mn-'), 
    #  (26, 32, 'caelus', 'n-s---ma-'), 
    #  (33, 35, 'et', 'c--------'), 
    #  (36, 42, 'terra', 'n-s---fa-'), 
    #  (42, 43, '.', '---------')]

Tagging Model and Its Accuracy
------------------------------
Lamon's tagging model is based on BiLSTM network trained with 
`Perseus Latin Dependency Treebanks <https://perseusdl.github.io/treebank_data/>`_ (4,000 sentences)
and self-trained with raw Latin corpora (440,000 sentences) collected by `Latina Vivense <https://latina.bab2min.pe.kr/xe/text>`_.

Since there is no available standard for evaluating Latin taggers, we built own test set named `vivens` of 900 sentences. The result of evaluation is shown below:

+---------------+---------------------+---------------------+
|               |  vivens (900 sents) | Perseus (4000 sents)|
+               +-------+------+------+-------+------+------+
|               | lemma |  tag | both | lemma |  tag | both |
+===============+=======+======+======+=======+======+======+
|Lamon          |  94.6 | 83.0 | 81.1 |  89.4 | 80.2 | 76.6 |
+---------------+-------+------+------+-------+------+------+
|Lamon (large)  |  94.2 | 83.3 | 81.3 |  89.7 | 81.9 | 78.3 |
+---------------+-------+------+------+-------+------+------+
|Lamon (uv.)    |  94.4 | 82.6 | 80.7 |  87.7 | 77.9 | 73.8 |
+---------------+-------+------+------+-------+------+------+
|Backoff        |  88.1 |      |      |  92.4 |      |      |
+---------------+-------+------+------+-------+------+------+
|123 POS        |       | 58.1 | 54.8 |       | 83.8 | 79.6 |
+---------------+-------+------+------+-------+------+------+
|CRF POS        |       | 69.1 | 63.4 |       | 77.3 | 72.9 |
+---------------+-------+------+------+-------+------+------+

* Lamon : base size (embedding_size:80, hidden_size:160)
* Lamon (large) : large size (embedding_size:160, hidden_size:320)
* Lamon (uv.) : large size without Perseus' dataset
* Backoff : `cltk.lemmatize.latin.backoff.BackoffLatinLemmatizer <https://docs.cltk.org/en/latest/latin.html#lemmatization-backoff-method>`_
* 123 POS : `cltk.tag.pos.POSTag.tag_ngram_123_backoff <https://docs.cltk.org/en/latest/latin.html#gram-backoff-tagger>`_
* CRF POS : `cltk.tag.pos.POSTag.tag_crf <https://docs.cltk.org/en/latest/latin.html#crf-tagger>`_
* For calculating `both` score of `123 POS` and `CRF POS`, `Backoff`'s results are used.

Since Lamon and all cltk's tagger are trained with Perseus' dataset, the scores for Perseus are not significant for confirming the actual accuracy of each model.
Rather, it shows that `123 POS` and `CRF POS` are overfitting to Perseus's dataset.

Because the size of the vivens dataset is small, the results of this evaluation can be inaccurate.
We plan to acquire larger dataset for evaluation and publish the dataset to make more accurate evaluation.

License
-------
`Lamonpy` is licensed under the terms of MIT License, meaning you can use it for any reasonable purpose and remain in complete ownership of all the documentation you produce.

History
-------
* 0.1.0 (2020-09-26)
