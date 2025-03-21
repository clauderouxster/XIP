kif:

time t1;
string path=_paths[0]+_sep;

file f;
f.openread(_args[0]);
string s;
string sx;
string bsx;
string svalue;
vector v;
vector vv;
vector vx;
boolean im;
printlnerr(t1);


while (s in f) {
	v=s.split(" ");
	
	sx="";
	try {
		im=false;
		bsx=v[5].split("/")[0]+"=";
	}
	catch(svalue) {
		printlnerr(svalue);
	}

	if ("image" in v[5]) {
		sx=v[5];
		im=true;
		if (".gif" in sx)
			continue;
	}
	elif ("searchLetter" in v[6] || "key" in v[6])
			sx=v[6];	

	
	if (sx!="") {		
		if (im==true)
			vv=sx.split('/');
		else
			vv=sx.split("&");
		while (svalue in vv) {			
			if (im==false) {
				if ("%20" in svalue) {
					svalue.split("%20",v);
					if ("artist=" in v[0]) {
						vx=v[0].split("=");
						v[0]="artist";
						v.insert(1,vx[1]);
					}
					println(bsx,v);
				}
			}
			else {
				if ("-" in svalue) {
					svalue.split('-',v);
					println(bsx,v);
				}
			}
		}	
	}
}
f.close();
time t2;
float yyy;
yyy=(t2-t1)/1000;
printlnerr(yyy);
