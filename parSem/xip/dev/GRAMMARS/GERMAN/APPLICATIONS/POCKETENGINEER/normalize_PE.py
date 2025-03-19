Python:
normalize(#1, string _norm).


def featurevalue(node, feature):
    assert feature and \
           feature.find("[") == -1 and \
           feature.find("]") == -1 and \
           feature.find(",") == -1

    # The German grammar sets pythonfullexical(1), so node.features is a list of strings,
    # one for each reading. Not sure what we should do if it's ambiguous; for the moment,
    # arbitrarily taking the first reading.  Same thing for node.lemma later on.
    fstring = node.features[0]

    assert fstring[0] == "["
    fstart = 1
    while fstart < len(fstring):
        fend = fstring.find(":", fstart)
        if fend == -1:
            fend = fstring.find("]", fstart)
        assert fend >= fstart
        vend = fstring.find(",", fend)
        if vend == -1:
            vend = fstring.find("]", fend)
        assert vend >= fend
        if fstring[fstart] == '!':
            fstart = fstart+1
        if fstring[fstart:fend] == feature:
            return fstring[fend+1:vend]
        fstart = vend+1
        
    return None

def finddep (depname, nodeindex, n):
    """Return a list of XipDependency objects with dependency type
    depname, in which the node nodeindex is the nth argument, counting
    from zero."""
    return [XipDependency(i) \
            for i in xips.dependencyset(depname)\
            if xips.dependency(i)[n+2] == nodeindex]


def normalize(nodeindex):
    global _norm
    _norm = ""

    node = XipNode(nodeindex)
    normalize_node(node)

    if _norm == "":
        return 0
    
    return 1

def normalize_nodelist(nodeindex):
    node = XipNode(nodeindex)
    normalize_node(node)
    if node.next != -1:
        normalize_nodelist(node.next)

def normalize_node(node):
    # stop one level above the leaves (leaves are lexical nodes from the HMM).
    if node.daughter != -1 and XipNode(node.daughter).daughter != -1:
        normalize_nodelist(node.daughter)
    else:
        normalize_word(node)
        
    

def normalize_word(node):
    global _norm

    form = featurevalue(node, "FORM")

    if featurevalue(node, "NEG"):
        newword = "NOT"
    elif featurevalue(node, "QUOTE") or featurevalue(node, "LEFTBRACKET") or featurevalue(node, "RIGHTBRACKET"):	# or featurevalue(node, "SGML")
        newword = ""

    elif node.pos in ("DET", "PRON", "CONJ", "PREF", "PTCL", "PREP") or featurevalue(node, "AUX") or featurevalue(node, "MODAL"):	# PREF: if not in rule, compound words will not be in normalized form, if in rule, verbprefixes will be twice in the normalized form, with the verb, and as prefix
        newword = ""
    else:
        # see comment earlier about pythonfullexical.
        newword = node.lemma[0]

    if node.pos=="VERB":
        vprefs = finddep("VPREF", node.index, 0)
        if vprefs:
            newword = XipNode(vprefs[0].parameters[1]).surface + newword


    if _norm and newword and _norm[-1] != " ":
        _norm += " "

   # _norm += newword


    # OK, I hope this is right now...
    # The encoding in which python receives information from XIP is determined
    # by the -outpututf8 flag, and is indicated by sourcecharset.
    # The encoding in which we should pass information back to XIP is the
    # encoding used in the FSTs, and is indicated by fstcharset.
    newword = newword.decode(sourcecharset(current_parseur))
    newword = newword.encode(fstcharset(current_parseur))
    _norm += newword
