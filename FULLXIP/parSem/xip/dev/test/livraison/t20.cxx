frame test {

	int i;

	function value() {
		return(i);
	}
}


test t;
t.i=100;

self tt;
tt=t;
println(tt.type(),tt.value(),t.value());


frame block {
	window w;
	winput txt;
	string final;

	function gettext(button b,block bb) {
		final=txt.value();
		w.close();
	}

	function ajoute() {
		button b(this) with gettext;
		b.create(1230,20,30,30,FL_Regular,FL_NORMAL_BUTTON,"Ok");
	}

	function lance(string ph) {
		final=ph;
		w.begin(300,200,1300,150,"Modification");
		w.sizerange(10,20,0,0);				
		txt.create(200,20,1000,50,true,"Selection");
		txt.value(ph);
		ajoute();
		w.end();
		w.resizable(txt);
		w.run();
	}
}

function reprise(string ph) {
	block b;
	b.lance(ph);
	b.lance(b.final);
	b.lance(b.final);
	b.lance(b.final);
	return(b.final);
}

println(reprise("Ceci est un test"));
