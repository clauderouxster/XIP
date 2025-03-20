string chemin=_args[0];

file f(_args[0],"r");

chemin=chemin.replace(".cxx","r.cxx");
file sauve(chemin,"w");

string s,v;
int i=0;

for (s in f) {
    if ("const char" in s)
        i=0;
    if ("//" in s) {
        v=s.replace(s["//":" "],"//"+i);
        i++;
        sauve.write(v);
    }
    else
        sauve.write(s);
    print(v,i);
}

sauve.close();
