
//parser lingveta;
//lingveta.load('C:\eclipse\XIPLING\grammaire\lingveta.grm');
use(_args[0],"",'kifltk');
use(_args[0],"debug",'c:\XIP\Debug\kifltk.dll');

function displaygc() {
    printlnerr("Garbaging:",gcsize());
}

garbagefunction(displaygc);

const string endl="\r";

map choix={'rigarde':'vide'};

loadin('generationlingveta/ending.cxx');

fst lfrancais('lingvatafrench.fst',"",0,false);
fst lenglish('lingvataenglish.fst',"",0,false);
 
fst analyse('ending.fst',"",0,false);
parser genere('generationlingveta/lingveta.grm');
parser francais('analyse/french.grm');
parser analyselingveta('lingveta.grm');
parser generefrancais('generationfrancais/french.grm');
parser genereenglish('englishgenerate/english.grm');

genere.addoption(XIP_ENABLE_DEPENDENCY,XIP_CHECK_INPUT_UTF8);
francais.addoption(XIP_ENABLE_DEPENDENCY,XIP_CHECK_INPUT_UTF8);
analyselingveta.addoption(XIP_ENABLE_DEPENDENCY,XIP_CHECK_INPUT_UTF8);
generefrancais.addoption(XIP_ENABLE_DEPENDENCY,XIP_CHECK_INPUT_UTF8);
genereenglish.addoption(XIP_ENABLE_DEPENDENCY,XIP_CHECK_INPUT_UTF8);

genere.removeoption(XIP_CHUNK_TREE);
genere.removeoption(XIP_CHUNK_TREE);
francais.removeoption(XIP_CHUNK_TREE);
analyselingveta.removeoption(XIP_CHUNK_TREE);
generefrancais.removeoption(XIP_CHUNK_TREE);
genereenglish.removeoption(XIP_CHUNK_TREE);

vector resanalyse;
vector lingvetares;

function setsentence(string s) {
	resanalyse.push(s);
}

function clear() {
	resanalyse.clear();
	lingvetares.clear();
}

bool fullfeat=false;

function ajoutetraits(map f1) {
	string res;
	iterator it=f1;
	bool beg=true;
	for (it.begin();it.nend();it.next()) {
		string k=it.key().lower();
		if (k in ["first","terminal","last","noambiguity"])
			continue;
		if (beg)
			beg=false;
		else
			res+=",";
		res+='"'+k+'":"'+it.value()+'"';
	}
	return(res);
}

function appellingveta(string name,map d,map f1,map f2,int idn1,int idn2,string pos1,string pos2) {	
	string res=name+"(0,";
	res+=pos1+"#"+idn1+"[";
	res+=ajoutetraits(f1);
	res+="],";
	res+=pos2+"#"+idn2+"[";
	res+=ajoutetraits(f2);
	res+="]).";
	lingvetares.push(res);
}

function appelunique(string name,map d,map f,int idn,string pos) {
	string res=name+"(0,"+pos+"#"+idn+"["+ajoutetraits(f)+"]).";
	lingvetares.push(res);
}

function generate(string r) {
	francais.generatefromstring(r);
}


function francaisverslingvata(vector vracine,vector tous) {
	string m;
	vector souschaine;
	vector traductions=lfrancais.down(vracine[0].lower(),"+Fr");
	if (traductions[0]=="Fr\t?")
		traductions=lfrancais.down(vracine[0].lower(),"+Fr");
	if (traductions[0]!="Fr\t?") {
		//on fait attention a ne garder que les mots qui ont la meme categorie
		string cat=pos("xx"+vracine[1]);				
		vector resultat=traductions[0].split("\t");
		if (cat==pos(resultat[1])) {
			if (resultat[1] not in tous)
				tous.push(resultat[1]);
		}
		bool premier=true;
		for (m in traductions) {			
			souschaine=m.split("\t");
			if (cat=="pron" && souschaine[1][:2]=="ko") {
				souschaine[1]="ka"+souschaine[1][2:];
				return(souschaine);
			}
			if (cat==pos(souschaine[1])) {
				if (souschaine[1] not in tous)					
					tous.push(souschaine[1]);
				if (premier) {
					resultat=souschaine;
					premier=false;
				}
			}
		}
		//si la categorie n'est pas reconnue, on renvoie le premier, dont on extraiera la racine.
		map end=ending(resultat[1]);
		
		if (choix.test(end["root"]))
			resultat[1]=choix[end["root"]]+ending(resultat[1])["suffix"];
		return(resultat);
	}
	return(souschaine);
}


