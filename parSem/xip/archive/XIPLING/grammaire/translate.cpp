Variables:

string uss;

kif:

function appellingveta(string name,map d,map f1,map f2,int idn1,int idn2,string pos1,string pos2);
function appelunique(string name,map d,map f,int idn,string pos);
function arbre(node n);

function construitdependence(dependency d) {
	vector nodes=d.parameters();
	string pos1,pos2;
	node n1=nodes[0];	
	node n2=nodes[1];	
	map f1=n1.data();
	while (f1['PHRASAL']=="+") {
		n1=n1.last();		
		f1=n1.data();
	}
	map f2=n2.data();
	while (f2['PHRASAL']=="+") {
		n2=n2.last();		
		f2=n2.data();
	}
	//println(d,n1,n2,n1.int(),n1.parent().int(),nodes[0].int(),n2.int(),n2.parent().int(),nodes[1].int());
	appellingveta(d,d.data(),f1,f2,n1,n2,n1,n2);
}

function construitunaire(dependency d) {
	vector nodes=d.parameters();
	string pos1,pos2;
	node n1=nodes[0];	
	map f1=n1.data();
	while (f1['PHRASAL']=="+") {
		n1=n1.last();		
		f1=n1.data();
	}
	appelunique(d,d.data(),f1,n1,n1);
}


function comparecas(string c1,string c2) {
   println(c1,c2);
   return(true);
}













