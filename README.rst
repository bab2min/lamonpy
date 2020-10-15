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

+-------------------+---------------------+---------------------+
|                   |  vivens (900 sents) | Perseus (4000 sents)|
+                   +-------+------+------+-------+------+------+
|                   | lemma |  tag | both | lemma |  tag | both |
+===================+=======+======+======+=======+======+======+
|**Lamon**          |  94.6 | 83.0 | 81.1 |  89.4 | 80.2 | 76.6 |
+-------------------+-------+------+------+-------+------+------+
|**Lamon (large)**  |  94.2 | 83.3 | 81.3 |  89.7 | 81.9 | 78.3 |
+-------------------+-------+------+------+-------+------+------+
|**Lamon (uv.)**    |  94.4 | 82.6 | 80.7 |  87.7 | 77.9 | 73.8 |
+-------------------+-------+------+------+-------+------+------+
|Backoff            |  88.1 |      |      |  92.4 |      |      |
+-------------------+-------+------+------+-------+------+------+
|123 POS            |       | 58.1 | 54.8 |       | 83.8 | 79.6 |
+-------------------+-------+------+------+-------+------+------+
|CRF POS            |       | 69.1 | 63.4 |       | 77.3 | 72.9 |
+-------------------+-------+------+------+-------+------+------+

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

Tagset
------
Lamon supports three types of tagset.

1. perseus
::

    1:	part of speech

    n	noun
    v	verb
    a	adjective
    d	adverb
    c	conjunction
    r	adposition
    p	pronoun
    m	numeral
    i	interjection
    e	exclamation
    u	punctuation

    2:	person

    1	first person
    2	second person
    3	third person

    3:	number

    s	singular
    p	plural

    4:	tense

    p	present
    i	imperfect
    r	perfect
    l	pluperfect
    t	future perfect
    f	future

    5:	mood

    i	indicative
    s	subjunctive
    n	infinitive
    m	imperative
    p	participle
    d	gerund
    g	gerundive

    6:	voice

    a	active
    p	passive
    d	deponent

    7:	gender
                
    m	masculine
    f	feminine
    n	neuter

    8:	case

    n	nominative
    g	genitive
    d	dative
    a	accusative
    v	vocative
    b	ablative
    l	locative

    9:	degree

    p	positive
    c	comparative
    s	superlative


2. vivens
::

    # Moods
    D: indicative
    S: subjunctive
    I: imperative
    T: infinitive
    L: participle

    # Tenses
    0M: present
    0E: perfect
    RM: imperfect
    RE: pluperfect
    FM: future
    FE: future perfect

    # Voices 
    A: active
    P: passive

    # Participle (combination of mood, tense & voice)
    L0A: present participle
    LRP: past participle
    LFA: future active participle
    LFP: gerundive

    # Persons
    1: first
    2: second
    3: third

    # Genders
    m: masculine
    f: feminine
    n: neuter

    # Numbers
    s: singular
    p: plural

    # Cases
    o: nominative
    g: genitive
    d: dative
    a: accusative
    b: ablative
    v: vocative
    x: adverbial

    # Degrees
    (positive isn't marked explicitly.)
    c: comparative
    u: superlative

    # etc
    r: preposition
    j: conjunction

3. raw
::

    ...

Demo
----
https://latina.bab2min.pe.kr/xe/lTagger (Korean)

Larger Models
-------------
Due to the package size limit of pypi, the distributed wheel package contains `base model` only. We provide larger models by Google-drive links.

* Large model : https://drive.google.com/file/d/1u8LdvD-zKtrj7kDRs6CjQw74ZG6aT8jS/view?usp=sharing
* Large model (unsupervised) : https://drive.google.com/file/d/1nw8LO_1o0O894gXzgQ7Hx5Fyikvy1w2u/view?usp=sharing

You can use these models by passing the model path to `Lamon.__init__` as arguments.
::

    from lamonpy import Lamon
    lamon = Lamon(dict_path='dict.large.bin', tagger_path='tagger.large.bin')

License
-------
`Lamonpy` is licensed under the terms of MIT License, meaning you can use it for any reasonable purpose and remain in complete ownership of all the documentation you produce.

History
-------
* 0.2.0 (2020-10-16)
    * `[NUM]` token for Roman numeral was added.
    * The accuracy was slightly increased by introducing joint lemma-tag layer.
* 0.1.0 (2020-09-26)
    * the first version of `lamonpy`
