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
	if ('%' in s && s.size()<=3)
		return(false);
    if (s[:3]=="kif")
        return(false);
    if (s=="r" || s=="w" || s=="W" || s=="H")
        return(true);
    if (s.size()<=1)
        return(false);
	for (c in s) 
		if (c.isalpha())
			return(true);	
	return(false);
}

vector nomsblocs=["KIFSTRINGS","KIFRAWSTRINGS","KIFLIBSTRINGS","KIFSOCKETSTRINGS","KIFLTKSTRINGS","KIFSYSSTRINGS","KIFSQLITESTRINGS","KIFXMLSTRINGS","KIFREGEXSTRINGS"];
vector v=["kif.cxx","kifraw.cxx","kiflibs.cxx","kifsocket.cxx","kifltk.cxx","kifsys.cxx","kifsqlite.cxx","kifxml.cxx","kifregex.cxx"];

vector vc;

vector chemin=['/Users/rouxclaude/Développement/XIP/xip/dev/src/'];
string autre='/Users/rouxclaude/Documents/ANTKIF/kif/dev/src/';
vector noms=['/Users/rouxclaude/Développement/kifkids/src/frenchstrings.cxx'];

string eng='/Users/rouxclaude/Développement/kifkids/src/englishstrings.cxx';

string frstrsbis=autre+'frenchstrings.cxx';

file loc('/Users/rouxclaude/Développement/kifkids/include/localisations.h','w');

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
    file sauveng(eng,"w");
    file sauvebis(frstrsbis,'w');
    sauvebis.write('#include "kif.h"'+"\r\r\r");
    sauve.write('#include "kif.h"'+"\r\r\r");
	sauve.write('Exported const char* kifwhile="tantque";');
	sauve.write("\r");
	sauve.write('Exported const char* kiffor="pour";');
	sauve.write("\r");
	sauve.write('Exported const char* kifswitch="parmi";');
	sauve.write("\r");
	sauve.write('Exported const char* kifif="si";');
	sauve.write("\r");
	sauve.write('Exported const char* kifelif="sinonsi";');
	sauve.write("\r");
	sauve.write('Exported const char* kifdisplayln="afficheln";');
	sauve.write("\r");
	sauve.write('Exported const char* kifdisplay="affiche";');
	sauve.write("\r");
	sauve.write('Exported const char* kifelse="sinon";');
	sauve.write("\r\r");
    sauveng.write('#include "kif.h"'+"\r\r\r");
	sauveng.write('Exported const char* kifwhile="while";');
	sauveng.write("\r");
	sauveng.write('Exported const char* kiffor="for";');
	sauveng.write("\r");
	sauveng.write('Exported const char* kifif="if";');
	sauveng.write("\r");
	sauveng.write('Exported const char* kifswitch="switch";');
	sauveng.write("\r");
	sauveng.write('Exported const char* kifdisplayln="println";');
	sauveng.write("\r");
	sauveng.write('Exported const char* kifdisplay="print";');
	sauveng.write("\r");
	sauve.write('Exported const char* kifelif="elif";');
	sauve.write("\r");
	sauveng.write('Exported const char* kifelse="else";');
	sauveng.write("\r\r");
	
	for (s in v) {
        deja.clear();
        compte=0;
        sauve.write("Exported const char* "+nomsblocs[i]+"[]={\r");	
        sauveng.write("Exported const char* "+nomsblocs[i]+"[]={\r");	
        sauvebis.write("Exported const char* "+nomsblocs[i]+"[]={\r");	
        loc.write("extern Exported const char* "+nomsblocs[i]+"[];\r"); 
        
        if (s=='kifsocket.cxx') {
            c+='kifsqlite/';
            autre+='kifsqlite/';
        }
            
        println(c+s);
		file f(c+s,'r');
		file SNOUVEAU('/Users/rouxclaude/Développement/kifkids/src/'+s,'w');
		file SNOUVEAUBIS(autre+s,'w');
		nb=0;
		for (l in f) {
			vc=l.extract(0,'"','"');
			if (0!=vc  && "char KIFOS[]" not in l &&  "token" not in l && "#include" not in l && "profilingcall" not in l &&
                vc[0]!='0x' && vc[0]!="rb" && l[:2]!='//') {				
                
                    
				for (sub in vc) {					
					if (not vraichaine(sub))
						continue;
                    
                    if ('kifMetaCharacters[' in l || 'Init_kif' in l || '\t' in l)
                        continue;
                    
                    if ("&KifCode::" in l && "kifTypes[" not in l)
                        continue;
                    
                    if (not deja.test(sub)) {
                        sauveng.write('"'+sub+'", //'+compte+" "+s+" "+nb+"\r");
                        string moteng=sub.trim();
                        if (traducs.test(moteng)) {
                            string trd=traducs[moteng];
                            if (moteng=="run" && nomsblocs[i]!="KIFSTRINGS")
                                trd="lance";
                            if ("(ligne" in trd || trd[:10]=='à la ligne' || trd=='\n\r\t')
                                trd=" "+trd;
                            sauve.write('"'+trd+'", //'+compte+" "+s+" "+nb+"\r");
                            sauvebis.write('"'+trd+'", //'+compte+" "+s+" "+nb+"\r");
                        }
                        else {
                            if ("XIP(" in sub) {
                                sauve.write('"", //'+compte+" "+s+" "+nb+" Leave blank\r");
                                sauvebis.write('"", //'+compte+" "+s+" "+nb+" Leave blank\r");
                            }
                            else {
                                sauve.write('"'+sub+'", //'+compte+" "+s+" "+nb+"     <------- Provide a translation\r");
                                sauvebis.write('"'+sub+'", //'+compte+" "+s+" "+nb+"     <------- Provide a translation\r");
                            }
                        }
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

            if (debut==false) {
                SNOUVEAU.write(l.utf8());
                SNOUVEAUBIS.write(l.utf8());
            }
            if (l.trim()=="};")
                debut=false;
			nb++;
		}
        SNOUVEAU.close();
        SNOUVEAUBIS.close();
        sauve.write("\"\"};\r\r\r");
        sauveng.write("\"\"};\r\r\r");
        sauvebis.write("\"\"};\r\r\r");
        i++;
    }
    //sauve.write("\"\"};\r");
    sauve.close();
    sauvebis.close();
    sauveng.close();
    loc.write("\r#endif\r\r");
    loc.close();
}
