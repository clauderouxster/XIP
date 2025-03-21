use('c:\XIP\Debug\kifltk.dll');
frame block {
	window w;
	self txt;
	string final;

	function gettext(button b,block bb) {
		final=txt.value();
		w.close();
	}

	function lance(string ph) {
		final=ph;
		w.begin(300,200,1300,150,"Modification");
		w.sizerange(10,20,0,0);		
		winput wtxt;
		txt=wtxt;
		wtxt.create(200,20,1000,50,true,"Selection");
		wtxt.value(ph);
		button b(this) with gettext;
		b.create(1230,20,30,30,FL_Regular,FL_NORMAL_BUTTON,"Ok");
		w.end();
		w.resizable(txt);
		w.run();
	}
}

function reprise(string ph) {
	block b;
	b.lance(ph);
	return(b.final);
}

reprise("Ceci est un test");
