
thread tacheappel(string s) {
	println(s,"Numero:<"+threadid()+","+parentthreadid()+">");
}

function rappel(string s) {
	tacheappel(s);
}

bool toto with synchronous=true;

frame tache {
	int i;
	int j with synchronous;

	function _initial(int v) {
		i=v;
		j=10;
	}

	thread affiche(string s) {
		rappel("Rappel:"+s+" avec "+i);
	}

	thread lance(int x,bool cst) {
		int k=i;
		println(x+" "+cst+" "+j);				
		if (x==100)
			toto=false;
		else
			j-=1;
		while (k==i) {}
		println("Thread:"+k+" "+i+" "+x);
	}
}




thread launch(tache t1) {
	int i;

	for (i=0;i<10;i++) 
		t1.lance(i,false);
	waitonfalse(t1.j);
	println("Tous lances");
	t1.lance(100,true);
}

function essai() {
	tache t1(10);
	launch(t1);
	waitonfalse(toto);
	t1.i=20;
}

essai();


