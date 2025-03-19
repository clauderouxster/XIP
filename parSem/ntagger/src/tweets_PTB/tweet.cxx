
file f("train","r");
svector svect=f.read().trim();
f.close();

string ln;
svector member;
string tweet='';
svector tweets;
int i=1;

for (ln in svect){
     i+=1;
     println(i);
     // println(ln.trim());
     if(ln.trim()=="") {
          tweets.push(tweet.trim());
          tweet='';
          continue;
     }
     //remove line spaces
     // expression for discared lines with ==>

     // we split the everyline based on ""
     member=ln.trim().split(" ");
     // if (~ member[0].regexip("'\s*")){
     // if words start with " ' " or is " ." we contact to previous word
     if(member[0].regexip("'?+") && member[0].regexip("%p+"))
          {
               tweet=tweet+member[0] ;
               continue;
          }
     //member[0].regexip("[%c,%C,%d]'?+") ||
     tweet=tweet+" "+member[0];
}
//}

file r("./tweetText/train",'w');
string line;
for(line in tweets)
     r.write(line,_endl);
r.close();
