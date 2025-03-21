//A first thread with a join
join thread jdisplay(string s) {
	print(s+"\r");
}

//which is launched from this thread also "join"
join thread launch(int x) {
    int i;	
    for (i=0;i<5000;i++) {
		string s="Thread:"+x+"="+i;
        jdisplay(s);
	}
	//we wait our local threads to finish
	waitonjoin();
    println("End:"+x);
}

//we launch two of them
launch(0);
launch(1);
//and we wait for them to finish...
waitonjoin();
println("Termination");


