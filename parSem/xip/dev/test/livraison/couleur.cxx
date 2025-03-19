woutput o;

//our new display...
//Every time the window will be modified, this function will be called with a mycood object
function display(window w,vector cols) {	
	//we select our color, which will be apply to all objects that follow
	int col=w.rgbcolor(cols);
	w.rectanglefill(150,150,250,250,col);
	o.value(col);
}


vector couleurs=[127,127,127];

//we declare our window together with its associated drawing function and the object coord
window wnd(couleurs) with display;

function rouge(slider s,vector cols) {
	cols[0]=s.value();
	wnd.redraw();
}
function vert(slider s,vector cols) {
	cols[1]=s.value();
	wnd.redraw();
}
function bleu(slider s,vector cols) {
	cols[2]=s.value();
	wnd.redraw();
}

slider vrouge(couleurs) with rouge;
slider vvert(couleurs) with vert;
slider vbleu(couleurs) with bleu;

//We need to instanciate the mouse call back
wnd.begin(100,100,500,500,"Drawing");
wnd.sizerange(10,10,0,0);

vrouge.create(50,10,180,20,FL_ALIGN_RIGHT,true,"Rouge");
vrouge.bounds(0,255);
vrouge.value(127);

vvert.create(50,40,180,20,FL_ALIGN_RIGHT,true,"Vert");
vvert.bounds(0,255);
vvert.value(127);
vbleu.create(50,70,180,20,FL_ALIGN_RIGHT,true,"Bleu");
vbleu.bounds(0,255);
vbleu.value(127);

o.create(360,10,130,34,false,"Couleur");
wnd.end();
wnd.run();
