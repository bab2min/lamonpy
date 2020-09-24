def test_tag():
    from lamonpy import Lamon
    inst = Lamon()
    print(inst.tag("Aesopus auctor quam materiam repperit Hanc ego polivi versibus senariis"))

def test_tag_multi():
    from lamonpy import Lamon
    inst = Lamon()
    sents = []
    sents.append("Aesopus auctor quam materiam repperit Hanc ego polivi versibus senariis")
    sents.append("A Styge, nate, redis iterum mihi fractaque non semel est mors horrida?")
    sents.append("pervius est Acheron iam languidus et remeare licet soli tibi nec te fata tenent post funera?")
    sents.append("cur te, cur ultima non tenuere tuas umbras loca?")
    sents.append("quid timuere tui manes, precor?")
    sents.append("quicquid in nobis tui mortale fuerat, ignis evictus tulit:")
    sents.append("Peregrinationes meas tu numerasti pone lacrimas meas in utre tuo nonne in supputatione tua?")
    sents.append("Quemadmodum stultus est qui empturus equum non ipsum inspicit sed stratum eius ac frenos, sic stultissimus est qui hominem aut ex veste aut ex conditione, quae nobis vestis modo circumdata est, aestimandum putat.")
    for r in inst.tag(sents):
        print(r)
