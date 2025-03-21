//utiliser client.cxx pour communiquer

socket s;

println(s.gethostname());
s.createserver(2012,10);


thread echange(int client) {
	map m={'a':1,'b':2,'c':3};
	vector v;
	v=s.read(client);
	println(v);
	s.write(client,m);
}


int client;
while (true) {
	client=s.wait();
	echange(client);
	println("On attend");
}

s.close();
