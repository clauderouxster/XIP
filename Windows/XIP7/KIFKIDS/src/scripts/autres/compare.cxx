file fbase('/Users/rouxclaude/Développement/kifkids/src/sauve/frenchstrings.cxx','r');
file feng('/Users/rouxclaude/Développement/kifkids/src/frenchstrings.cxx','r');

string s;
vector vbase;
vector veng;

for (s in fbase) {
    if ('//' in s)
        vbase.push(s[:" //"].trim()[1:-2]);
}

for (s in feng) {
    if ('//' in s)
        veng.push(s[:" //"].trim()[1:-2]);
}

int i;

file fsauve('/Users/rouxclaude/Développement/kifkids/src/sauve/traductions','w');
while (i<veng.size()) {
    fsauve.write(veng[i]+"\t"+vbase[i]+"\r");
    i++;
}
fsauve.close();
