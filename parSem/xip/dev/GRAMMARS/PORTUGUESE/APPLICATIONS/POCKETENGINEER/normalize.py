Python:
normalize(#1, string _norm).
    

def featurevalue(node, feature):
    assert feature and \
           feature.find("[") == -1 and \
           feature.find("]") == -1 and \
           feature.find(",") == -1

    fstring = node.features
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
        if fstring[fstart:fend] == feature:
            return fstring[fend+1:vend]
        fstart = vend+1
        
    return None


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
    next = None
    if node.next != -1:
        next = XipNode(node.next)

    # special case for passive subj+verb: normalize in reverse order.
    # I'm being lazy and not checking for the VDOMAIN[passive]
    # dependency, because I know that if a PEUNIT contains an NP and a
    # VERB then it's necessarily passive.
    #if node.pos == "NP" and next and featurevalue(next, "VERB"):
#        normalize_node(next)
#        normalize_node(node)
#        if next.next != -1:
#            normalize_nodelist(next.next)
#    else:
#        normalize_node(node)
#        if node.next != -1:
#            normalize_nodelist(node.next)

def normalize_node(node):
    # stop one level above the leaves (leaves are lexical nodes from the HMM).
    if node.daughter != -1 and XipNode(node.daughter).daughter != -1:
        normalize_nodelist(node.daughter)
    else:
        normalize_word(node)
        
    

def normalize_word(node):
    global _norm

    form = featurevalue(node, "FORM")

    #if form == "FCAN":
     #   newword = "can"
#    elif featurevalue(node, "auxnot"):
#        newword = "not"
    if node.pos in ("PREP", "DET", "AUX", "PRON", "CONJ"): #or \
         #featurevalue(node, "WH") or \
         #form == "FBE":
        newword = ""
    else:
        newword = node.lemma

    if _norm and newword and _norm[-1] != " ":
        _norm += " "

    # OK, I hope this is right now...
    # The encoding in which python receives information from XIP is determined
    # by the -outpututf8 flag, and is indicated by sourcecharset.
    # The encoding in which we should pass information back to XIP is the
    # encoding used in the FSTs, and is indicated by fstcharset.
    newword = newword.decode(sourcecharset(current_parseur))
    newword = newword.lower()
    newword = newword.encode(fstcharset(current_parseur))
    _norm += newword

