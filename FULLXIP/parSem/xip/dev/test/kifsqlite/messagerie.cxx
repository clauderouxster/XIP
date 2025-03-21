#!/usr/bin/KiF
socket s;

//server side
string name=s.gethostname();
println("Serveur local:",name);
s.createserver(2020,10);
boolean fin=false;
boolean reaffiche=false;

thread serveur(socket s) {	
	string mes="";
	while (mes!='.') {
		try {
			mes=s.read();
		}
		catch() {
			return;
		}
		if (mes!="") {
			println("\n="+mes);			
			reaffiche=true;
			print(">");
			pflush();
		}
	}
	fin=true;	
}

s.wait();
println("Connexion");
serveur(s);

string message;
int c;
while (fin==false) {
	print(">");
	c=0;
	message="";
	while (c!=13 && c!=10) {
		c=getc();
		if (c>=32) {
			message+=c.chr();
			print(">"+message);
			pflush();
		}
	}
	println();
	s.write(message);
}
s.close();
