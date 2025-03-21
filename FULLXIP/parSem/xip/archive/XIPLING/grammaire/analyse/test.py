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
    cats=["gn","pv","nv","gp","ga"]
    catbs=["gn","pv","nv","gp","ga"]
    saute=["conjque","coord"]
    
    for c in cats:
        for cc in catbs:
            if c==cc and c in saute:
                continue
            s=c+"_"+cc+"=[depsup=+],"
            s=s.upper()
            print s
    print "GR=[depsup=+]."        
    for c in cats:
        for cc in catbs:
            if c==cc and c in saute:
                continue
            if c=="coord":
                s="1> |"+c+"#1,(GR),"+cc+"#2| "+c+"_"+cc+"(#1,#2)."
            elif c=="gp":
                s="1> |"+c+"{?*,GP[last]{?*,#1[last]}},?*,"+cc+"#2| "+c+"_"+cc+"(#1,#2)."
            else:
                s="1> |"+c+"{?*,#1[last]},?*,"+cc+"#2| "+c+"_"+cc+"(#1,#2)."
            s=s.upper()
            print s
    print "1> |GR#1| gr(#1)."
        
