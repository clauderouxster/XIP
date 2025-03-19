window w;
map styles;
editor wo;
int ifont=0;

//we modify the current style of the editor to reflect the selected font
function fontchoice(int i) {
	//we create a new default style, whose font id is i
	styles["#"]=[FL_BLACK,i,16];
	wo.addstyle(styles);
	//we modify the title of the editor label to reflect the current font name
	wo.label(w.getfont(i)+":"+i);	
	//to be sure that the label will be correctly we re-display the whole window
	w.redraw();
}


//Whenever the "next" button is pressed we change our current font
function change(button b,int i) {
	fontchoice(ifont);
	ifont++;	
}



button b(ifont) with change;

w.begin(50,50,800,500,"Font Display");
w.sizerange(10,10,0,0);


int i;
//First we load our font from the system, to see which fonts are available
int nb=w.initializefonts();
//int nb=w.fontnumber();

wo.create(70,30,730,460,"Fonts");
//we use a default and available anywhere font
styles["#"]=[FL_BLACK,FL_HELVETICA,16];
wo.addstyle(styles);

//we loop among all available fonts to display both their name 
//and their available sizes. [0] means that every size is available
string s,fonts;
vector v;
for (i=0;i<nb;i++) {
	if (fonts!="")
		fonts+="\r";
	s=w.getfont(i);
	v=w.getfontsizes(i);
	fonts+=i+":"+s+"="+v;
}

//we store these names as the content of the editor
wo.value(fonts);

//the next button
b.create(10,10,40,30,FL_Regular,FL_NORMAL_BUTTON,"Next");
w.end();
w.resizable(wo);

w.run();
