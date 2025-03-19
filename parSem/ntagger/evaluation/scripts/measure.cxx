
use('kifsqlite');

string wsjfolderName='/home/fnoorala/Desktop/hmmComparison/wsj';
string mainfilepath=wsjfolderName+"/adjusted-main.txt";
//we declare a new sqlite variable
sqlite mydb;

//we open a database. If it does not exist, it creates it...
mydb.open('corpura.db');

mydb.run("DROP TABLE measures");
mydb.run("DROP TABLE tags");
try {
     //we insert a new table in the current database
     mydb.create("tags","word TEXT","wsj TEXT","xip TEXT");
     println("Table:tags is created");
}
catch() {
     //This database already exists
     println("Table:tags exist!");
}

try {
     //we insert a new table in the current database
     mydb.create("measures","tag TEXT","freq FLOAT","TP FLOAT","FP FLOAT","FN FLOAT","TN FLOAT","Recall FLOAT","Precision FLOAT","Accuracy FLOAT","Fmeasure FLOAT","TT FLOAT");
     println("Table:measures is created");
}
catch() {
     //This database already exists
     println("Table: measures exist!");
}

//We insert values in the database, using a commit mode (which is much faster)
mydb.begin();
mydb.run("DELETE from tags");
// read lines from lists and insert it in table
file f(mainfilepath,'r');
svector svect=f.read();
f.close();
string ln;
svector member;
for (ln in svect){
     if (~ ("//" in ln) && ln!='') 
    
     // we split the everyline based on "/"
     member=ln.trim().split("/");
	
     try {
          //we insert in tags 3 values,word/wsjTag/xipTag
	if (member.size()!=0)	
          mydb.insert("tags","word",member[0],"wsj",member[1],"xip",member[2]);
         //println(member[0]+" "+member[1]+" "+member[2]);
     }
     catch() {
          println("Error");
     }
}

//we then commit our commands.
mydb.commit();
mydb.run("DELETE from measures");
string script="SELECT wsj as tag,freq,TP,FP,FN,(TT-(TP+FP+FN))as TN,(TP/(TP+FN))as Recall,(TP/(TP+FP))as Precision,((TT-(FP+FN))/TT) as Accuracy ,(2*(TP/(TP+FN))*(TP/(TP+FP)))/((TP/(TP+FN))+(TP/(TP+FP)))as Fmeasure,TT FROM(SELECT zz.wsj,(SELECT cast(count(*)as float) from tags) as TT,cast(SUM(zz.Total) as float) as freq,cast(SUM(zz.TP) as float) as TP,cast(SUM(zz.FP) as float) as FP,cast(SUM(zz.FN) as float) as FN FROM(SELECT wsj,cast(count(*) as float) AS Total,0 AS TP,0 AS FN,0 AS FP FROM tags group by wsj UNION ALL SELECT a.wsj, 0 AS Total, cast(count(*) as float) as TP,0 AS FN,0 AS FP FROM (SELECT wsj FROM tags where wsj=xip)AS a group by a.wsj UNION ALL SELECT b.wsj,0 AS Total,0 as TP,count(*) AS FN,0 AS FP FROM (SELECT wsj FROM tags where wsj!=xip)AS b group by b.wsj UNION ALL SELECT z.xip as wsj,0 AS Total,0 as TP,0 AS FN,z.count AS FP FROM(SELECT b.xip,cast(count(*) as float) as count FROM (select xip FROM tags where wsj!=xip)b group by b.xip)z where z.xip in (SELECT wsj FROM  tags))zz Group by zz.wsj);";
mydb.run("INSERT INTO measures(tag,freq,TP,FP,FN,TN,Recall,Precision,Accuracy,Fmeasure,TT)"+" "+script);
//string script="SELECT count(*) from tags;";
string scr="SELECT * from measures;";
//we print the result of evaluation
file r(wsjfolderName+_sep+"measure.csv",'w');
r.writeln('TAG'+'-'+'FRE'+'-'+'TP'+'-'+'FP'+'-'+'FN'+'-'+'TN'+'-'+'RE'+'-'+'PR'+'-'+'FM'+'-'+'AC');
iterator it=mydb.run(scr);
svector sv;
for(it.begin();it.nend();it.next()){
string w=it.value();
string AC=w.split("','")[0].split(',')[0].split(':')[1];
string FN=w.split("','")[0].split(',')[1].split(':')[1];
string FP=w.split("','")[0].split(',')[2].split(':')[1];
string FM=w.split("','")[0].split(',')[3].split(':')[1];
string PR=w.split("','")[0].split(',')[4].split(':')[1];
string RE=w.split("','")[0].split(',')[5].split(':')[1];
string TN=w.split("','")[0].split(',')[6].split(':')[1];
string TP=w.split("','")[0].split(',')[7].split(':')[1];
string TT=w.split("','")[0].split(',')[8].split(':')[1];
string FRE=w.split("','")[0].split(',')[9].split(':')[1];
string TAG=w['tag':][:'}'][:-1][':':][2:][:-1];
string str="'"+TAG+"'"+'-'+FRE+'-'+TP+'-'+FP+'-'+FN+'-'+TN+'-'+RE+'-'+PR+'-'+FM+'-'+AC;  
r.writeln(str);   
}



//other way
//file r(wsjfolderName+_sep+"measure.txt",'w');
//vector v=mydb.run(script);
//string row;
//for(row in v)
//	r.write(row,_endl);

r.close();
mydb.close();