function versfrancais(string s) {
	//Il faut d'abord extraire les surfaces et les lemmas...
	//la surface nous offrira la traduction que nous placerons dans les lemmas
	vector v=s.extract(0,'[',']');
	string surf,lem,res;
	string x,e;
	int i,pos;
	vector trad;
	map mx,end;	
	string addfeat;
	vector results;
	for (x in v) {
		addfeat="";
		mx="{"+x+"}";
		surf=mx["surface"];
		end=ending(surf);
		if (end["basis"][-2] in ["f","m"]) {
			trad=lfrancais.up(surf);
			if ("+?" in trad[0]) {
				surf=end["root"]+"t"+end["basis"][1:];
				trad=lfrancais.up(surf);
			}
		}
		else {
			surf=end["root"]+end["basis"];
			trad=lfrancais.up(surf);
		}
		if ("+?" in trad[0]) {
			for (e in ["i","e","u"]) {
				surf=surf[:-1]+e;
				trad=lfrancais.up(surf);
				if (e=='i')
					addfeat="time:+,";
				if ("+?" not in trad[0])
					break;
			}
		}		
		trad=trad[0].split("\t");	
		if (addfeat!="")
			s=s.replace('"lemma":"'+mx["lemma"],addfeat+'"lemme":"'+trad[0]);
		else
			s=s.replace('lemma":"'+mx["lemma"],'lemme":"'+trad[0]);
	}	
	//On vire aussi les masc inutiles...
	s=s.replace('"gender":"NEUTRAL",',"");
	s=s.replace('"gender":"MASC",',"");
	s=s.replace('"gender":"FEM",',"");
	return(s);
}

function toenglish(string s,self particle) {
	//Il faut d'abord extraire les surfaces et les lemmas...
	//la surface nous offrira la traduction que nous placerons dans les lemmas
	vector v=s.extract(0,'[',']');
	particle="";
	//vector vcat=s.extract(0,
	string surf,lem,res;
	string x,e;
	int pos,i=0;
	vector trad;
	map mx,end;	
	string addfeat;
	vector results;
	for (x in v) {
		addfeat="";
		mx="{"+x+"}";
		surf=mx["surface"];		
		end=ending(surf);		
		if (end["basis"][-2] in ["f","m"]) {
			trad=lenglish.up(surf);
			if ("+?" in trad[0]) {
				surf=end["root"]+"t"+end["basis"][1:];
				trad=lenglish.up(surf);
			}
		}
		else {
			surf=end["root"]+end["basis"];
			trad=lenglish.up(surf);
		}
		if ("+?" in trad[0]) {
			for (e in ["i","e","u"]) {
				surf=surf[:-1]+e;
				trad=lenglish.up(surf);
				if (e=='i')
					addfeat="time:+,";
				if ("+?" not in trad[0])
					break;
			}
		}	
		trad=trad[0].split("\t");
		if (addfeat!="")
			s=s.replace('"lemma":"'+mx["lemma"],addfeat+'"lemme":"'+trad[0]);
		else
			s=s.replace('lemma":"'+mx["lemma"],'lemme":"'+trad[0]);

		if ("#" in trad[0]) {
			vector vcat=s.extract(0,',','"]');
			int idx=vcat[0]['#':'['][1:];
			//On va avoir d'autres elements
			vector motcomplexe=trad[0].split("#");
			vector cats=trad[1].split('+')[:-1];
			particle="PARTICLE(0,"+vcat[0]+'"],'+cats[1]+"#"+(idx*10)+'[lemme="'+motcomplexe[1]+'"]).';
			s=s.replace(trad[0],motcomplexe[0]);
			particle=particle.replace(trad[0],motcomplexe[0]);
		}
		i++;
	}
	//On vire aussi les masc inutiles...
	s=s.replace('"gender":"MASC",',"");
	s=s.replace('"gender":"FEM",',"");
	s=s.replace('"gender":"NEUTRAL",',"");
	return(s);
}


