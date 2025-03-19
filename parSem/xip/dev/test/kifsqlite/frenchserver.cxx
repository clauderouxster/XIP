//We load the socket library
use('/opt/ParSem/users/Roux/xip/parSem/xip/dev/xipFst/lib/linux/sharedLib/kifsocket');
parser french;
//we load a grammar
french.load('/opt/ParSem/users/Roux/xip/parSem/xip/dev/GRAMMARS/FRENCH/BASIC/french.grm');

function displaygc() {
	printlnerr("Garbaging:",gcsize());
}

garbagefunction(displaygc);

//This function will be used to launch the parse of a sentence.
function appel(string sx) {
	string res=french.parse(sx);
	return(res);
}

int nbconnected=0;
map sockets;
map actives;



//We read on a socket the sentence from the client...
thread parse(int num,socket s) {
	string sx;
	string res;
	string mes;
	int example=0;
	int nx=sockets[num];
	println("Parse:",num);
	bool connect=true;
	while (connect==true) {
		try {
			//we read our sentence
			actives[num]=true;
			sx=s.read(num);
			actives[num]=false;
			if (sx!="") {
				//the we parse it
				res=appel(sx);					
				//println("Nb:",nbconnected,"Example:",example,"Sending: ",sockets[num],"-->",res.size());
				//println("Garbage stats:",gcsize());
				example+=1;
				//we return the result to the client
				s.write(num,res);
			}
		}
		catch(mes) {	
			connect=false;
			sockets.pop(num);
			actives.pop(num);
			println("Deconnexion:",nx,"remains:",sockets.size(),"nb examples:",example);			
			//println("Garbage stats:",gcsize());
			nbconnected-=1;
			s.close(num);
			return;
		}	
			
	}
}

socket s;
//We create our server on the port 2021, for 20 potential clients
s.createserver(2021,20);
s.settimeout(10);
println("Port: 2021 "+s.gethostname());
int n;
int id=0;

while (true) {
	//we wait for a client to connect
	n=s.wait();
	//we use this socket id to parse the sentence...
	sockets[n]=id;
	actives[n]=true;
	println("Nouveau:","Id:",id,"Socket:",n,"Nb connecte:",nbconnected);
	println("Garbage stats:",gcsize());
	try {
		s.write(n,id);
		id+=1;
		nbconnected+=1;
		parse(n,s);
	}
	catch();
}

