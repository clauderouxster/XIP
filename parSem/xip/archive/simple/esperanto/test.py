def inversio():
    inv={}
    inv["sg"]="pl"
    inv["pl"]="sg"
    node={"sg":"+","lem":"toto"}
    fx="pl"
    if fx in ["sg","pl"]:
        if fx not in node:
            if inv[fx] in node:
                node.pop(inv[fx])
            node[fx]="+"
    print node


def combinaison():
    cats=["gn","pv","nv","gp","ga","conjque","coord"]
    saute=["conjque","coord"]
    
    for c in cats:
        for cc in cats:
            if c==cc and c in saute:
                continue
            s=c+"_"+cc+","
            s=s.upper()
            print s
            
    for c in cats:
        for cc in cats:
            if c==cc and c in saute:
                continue
            s="50> |"+c+"#1,GR*,"+cc+"#2| "+c+"_"+cc+"(#1,#2)."
            s=s.upper()
            print s
            
