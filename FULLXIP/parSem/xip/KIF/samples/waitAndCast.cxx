string requete;
vector v;
string s;

frame test {
	int i;

	function string() {
		string s="cadre"+i;
		return(s);
	}

	function recadre() {
		string s=string();		
		waitall(s);
		println("Ici dans ce cadre:",i,requete,s);		
	}

	function _initial(int j) {
		i=j;
		recadre();
	}
}


autorun red() {
	waitall("rouge");
	println("Quelle couleur violente!!!");
}

autorun blue() {	
	waitall("bleu");
	println("La couleur du ciel");	
}

autorun azur() {
	waitall("bleu");
	println("Azur: La couleur de Nice");
}


autorun marroun() {
	waitall("marron");
	println("La couleur de l'automne");
}

autorun black() {
	waitall("noir");
	println("La couleur de la nuit");
}

autorun white() {
	waitall("blanc");
	println("Aveuglant");
}

autorun green() {
	waitall("vert");
	println("La couleur de l'ocean");
}

autorun yellow() {
	waitall("jaune");
	println("Le poussin");
}

autorun rose() {
	waitall("rose");
	println("Une belle fleur, vraiment");
}

autorun violet() {
	waitall("violet");
	println("Une couleur sombre et mysterieuse");
}

autorun end() {
	waitall("fin");
	exit(-1);
}

autorun killer() {
	waitall("tue");
	kill();
}

autorun Wait() {
	waitall("wait");
	println(waiting());

}



test f(1);
test g(2);
test h(3);

while (true) {
	print("Couleur: ");
	requete.get();
	v=requete.split(" ");
	while (s in v) {
		try {
			cast(s.lower());
		}
		catch() {
			println("Erreur:",s);
		}
	}
}
