frame test {
	string u="Dans test";
	join thread jdisplay(int s) {
		  print("Ici:"+u.size()+","+s+"\r");
	}
}




join thread launch() {
    int i;
	test t;
    for (i=0;i<5000;i++) {
		t.u+="i"+i;
        t.jdisplay(i);
		t.u="";
	}
	waitonjoin();
    println("Fin");
}

launch();
launch();
waitonjoin();
println("Termination");

