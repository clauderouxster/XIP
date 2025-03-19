kif:

parser p;

p.load(_args[0]);


function parsing(string s) {
	string r;
	r=p.parse(s);
	return(r);
}