frame block {
	window w;
	editor lingvte;
	string initiallingveta;
	string final;
	editor wlingveta;
	woutput wfrancais;
	woutput wenglish;
	map styles;
	editor wcomp;
	window comp;

	function gettext(button b,block bb) {
		final=lingvte.value();		
		comp.close();
		w.close();
	}

	function sauve(button b,block bb) {
		string s=wlingveta.value();
		println(s);
		file f('generationfrancais/input.txt',"w");
		f.write(s);
		f.close();
		f.openwrite('englishgenerate/input.txt');
		f.write(s);
		f.close();
	}

	function parse(string ph) {
		//On cherche les iar
		vector xv=ph.split(" ");
		string s,p;
		vector mots;
		vector cats;
		for (s in xv) {
			p=ending(s)["pos"];
			if (p=="Pron" || p=="Prep") {
				mots.push(s);
				cats.push(p);
			}
		}
		int j=0;
		for (s in mots) {
			lingvte.annotate(s,cats[j]);
			j++;
		}
	}

	function parsecat(editor e,string ph) {				
		e.annotateregexip("%C+#%d+","Cat");
		e.annotateregexip("%C+%(","dependance");
		e.annotateregexip('surface":"?+"',"surface");
		e.annotateregexip('lemme":"?+"',"lemme");
	}

	function reanalyse(button b,block bb) {
		string s,ligne,leng;
		clear();
		fullfeat=true;
		string ph=lingvte.value();
		parse(ph);
		analyselingveta.parse(ph);
		
		vector vph=ph.split(" "); 
		vector vinit=initiallingveta.split(" ");
		if (vph.size()==vinit.size()) {
			int i;
			for (i=0;i<vph.size();i++) {
				if (vph[i]!=vinit[i])
					choix[ending(vinit[i])["root"]]=ending(vph[i])["root"];
			}
		}
		string particle;
		vector particles;
		for (s in lingvetares) {
			leng+=toenglish(s,particle)+endl;
			if (particle!="" && particle not in particles) {
				leng+=particle+endl;
				particles.push(particle);
			}
			ligne+=versfrancais(s)+endl;
		}
		generefrancais.generatefromstring(ligne);
		wlingveta.value(ligne);
		parsecat(wlingveta,ligne);
		resanalyse[0]=resanalyse[0].replace(" ,",",");
		wfrancais.value(resanalyse[0]);
		clear();
		genereenglish.generatefromstring(leng);
		resanalyse[0]=resanalyse[0].replace(" ,",",");
		wenglish.value(resanalyse[0]);
	}


	function lance(string analysefrancais,string fr,string ph,string lph,string tr,string eng,string tous) {
		styles={'#':[FL_BLACK,FL_HELVETICA,FL_NORMAL_SIZE ],'Prep':[ FL_DARK_RED,FL_HELVETICA_ITALIC,FL_NORMAL_SIZE ],
			"Pron":[ FL_DARK_GREEN,FL_HELVETICA_ITALIC,FL_NORMAL_SIZE ],"Cat":[ FL_RED,FL_HELVETICA_BOLD,FL_NORMAL_SIZE ],
		"surface":[FL_DARK_GREEN,FL_HELVETICA_ITALIC,FL_NORMAL_SIZE ],
		"lemme":[FL_DARK_YELLOW,FL_HELVETICA_ITALIC,FL_NORMAL_SIZE ],"dependance":[ FL_BLUE,FL_HELVETICA_BOLD,FL_NORMAL_SIZE ]};
  
		final=ph;

		comp.begin(300,200,1500,300,"Compilation");		
		wcomp.create(50,20,1400,200,"");
		wcomp.addstyle(styles);
		wcomp.value(analysefrancais);
		parsecat(wcomp,analysefrancais);
		comp.end();

		w.begin(300,200,1300,700,"Modification");
		w.sizerange(10,20,0,0);		
		woutput wout;
		wout.create(200,20,1000,50,true,"Français");
		wout.value(fr+endl+tous);

		lingvte.create(200,120,1000,50,"Traduction");
		lingvte.addstyle(styles);
		lingvte.value(ph);
		parse(ph);
		initiallingveta=ph;


		wlingveta.create(200,220,1000,200,"LINGVETA");
		wlingveta.addstyle(styles);
		wlingveta.value(lph);
		parsecat(wlingveta,lph);

		wfrancais.create(200,430,1000,50,true,"Regénération");
		tr=tr.replace(" ,",",");
		wfrancais.value(tr);

		wenglish.create(200,500,1000,50,true,"English");
		eng=eng.replace(" ,",",");
		wenglish.value(eng);

		button b(this) with gettext;
		b.create(1230,20,60,60,FL_Regular,FL_NORMAL_BUTTON,"Ok");
		
		button bsave(this) with sauve;		
		bsave.create(1230,100,60,60,FL_Regular,FL_NORMAL_BUTTON,"Sauve");

		button banalyse(this) with reanalyse;		
		banalyse.create(1230,180,60,60,FL_Regular,FL_NORMAL_BUTTON,"Analyse");
		
		w.end();		

		w.resizable(lingvte);
		w.run();
	}
}



