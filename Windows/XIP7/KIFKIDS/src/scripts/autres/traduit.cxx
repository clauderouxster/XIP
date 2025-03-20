file ffrench('/Users/rouxclaude/Développement/kifkids/src/frenchstrings.cxx','r');
file fenglish('/Users/rouxclaude/Développement/kifkids/src/englishstrings.cxx','r');
string s,lf,le;

vector frenchs;
vector englishs;
map mf,me;
int maxi;

vector vfrench,venglish;

for (s in ffrench)
    vfrench.push(s.trim());
for (s in fenglish)
    venglish.push(s.trim());

function teststring(string l) {
    if (l.isalpha() && l.islower())
        return(true);
    int i='_' in l;
    if (i!=-1) {
        l.pop(i);
        if (l.isalpha() && l.islower())
            return(true);        
    }
    return(false);
}

string subf,sube;
string e;
int i=0;
for (s in vfrench) {
    e=venglish[i];
    if ('(' in s && ('):' in s || ') :' in s)) {
        subf=s['"':'('][1:];
        sube=e['"':'('][1:];
        if ('=' in subf) {
            subf=subf['=':][1:].trim();
            sube=sube['=':][1:].trim();
        }
        else
            if (' ' in subf && 'Erreur' not in subf) {
                subf=subf[' ':][1:].trim();
                sube=sube[' ':][1:].trim();                
            }
        frenchs.push(subf);
        englishs.push(sube);
    }
    
    lf=s['"':'"'][1:];
    le=e['"':'"'][1:];
    if (lf.size()>1) {
        if (teststring(lf)) {
            me[le]=lf;
            mf[lf]=le;
        }
    }
    i++;
}

vector tradf;
map trads;
for (s in frenchs) {
    if (mf[s]) {
        tradf.push("."+mf[s]+"(");
        trads["."+mf[s]+"("]="."+s+"(";
        mf.pop(s);
    }
}

mf['défaut']="default";
mf['pas']="not";
mf['sinon']='else';
mf['sinonsi']='elif';

for (s in mf) {
    if (s in ['si','tantque','pour','parmi','sinonsi']) {
        tradf.push(mf[s]+" (");
        trads[mf[s]+" ("]=s+" (";        
    }
    else {
        if (s in ['vrai','faux','renvoie','arrête','défaut','sinon']) {
            tradf.push(mf[s]);
            trads[mf[s]]=s;                
        }
        else {
            tradf.push(mf[s]+" ");
            trads[mf[s]+" "]=s+" ";
        }
    }
}

function compare(string s1,string s2) {
    if (s1.size()>s2.size())
        return(true);
    return(false);
}

tradf.sort(compare);

ffrench.close();
fenglish.close();

map trs;
trs['"r")']="'l')";
trs["'r')"]="'l')";
trs['"w")']="'e')";
trs["'w')"]="'e')";
trs['"w+")']="'e+')";
trs["'w+')"]="'e+')";


file flit(_args[0],"r");
string codes;
for (s in flit) {
    if ("file " in s) {
        for (lf in trs) {
            if (lf in s) {
                s=s.replace(lf,trs[lf]);
                break;
            }
        }
    }            
    codes+=s;
}

i=0;
for (s in tradf) {
    codes=codes.replace(s,trads[s]);
    i++;
}

flit.close();

flit.openwrite(_args[0]+".kif");

flit.write(codes);

flit.close();
