file ftrad('/Users/rouxclaude/Développement/kifkids/src/sauve/traductions','r');
string s,l;

vector vl;
vector v;

for (s in ftrad) {
    v=s.trim().split("\t");
    l=v[0].trim();
    vl.push(l);
}

file ffr('/Users/rouxclaude/Développement/kifkids/src/sauve/francais','r');

vector vtr;

for (s in ffr) {
    v=s.trim().split("\t");
    l=v[0].trim();
    vtr.push(l);
}


println(vtr.size(),vl.size());

file fsave('/Users/rouxclaude/Développement/kifkids/src/sauve/accents','w');

int i;
for (i=0;i<vtr.size();i++)
    fsave.write(vl[i].utf8()+"\t"+vtr[i]+"\r");

fsave.close();
