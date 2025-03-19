//Test sur les self dans les frames pour creer un arbre

frame noeud {
	self fils;
	self pere;
	self frere;
	self precedent;
	string label;

	function _initial(string s) {
		label=s;
	}


	function string() {
		return(label);
	}

	function Lieprecedent(noeud n) {
		precedent=n;
	}
	
	function Ajoutefrere(noeud n) {
		self x=frere;
		frere=n;
		n.frere=x;
		n.pere=pere;
		n.Lieprecedent(this);		

		if (x!=null)
			x.Lieprecedent(n);
	}

	function Ajoutefils(noeud n) {
		self x=fils;
		fils=n;
		n.fils=x;
		n.pere=pere;
	}
	
	function Label() {
		return(label);
	}

	function affiche() {
		print(label," ");
		if (precedent!=null)
			print("+",precedent.Label()," ");
		if (fils!=null) {
			print("(");
			fils.affiche();
			print(")");
		}
		if (frere!=null)
			frere.affiche();
	}
}


noeud n1("f1");
noeud n2("f2");
noeud n3("f3");
noeud n4("f4");
noeud n5("f5");
noeud n6("f6");

n1.Ajoutefrere(n2);
n2.Ajoutefrere(n3);
n3.Ajoutefrere(n4);
n2.Ajoutefils(n5);
n2.Ajoutefils(n6);

n1.affiche();
println();


vector v;
v.push(n1);
v.push(n2);
v.push(n3);
v.push(n4);
v.push(n5);
v.push(n6);

self u;

while (u in v) 
	print(u," ");
println();
pause(100);


