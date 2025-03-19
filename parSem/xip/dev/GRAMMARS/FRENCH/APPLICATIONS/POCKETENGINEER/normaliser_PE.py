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
        if fstring[fstart] == '!':
            fstart = fstart+1
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

def normalize_daughters(nodeindex):
    node = XipNode(nodeindex)
    normalize_nodelist([node] + successors(node))

def normalize_nodelist(nodes):
    if nodes:
        if nodes[0].pos == "NP":
            # If there's an NP followed (immedately or not) by a verb
            # in the same PEUNIT, then this is a passive construction.
            # Put the last verb (there could be multiple postposed
            # participles) before the NP to recreate the
            # active form.
            # soit forme passive
            # (être + partpas), soit NP suivis d'un participe
            for next in reversed(nodes[1:]):
                if featurevalue(next, "VERB"):
                    normalize_node(next)
                    nodes.remove(next)
                    break

        normalize_node(nodes[0])
        for next in nodes[1:]:
            normalize_node(next)

def successors (node):
    result = []
    while (node.next != -1):
        newnode = XipNode(node.next)
        result += [newnode]
        node = newnode
    return result


def normalize_node(node):
    # stop one level above the leaves (leaves are lexical nodes from the HMM).
    if node.daughter != -1 and XipNode(node.daughter).daughter != -1:
        normalize_daughters(node.daughter)
    else:
        normalize_word(node)
        
    

def normalize_word(node):
    global _norm
    global current_parseur

    form = featurevalue(node, "FORM")
 
#    if form == "FCAN":
#        newword = "can"
#    elif featurevalue(node, "auxnot"):
#        newword = "not"
    if node.pos in ("PREP", "DET", "PRON", "CONJ", "PUNCT", "COORD") or \
        form == "FETRE"	or \
        form == "FAVOIR"	or \
        node.lemma == "devoir"	or \
        node.lemma == "pouvoir"	or\
        featurevalue(node, "INT"):
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
