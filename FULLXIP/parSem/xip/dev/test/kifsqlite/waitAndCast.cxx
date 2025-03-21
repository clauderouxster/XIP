#!/bin/bsh

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
		while (true) {			
			wait(s);
			println("Ici dans ce cadre:",i,requete,s);
		}
	}

	function _initial(int j) {
		i=j;
		recadre();
	}
}


autorun red() {
	while (true) {
		wait("rouge");
		println("Quelle couleur violente!!!");
	}
}

autorun blue() {
	while (true) {
		wait("bleu");
		println("La couleur du ciel");
	}
}

autorun azur() {
	while (true) {
		wait("bleu");
		println("Azur: La couleur de Nice");
	}
}


autorun marroun() {
	while (true) {
		wait("marron");
		println("La couleur de l'automne");
	}
}

autorun black() {
	while (true) {
		wait("noir");
		println("La couleur de la nuit");
	}
}

autorun white() {
	while (true) {
		wait("blanc");
		println("Aveuglant");
	}
}

autorun green() {
	while (true) {
		wait("vert");
		println("La couleur de l'ocean");
	}
}

autorun yellow() {
	while (true) {
		wait("jaune");
		println("Le poussin");
	}
}

autorun rose() {
	while (true) {
		wait("rose");
		println("Une belle fleur, vraiment");
	}
}

autorun violet() {
	while (true) {
		wait("violet");
		println("Une couleur sombre et mysterieuse");
	}
}

autorun end() {
	wait("fin");
	exit(-1);
}

autorun killer() {
	wait("tue");
	kill();
}

autorun Wait() {
	while (true) {
		wait("wait");
		println(waiting());
	}
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
