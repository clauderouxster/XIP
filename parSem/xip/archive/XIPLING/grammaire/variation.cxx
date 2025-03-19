
bool trouve=false;


function arbreloc(node top) {
	node n=top.child();
	if (n!=null) {
		print(top.pos()+"[");
		while (n!=null) {		
			arbreloc(n);
			n=n.next();
		}	
		print("]");
	}
}

function arbre(node top) {
	arbreloc(top);
	println();
}

parser parse('C:\eclipse\XIPLING\grammaire\lingveta.grm');

fst analyse('C:\eclipse\XIPLING\grammaire\ending.fst',"",0,false);
vector prep=["iar","iars","isars","idars","inars"];
vector pron=["ier","iser","ider","iner","iers","isers","iders","inerd","ierd","iserd","iderd","inern","iern","isern","idern","inern"];

map m={"ier":pron,"iar":prep};

//frame analyse {
//	int compte;
//	bool cont with synchronous=true;
//	string laphrase;
//	int stop=0;
//
//	function variation(vector vphrase,vector pos,int choix) {
//	}
//}

int compte=0;
function couple(vector phrase,vector pos,vector clef,vector k,int i) {	
	if (i>=pos) {
		int j;
		for (j=0;j<clef;j++)
			phrase[clef[j]]=k[j];		
		parse.parse(phrase.join(" "));
		compte++;
		return;
	}
		
	int j;

	for (j=0;j<pos[i];j++) {
		vector v=k;
		v.push(pos[i][j]);
		couple(phrase,pos,clef,v,i+1);
	}
}



function appel(string phrase) {
	trouve=false;
	vector vphrase=phrase.split(" ");
	vector pos,cases;	
	map clef;
	string data,c;
	int i=0,j,p,compte=0;
	string s,an;
	bit test(4);
	while (s in vphrase) {
		println(cases);
		if (s[-3:]=="iar") {
			pos.push([]);
			clef[i]=true;
			for (p=0;p<prep.size();p++) {
				data=s[:-3]+prep[p];
				an=analyse.up(data);
				while (c in cases)
					if ("+a"+c in an)
						pos[-1].push(data);
			}
		}
		else
			if (s[-3:]=="ier") {
				pos.push([]);
				clef[i]=true;
				for (p=0;p<pron.size();p++) {
					data=s[:-3]+pron[p];
					an=analyse.up(data);
					while (c in cases)
						if ("+a"+c in an)
							pos[-1].push(data);
				}
			}
			else {
				data=analyse.up(s);
				if ("+acc" in data)
					if ("acc" not in cases) {
						cases.push("acc");
						test[1]=true;
					}
				if ("+dat" in data)
					if ("dat" not in cases) {
						cases.push("dat");
						test[2]=true;
					}
				if ("+nom" in data)
					if ("nom" not in cases) {
						cases.push("nom");
						test[0]=true;
					}
				if ("+gen" in data)
					if ("gen" not in cases) {
						cases.push("gen");
						test[3]=true;
					}
			}

		i++;
	}	
	couple(vphrase,pos,clef,[],0);
}


float f=random();
//analyse app;
appel("yuneca filinefa nacan vetureletan udiar nevefad apudefas kondukeiag");
//println("On attend");
//waitonfalse(app.cont);
//println("Resultat:",app.laphrase,app.compte);
