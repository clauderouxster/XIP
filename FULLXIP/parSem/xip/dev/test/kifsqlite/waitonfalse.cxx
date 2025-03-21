
//We implement a synchronized thread
protected launch(string n,int m) {
	int i;
	println(n);
	//we display all our values
	for (i=0;i<m;i++)
		print(i," ");	
		//we decrement our stopby variable
	println();
}

function principal() {
	//we launch our thread
	launch("Premier",2);
	launch("Second",4);
}

principal();
