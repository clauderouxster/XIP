map numbers={"zo":0,"u":1,"do":2,"tri":3,"tre":4,"ci":5,"si":6,"se":7,"o":8,"no":9,"di":10,"cen":100,"tun":1000,"mi":1000000,"bi":1000000000};

function number(string s,vector val,boolean first,vector vlx);

function evalue(string s,vector val,vector vlx,int sz) {
    int ln=s.size();
    if (ln==0)
        return(true);
    if (ln<sz)
        return(false);
    string start=s[:sz];
    if (numbers[start]!=null) {
        val.push(numbers[start]);
        return(number(s[sz:],val,false,vlx));
	}
    return(false);
}

function number(string s,vector val,boolean first,vector vlx) {
	//we start with three letters
	if (not evalue(s,val,vlx,3)) {
		if (not evalue(s,val,vlx,2)) {
			if (not evalue(s,val,vlx,1)) {
				return(false);
			}
		}
	}

	if (first) {
		int vl=0;
		int i=0;
		while (i<val.size()) {
			int nb=val[i];
			if (i+1<val.size()) {
				int nbnext=val[i+1];
				if (nb<nbnext)
					val[i+1]=nb*nbnext;
				else
					vl+=nb;
			}
			else
				vl+=nb;
			i+=1;
		}
		vlx[0]=vl;
	}
	return(true);
}

function test() {
    vector vl=[0];
    number("tuncicendodino",[],true,vl);
    println(vl);
}

test();
