
frame synchro {
	int k;


	function boolean() {
		if (k==0)
			return(false);
		return(true);
	}

	function --() {
		k--;
	}
}

int j=150;
//a synchronized variable
synchro sync with synchronous;

//a simple thread description…
thread mythread(int k) {	
	string s=k+":"+",";
    print(s);
   //we decrement “sync”	
	int jk=10*78*k-17;
	//s=jk+",";
    sync--;	
	//print(s);
}

int kx;
for (kx=0;kx<5000;kx++) {	
	int i;
	sync.k=j;
	println("Avant:"+i+","+kx);
	for (i=0;i<j*10;i+=10)
		mythread(i);//we launch it three times
	println("\nLancement: "+kx);
	waitonfalse(sync); //we wait for each thread to have actually started before continuing…
	println("Fait: "+kx);
	println(gcsize());
}
