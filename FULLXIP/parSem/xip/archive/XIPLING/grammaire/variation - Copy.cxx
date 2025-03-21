
bool trouve=false;
function appellingveta(string name,map d,map f1,map f2,int idn1,int idn2,string pos1,string pos2) {
}

parser parse('C:\eclipse\XIPLING\grammaire\lingveta.grm');

vector prep=["iar","iars","isars","idars","inars"];
vector pron=["ier","iser","ider","iner","iers","isers","iders","inerd","ierd","iserd","iderd","inern","iern","isern","idern","inern"];

map m={"ier":pron,"iar":prep};

frame analyse {
	int compte;
	bool cont with synchronous=true;
	string laphrase;
	int stop=0;

	function variation(vector vphrase,vector pos,int choix) {
		if (compte<0 || laphrase!="") {
			cont=false;
			return;
		}
		int nb,v,p;
		string sub;
		v=compte;
		vector vp=vphrase;
		for (nb=choix;nb>=0;nb--) {
			p=pos[nb];
			sub=vphrase[p];
			vp[p]=sub[:-3]+m[sub[-3:]][v%5];
			v=v/5;		
		}

		sub=vp.join(" ");
		parse.parse(sub);
		if (trouve==true) {
			laphrase=sub;
			vp.insert(0,stop);
			cont=false;		
		}
		elif (laphrase=="") {
			compte--;
			variation(vphrase,pos,choix);
		}
	}
}

function appel(analyse app,string phrase) {
	trouve=false;
	vector vphrase=phrase.split(" ");
	vector pos;
	int i=0;
	string s;
	int compte=0;
	while (s in vphrase) {
		if (s[-3:]=="iar") {
			pos.push(i);
			compte+=prep.size();
		}
		else
			if (s[-3:]=="ier") {
			pos.push(i);
			compte+=pron.size();
		}
		i++;
	}
	app.compte=compte-1;
	app.variation(vphrase,pos,pos.size()-1);	
}

float f=random();
analyse app;
appel(app,"yuneca filinefa nacan vetureletan udier nevefad apudefas kondukeiag");
println("On attend");
//waitonfalse(app.cont);
println("Resultat:",app.laphrase,app.compte);
