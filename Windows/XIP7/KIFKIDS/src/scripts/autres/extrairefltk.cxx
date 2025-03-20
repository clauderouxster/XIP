file ftrad('/Users/rouxclaude/Développement/kifkids/src/sauve/traductions','r');
string s,l,c,sub;

map traducs;
vector vl;

for (s in ftrad) {
    vl=s.trim().split("\t");
    l=vl[0].trim();
    c=vl[1].trim();
    if (traducs.test(l))
        continue;
    traducs[l]=c;
}


function vraichaine(string s) {
	string c;
	if ('%' in s)
		return(false);
    if (s[:3]=="kif") {
        println(s);
        return(false);
    }
    if (s=="r" || s=="w" || s=="W" || s=="H")
        return(true);
    if (s.size()<=1)
        return(false);
	for (c in s) 
		if (c.isalpha())
			return(true);	
	return(false);
}

vector nomsblocs=["KIFLTKSTRINGS"];
vector v=["kifltk.cxx"];

vector vc;

vector chemin=['/Users/rouxclaude/Développement/XIP/xip/dev/src/kifsqlite/'];
vector noms=['/Users/rouxclaude/Développement/kifkids/src/frenchstringsfltk.cxx'];

file loc('/Users/rouxclaude/Développement/kifkids/include/localisationfltk.h','w');

int i=0;
int compte=0;
int nb;

vector chaines;
string sprec;
map deja;
bool debut=false;

loc.write("#ifndef _localisations\r#define _localisations\r\r");
for (c in chemin) {
	file sauve(noms[0],'w');
    sauve.write('#include "kif.h"'+"\r\r\r");
	for (s in v) {
        deja.clear();
        compte=0;
        sauve.write("Exported const char* "+nomsblocs[i]+"[]={\r");	
        loc.write("extern Exported const char* "+nomsblocs[i]+"[];\r"); 
                    
        println(c+s);
		file f(c+s,'r');
		file SNOUVEAU('/Users/rouxclaude/Développement/kifkids/src/'+s,'w');
		nb=0;
		for (l in f) {
			vc=l.extract(0,'"','"');
			if (0!=vc  && "char KIFOS[]" not in l &&  "token" not in l && "#include" not in l && "profilingcall" not in l && "kmap.values[" not in l &&   vc[0]!='0x' && vc[0]!="rb" && l[:2]!='//') {				
				for (sub in vc) {					
					if (not vraichaine(sub))
						continue;
                    if ("&KifCode::" in l && "kifTypes[" not in l)
                        continue;
                    
					if (not deja.test(sub)) {
                        if (traducs.test(sub.trim()))
                            sauve.write('"'+traducs[sub.trim()]+'", //'+compte+" "+s+" "+nb+"\r");
                        else
                            sauve.write('"'+sub+'", //'+compte+" "+s+" "+nb+"     <------- Provide a translation\r");
						deja[sub]=compte;
						compte++;
					}
                        
                    if ('Push("' in l || "CreateXMLNode" in l || "AddAttributeValue" in l) {
                        l=l.replace('"'+sub+'"',"(char*)"+nomsblocs[i]+"["+deja[sub]+"]");										
                    }
                    else
                        l=l.replace('"'+sub+'"',nomsblocs[i]+"["+deja[sub]+"]");										
				}
			}
            if ("kifErrorStrings" in l) {
                if ("const char" in l && "extern" not in l)
                    debut=true;
                l=l.replace("kifErrorStrings","KIFSTRINGS");
            }
            if (debut==false)
                SNOUVEAU.write(l);
            if (l.trim()=="};")
                debut=false;
			nb++;
		}
        SNOUVEAU.close();
        sauve.write("\"\"};\r\r\r");
        i++;
    }
    //sauve.write("\"\"};\r");
    sauve.close();
    loc.write("\r#endif\r\r");
    loc.close();
}
