kif:

use('kifsqlite');

sqlite mydb;
mydb.open('test.db');

try {
 mydb.create("table1","nom TEXT","age INTEGER");
}
catch();

mydb.insert("table1","nom","Ltoto","age",20);
mydb.insert("table1","nom",'"atoto"',"age",220);
mydb.insert("table1","nom","atoto","age",320);

iterator it;

//vector res;
//res=mydb.execute("select * from table1 where nom='atoto';");
//
//
//it=res;
//for (it.begin();it.nend();it.next())
//	println("Values: ",it.key()," ",it.value()["nom"]);

it=mydb.execute("select * from table1 where nom='atoto';");
self mp;
for (it.begin();it.nend();it.next()) {
	mp=it.value();
    println("Value: ",it.key()," ",mp["age"]);
}



mydb.close();

