kif:


string res;
vector v;
self xself;
boolean im;

kif bib;

bib.load('bibkif.cxx');
i=bib.appelage(30);
print("Bib: ",i,"\n");

parser p;

p.load('french.grm');
string mess;
i=p.affich(10);
print("Retour: ",i,"\n");

p.parsefile('texte.txt','resultat.txt');

