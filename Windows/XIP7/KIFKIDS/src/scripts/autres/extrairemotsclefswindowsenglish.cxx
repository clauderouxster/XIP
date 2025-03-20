file ftrad('C:\XIP\XIP7\KIFKIDS\src\englishstrings.cxx','r');
string s,l;

vector tous;
map ml;
int maxi;

function teststring(string l) {
    if (l.isalpha() && l.islower())
        return(true);
    int i='_' in l;
    if (i!=-1) {
        l.pop(i);
        if (l.isalpha() && l.islower())
            return(true);        
    }
    return(false);
}

string sub;
for (s in ftrad) {
    if ('(' in s && ('):' in s || ') :' in s)) {
        sub=s['"':'('][1:];
        if ('=' in sub)
            sub=sub['=':][1:].trim();
        else
            if (' ' in sub && 'Error' not in sub)
                sub=sub[' ':][1:].trim();
        tous.push(sub);
    }
    l=s['"':'"'][1:];
    if (teststring(l)) {
        ml[l]=true;
        if (maxi<l.size())
            maxi=l.size();
    }
}

vector unique;
maxi-=2;
map vl;
int i;
string val;
for (s in tous) {
    if (ml.test(s)) { 
        for (i=1;i<maxi;i++) {
            sub=s[0:i];
            val=s[i:];
            if (sub=="" || val=="")
                break;
            if (vl.test(sub)) {
                if (val not in vl[sub])
                    vl[sub].push(val);
            }
            else
                vl[sub]=[val];
        }
        ml[s]=false;
    }
}

for (s in ml) {
    if (ml[s]==true && "cursor" not in s && "display" not in s && s[0]!='_' && "garbage" not in s && s.size()>1 && "root" not in s && "eucli" not in s)
        unique.push(s);
}

function comp(string s,string ss) {
    if (s.size()>ss.size())
        return(true);
    return(false);
}

unique.sort(comp);

for (s in vl)
    vl[s].sortstring(true);


//NSArray *keys = [NSArray arrayWithObjects:@"key1", @"key2", @"key3", nil];
//NSArray *objs = [NSArray arrayWithObjects:@"obj1", @"obj2", @"obj3", nil];
//NSDictionary *dict = [NSDictionary dictionaryWithObjects:objs forKeys:keys];

string clef="NSArray* lesclefs=[NSArray arrayWithObjects:";
string objs="NSArray* lesvaleurs=[NSArray arrayWithObjects:";
for (s in vl) {
    clef+='@".'+s+'",';
    objs+='[NSArray arrayWithObjects:@"'+vl[s].join('",@"')+'",nil],';
}
 
clef+="nil];";
objs+="nil];";
println("void initialisationdico() {
        if (methodes!=NULL)
        return;
        ");
println(objs);
println(clef);
println("maxiclef="+maxi+";");
println("    methodes = [NSDictionary dictionaryWithObjects:lesvaleurs forKeys:lesclefs];
        [methodes retain];
        }
        ");


clef='mots=[[NSString alloc] initWithString:@"';
for (s in unique) {
   clef+='\\b'+s+'\\b|';
}

clef+='\\bpas\\b|\\bdefault\\b|\\bcurl\\b|\\belse\\b|\\belif\\b|\\bwith\\b|\\.[a-zA-Z\\x80-\\xFF][0-9a-zA-Z\\x80-\\xFF]*\\(|\"|/|\\n|\\r';
clef+="|'\"];";
println(clef);

//file fsave('/Users/rouxclaude/Développement/kifkids/src/sauve/motsclefs','w');

//fsave.close();
