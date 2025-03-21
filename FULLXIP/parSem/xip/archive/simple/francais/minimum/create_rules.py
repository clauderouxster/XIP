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

def catpos(cc):
    ps=cc.find("[")
    if ps!=-1:
        return cc[:ps]
    return cc

def comb():
    debug=False
    lescats=[["gn[rel:~]",["pv","nv","conjque[rel:~]"],["?[rel:~]"],None],
             ["gn",["gn"],["coord","gn","punct","gp","gr"],None],
             ["gn",["ga"],["ga","coord","punct","gr","gn"],"#1[gender]:#2[gender]"],
             ["gn",["gp"],["coord","ga","gp","gr"],None],
             ["gn",["conjque"],["coord","gn","gp","punct","gr"],None],
             ["nv",["gn","gp","ga","conjque[rel:~]"],["gn","coord","gp","ga","conjque[rel:~]","punct","gr","nv[aux]"],None],
             ["nv",["pv"],["gn","pv","gp","ga","conjque[rel:~]"],None],
             ["nv[aux:~]",["nv[inf]"],["punct","nv[inf]","coord","gr"],None],
             ["nv[aux]",["nv"],["punct","coord","gr"],None],
             ["ga",["ga"],["punct","coord","gr","ga"],None],
             ["gp",["gp"],[],None],
             ["gp",["ga"],["coord","punct","ga"],None],
             ["gp",["gn"],[],None],
             ["pv",["pv"],["coord","punct","gr","pv"],None],
             ["pv",["gn"],["coord","punct","gr","pv"],None],
             ["pv",["gp"],["coord","punct","gp","gn","gr","pv"],None],
             ["conjque",["nv"],["gn","pv","gp","ga","coord","gr","nv[aux]"],None],
             ["gn[rel]",["nv"],["gn","pv","gp","ga","coord","gr","nv[aux]"],None],
             ["nv",["gr"],[],None],
             ["pv",["gr"],[],None],
             ["gr",["gp","ga","gn","nv","gr"],["gr"],None],
             ["coord",["gn","pv","nv","gp","ga"],[],None]]
    
    

    f=open(r"C:\eclipse\lingveta\francais\minimum\deppositions.xip","w")
    f.write("DependencyRules:\n\n")
    for lc in lescats:
        [c,cats,cond,iftest]=lc
        cf=catpos(c)
        scond=""
        if cond!=[]:
            scond=";".join(cond)
            if scond=="?":
                scond="?*,"
            elif len(cond)==1 and scond[0]=="?":
                scond="?*"+scond[1:]+","
            else:
                if len(cond)==1:
                    scond+="*,"
                else:
                    scond="("+scond+")*,"
        letest=""
        if iftest!=None:
            letest="if ("+iftest+") "
        for cc in cats:
            ccf=cc
            ps=ccf.find("[")
            if ps!=-1:
                ccf=ccf[:ps]
            if c =="coord":
                s="1> |"+c+"#1,"+cc+"{?*,#2[last]}| "+letest+cf+"_"+ccf+"(#1,#2)."
            elif c=="conjque":
                s="1> |"+c+"#1,"+scond+cc+"{?*,#2[last]}| "+letest+cf+"_"+ccf+"(#1,#2)."
            elif cc=="conjque":
                s="1> |"+c+"{?*,#1[last]},"+scond+cc+"#2| "+letest+cf+"_"+ccf+"(#1,#2)."
            else:
                s="1> |"+c+"{?*,#1[last]},"+scond+cc+"{?*,#2[last]}| "+letest+cf+"_"+ccf+"(#1,#2)."
            s=s.upper()
            f.write(s+"\n")
            print s

    f.write("\n\n")

    suite="""        
1> |GN{?*,adj#1,?*,#2[last]}| GA_GN(#1,#2).
1> |GN{?*,det#1,?*,#2[adj:~,last]}| DET_GN(#1,#2).
1> |GN{?*,det#1,?*,adj#2[last]}| DET_GA(#1,#2).
1> |GN{?*,num#1,?*,#2[last]}| NUM_GN(#1,#2).
1> |GN{?*,noun#1,?*,#2[last]}| NOUN_GN(#1,#2).
1> |NV{?*,neg#1,?*,#2[last]}| NEG_NV(#1,#2).
1> |PV{?*,neg#1,?*,#2[last]}| NEG_PV(#1,#2).
1> |GP{?*,num#1,?*,#2[last]}| NUM_GN(#1,#2).
1> |GP{?*,adj#1,?*,#2[last]}| GA_GN(#1,#2).
1> |GP{?*,noun#1,?*,#2[last]}| NOUN_GN(#1,#2).
1> |GP{Prep,?*,det#1,?*,#2[last]}| DET_GN(#1,#2).
1> |GP{Prep#1,?*,#3[last]}}| Prep_GP(#1,#3).
1> |PV{Prep#1,?*,#3[last]}}| Prep_PV(#1,#3).
1> |GN{?*,adv#1,?*[adj:~],adj#2}| GR_GA(#1,#2).
1> |GN{?*,adj#1,?*,adj#2}| GA_GA(#1,#2).
1> |GN{?*,coord#1,adv*,adj#2}| COORD_GA(#1,#2).
1> |NV{?*,pron#1,?*,#2[last]}| GN_NV(#1,#2).
1> |CONJQUE#1,?*[rel=+],NV{?*,#2[last]}| if (CONJQUE_NV(#1,#2) & #2[rel=+]) ~.
1> |CONJQUE#1,?*,NV{?*,#2[last]},GN[rel=+]| if (CONJQUE_NV(#1,#2)) ~.
1> if (NV_NV(#1[rel],#2) & #2[rel=+]) ~.
1> |GN{pron#1[lemme:qui]},?*[rel=+],NV{?*,#2[last]}| if (GN_NV(#1,#2) & #2[rel=+]) ~.
//Si c'est un GP direct apres le verbe avec preposition: de telecharger des contenus
//Alors, on le promeut en GN
1> if (^PV_GP(#1,#2) & ~GN_GP(#3,#2) & PREP_GP(#4[lemme:de],#2)) PV_GN(#1,#2).
1> if (^PREP_GP(#4[lemme:de],#2) & PV_GN(#1,#2)) ~.
1> |?[rel]{?*,#1}| if (#1[rel=+])~.
1> if (^GN_NV(#1[rel:~],#2[rel]) | ^GN_NV(#1[rel],#2[rel:~])) ~.
1> |Verb#1[partpas]| if (^GN_NV(#2,#1) & ~NV_NV(#3,#1)) GN_NV_KER(#2,#1).
    """
    
    f.write(suite)    
    f.close()
    
    f=open(r"C:\eclipse\lingveta\francais\minimum\fonctions.xip","w")
    f.write("functions:\n\n")
    lstcat={}
    for lc in lescats:
        [c,cats,cond,iftest]=lc
        c=catpos(c)
        lstcat[c]=True
        for cc in cats:
            ps=cc.find("[")
            cc=catpos(cc)
            s=c+"_"+cc+","
            s=s.upper()
            f.write(s+"\n")
            print s
    f.write("GR_GA,\n")
    f.write("GA_GN,\n")
    f.write("NOUN_GN,\n")
    f.write("NUM_GN,\n")
    f.write("DET_GN,\n")
    f.write("NEG_PV,\n")
    f.write("NEG_NV,\n")
    f.write("GN_NV_KER,\n")
    f.write("DET_GA,\n")
    f.write("PREP_PV,\n")
    f.write("PREP_GP.\n\n")
    if debug:
        f.write("HIDDEN:\n\n")
        for c in lstcat:
            f.write("X_"+c+",\n")
        f.write("X_GR.\n\n")
    f.close()
    
            
comb()