function reprise(string analysefrancais,string fr,string ph,string lph,string trad,string english,string tous) {
	block b;
	b.lance(analysefrancais,fr,ph,lph,trad,english,tous);
	return(b.final);
}

function sauveanglais(string compilation) {
	file fic('englishgenerate/input.txt',"w");
	fic.write(compilation);
	fic.close();
}

function traduit(string ph,self comp) {
	vector resultat;
	clear();
	string phfr=ph;
	francais.parse(ph);
	comp=lingvetares.join(endl);
	string s=comp;
	fullfeat=false;
	genere.generatefromstring(s);
	string mot,phrase,racine;
	int index;
	vector vmots,vracine,traductions,souschaine,equivalence;
	vector tous;
	for (ph in resanalyse) {
		vmots=ph.split(" ");
		phrase="";
		for (mot in vmots) {
			mot=mot.latin();
			vracine=mot.split("#");
			souschaine=[];
			if (0!=equivalence)
				phrase+=equivalence[0]["phrase"]+" ";
			else {
				switch(vracine[0]) {
					"que": souschaine=["","ka"+vracine[1]];
					"qui": souschaine=["","ka"+vracine[1]];
					default: souschaine=francaisverslingvata(vracine,tous);
				}
				if (0!=souschaine) {
					racine=ending(souschaine[1])["root"];
					if (racine!="")
						phrase+=racine+vracine[1]+" ";
				}
			}
		}
		resultat.push(phrase);
	}
	clear();
	fullfeat=true;
	analyselingveta.parse(resultat[0]);
	string ligne,leng,particle;
	vector particles;
	for (s in lingvetares) {
		leng+=toenglish(s,particle)+endl;
		if (particle!="" && particle not in particles) {
			leng+=particle+endl;
			particles.push(particle);
		}

		ligne+=versfrancais(s)+endl;
	}
	generefrancais.generatefromstring(ligne);
	vector resfrench=resanalyse;
	clear();
	genereenglish.generatefromstring(leng);
	string resph=reprise(comp,phfr,resultat[0],leng,resfrench[0],resanalyse[0],tous);
	return(resultat);
}



function sauvecompilation(string compilation) {
	file fic('generationlingveta/input.txt',"w");
	fic.write(compilation);
	fic.close();
}

//concepts.ajouteequivalencefrancais("pour","","udiar");

string compilation;
vector traduc;
string resph;

traduc=traduit("La dame parle.",compilation);
traduc=traduit("La dame n'aime pas la maison.",compilation);
traduc=traduit("le chien aboie dans la nuit.",compilation);
traduc=traduit("La jeune fille conduit une voiture neuve.",compilation);
traduc=traduit("La jeune fille conduit une voiture neuve avec un chien.",compilation);
traduc=traduit("La jeune fille conduit une voiture neuve pour le neveu du voisin.",compilation);
traduc=traduit("La jeune fille qui conduit une voiture neuve parle avec le neveu du voisin.",compilation);
traduc=traduit("La jeune fille que regarde le chien avec un os parle avec le neveu du voisin.",compilation);
