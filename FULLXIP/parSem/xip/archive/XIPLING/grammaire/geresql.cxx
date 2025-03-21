
use(_args[0],"",'kifsqlite');
use(_args[0],"debug",'C:\XIP\Debug\kifsqlite');

//use(_args[0]);

sqlite lingvetabd;

function ouvrirsql(string nom) {

	//we open a database. If it does not exist, it creates it...
	lingvetabd.open(nom);
	int s;

	try {
		//we insert a new table in the current database
		lingvetabd.create("concepts","nom TEXT PRIMARY KEY","clef INTEGER","UNIQUE(nom)");		
	}
	catch(s) {
		if (s!=805)
			println("Error:",s);
	}
	try {
		lingvetabd.create("graphesfrancais","clef INTEGER PRIMARY KEY","phrase TEXT","test TEXT");
	}
	catch(s) {
		if (s!=805)
			println("Error:",s);
	}
	try {
		lingvetabd.create("grapheslingveta","clef INTEGER PRIMARY KEY","phrase TEXT","test TEXT");
	}
	catch(s) {
		if (s!=805)
			println("Error:",s);
	}
}

function compteclef() {
	string action="COUNT(clef)";
	string clef='select '+action+' from concepts;';
	vector sz=lingvetabd.execute(clef);
	return(sz[0][action]);
}

function recupereindex(string nom) {
	string action="clef";
	string clef='select '+action+' from concepts where nom="'+nom+'";';
	vector sz=lingvetabd.execute(clef);
	if (0==sz.size())
		return(-1);
	return(sz[0][action]);
}

function Begin() {
	lingvetabd.begin();
}

function End() {
	lingvetabd.commit();
}

function ajouteconcept(string nom,self i) {
	int nb=recupereindex(nom);	
	if (nb!=-1)
		return(nb);

	if (i==-1)
		i=compteclef();

	try {
		lingvetabd.insert("concepts","nom",nom,"clef",i);
		i++;
	}
	catch;
	return(i);
}

function ajouteequivalencefrancais(string clef,string test,string equivalence) {
	int nb=ajouteconcept(clef,-1);	
	//"graphesfrancais","clef INTEGER PRIMARY KEY","phrase TEXT","test TEXT"
	try {
		if (nb!=-1)
			lingvetabd.insert("graphesfrancais","clef",nb,"phrase",equivalence,"test",test);
	}
	catch;
}


function ajouteequivalencelingveta(string clef,string test,string equivalence) {
	//"grapheslingveta","clef INTEGER PRIMARY KEY","phrase TEXT","test TEXT"
	int nb=ajouteconcept(clef,-1);	
	try {
		if (nb!=-1)
			lingvetabd.insert("grapheslingveta","clef",nb,"phrase",equivalence,"test",test);
	}
	catch;
}


function recupereequivalencefrancais(string clef) {
	int i=recupereindex(clef);	
	if (i==-1)
		return([]);
	string action="phrase,test";
	string commande='select '+action+' from graphesfrancais where clef='+i+';';
	vector sz=lingvetabd.execute(commande);
	if (0==sz.size())
		return([]);
	return(sz);
}


//ouvrirsql('C:\eclipse\XIPLING\grammaire\lingveta.db');
//ajouteequivalencefrancais("pour","","udar");
//self vr=recupereequivalencefrancais("pour",0);
//println(vr);

