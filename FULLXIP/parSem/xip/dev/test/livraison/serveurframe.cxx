//A simple server of objects...

//First we declare a simple frame
frame test {
	int i=10;

	function Value() {
		return(i);
	}
	
	function Compute(int j) {
		return(i*j);
	}
}

//The method creates a new object of frame t, with the name: n
function Create(string n,int i) {
	test t;
	t.i=i;
	_KIFMAIN[n]=t;
}


thread xrun(socket s,int c,string st) {
	string err;
	try {
		s.run(c,st);
	}
	catch(err) {
		println("Error in xrun:",err);
	}
	s.close(c);
	println("Stopped");
}

//We have our specific frame object, which any client can ask for
test toto;

//We create our server
socket s;

println(s.gethostname());
//On port: 2012
s.createserver(2012,10);


//we wait for a client to connect
int client;
while (true) {
	client=s.wait();
	//The stop string is here, well: "stop"
	//We wait for the client to execute functions on the server side...
	//If our server receives the "stop" string, it stops.
	xrun(s,client,"stop");
}


