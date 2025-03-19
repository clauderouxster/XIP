use('kifsqlite');


//***************************************************************
string wsjfolderName='/home/fnoorala/parSem/twitter';
string mainfilepath=wsjfolderName+_sep+"pos.txt";

file f(mainfilepath,'r');
svector svect=f.read();
f.close();
string ln;
svector member;
string tweet='';
svector tweets;
for (ln in svect){
     if(ln=='') {
          tweets.push(tweet.trim());
          tweet='';
     continue;}
     //remove line spaces
     // expression for discared lines with ==>

     // we split the everyline based on ""
     member=ln.split(" ");
     // if words start with " ' " or is " ." we contact to previous word
     if(member[0].regexip("'?+") || member[0].regexip("%p+")) {tweet=tweet+member[0] ; continue;}
     //member[0].regexip("[%c,%C,%d]'?+") ||
     tweet=tweet+" "+member[0];
}
//for(ln in tweets)
//	println(ln);

file r(wsjfolderName+_sep+"tweets.txt",'w');
string line;
for(line in tweets)
     r.write(line,_endl);
r.close();
