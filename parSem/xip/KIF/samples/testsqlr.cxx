kif:


println("Version:",version(),_paths);
//Loading the kif grammar
//Use use if launching from XIP
use('kifsqlite');

//The following lines are only used to find a path where to store the database
//_paths[0] is the current path of the calling program
//_paths[1] is the path of the current KiF program being run, which is this one of course...
vector vchemin=_paths[1].split(_sep)[:-1];
//vector.join(character), it concatenates every single string in a vector into one single string...
string bdpath=vchemin.join(_sep)+'\test.db';
//If we are on a windows network path, we need to add
if (_paths[1][0:2]=='\\')
	bdpath='\\'+bdpath;

sqlite mydb;
mydb.open(bdpath);

try {
 mydb.create("table1","nom TEXT PRIMARY KEY","age INTEGER");
 println("table1 est cree");
}
catch() {
	println("Deja cree");
}

int i;
string nm;
mydb.begin();
for(i=0;i<5000;i+=1) {
	
	nm="tiia_"+i;
	try {
		mydb.insert("table1","nom",nm,"age",i);
		println(i);
	}
	catch() {
		println("Deja inseree");
	}
}
mydb.commit();

iterator it=mydb.execute("select * from table1 where age>10;");
for (it.begin();it.nend();it.next())
    println("Value: ",it.value());

mydb.close();

