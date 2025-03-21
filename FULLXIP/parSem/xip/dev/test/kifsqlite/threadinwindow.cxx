

int px=300;
int py=400;
int nb=1;

map fenetres;

function fermeture(window w, int n) {
	fenetres.pop(n);
	return(true);
}

//Once triggered, this thread will increment the coordinates et forces a redraw of the window for each new value.
thread bouge(woutput val) {
	int i=0;
	window wx;
	woutput wo;
	string errs;
	int nbx=nb;
	
	wx.timeout(1);
	wo.timeout(1);
	lock("creation");
	try {
		wx.begin(px,py,250,100,"ICI:"+nb);
		fenetres[nb]=wx;
		wo.create(50,20,120,30,true,"Valeur");	
		wx.end();
		px+=300;
		nb++;
		if (px>=1800) {
			px=300;
			py+=150;
		}		
	}
	catch(errs) {							 
		printlnerr(errs[:".cxx"],threadid());
		if (wx.created())
			wx.close();
		unlock("creation");
		return;
	}
	

	try {
		val.value("="+px+":"+py);
	}
	catch();

	unlock("creation");
	wo.timeout(0.01);
	string err;
	while (true) {			
		i++;
		try {
			wo.value(i);		
		}
		catch(err) {
			if ("Time out" in err)
				continue;			
			if (wx.created())
				wx.close();
			return;
		}
	}
}


function pressed(button b,woutput w) {	
	bouge(w);
}

function reset(button b,woutput w) {
	px=300;
	py=400;
	w.value("="+px+":"+py);
}

function fermer(button b,map f) {
	px=300;
	py=400;
	iterator it=f;
	for (it.begin();it.nend();it.next())
		it.value().close();
}

//we declare our window together with its associated drawing function and the object coord
window wnd;
woutput wo;
//We need to instanciate the mouse call back
wnd.begin(100,50,500,300,"Drawing");
wo.create(50,50,100,30,true,"Valeur");
wo.timeout(1);
wo.value("="+px+":"+py);
button b(wo) with pressed;
b.create(430,20,60,60,FL_Regular,FL_NORMAL_BUTTON,"Ok");
button bb(wo) with reset;
bb.create(430,90,60,60,FL_Regular,FL_NORMAL_BUTTON,"RAZ");
button bbb(fenetres) with fermer;
bbb.create(430,160,60,60,FL_Regular,FL_NORMAL_BUTTON,"Fermer");
wnd.sizerange(10,10,0,0);
//we add a mouse call back
wnd.end();

wnd.run();
