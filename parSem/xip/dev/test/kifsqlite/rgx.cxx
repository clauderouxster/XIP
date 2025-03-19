use("kifregex");

string rgx='\w+day';
string str="Yooo Wesdenesday Saturday";
vector vrgx=str.regex(rgx); //['Wesdenesday','Saturday']
string s=str.regex(rgx); //Wesdenesday
int i=str.regex(rgx);	// position is 5
println(vrgx,s,i);

rgx='(\d{1,3}):(\d{1,3}):(\d{1,3}):(\d{1,3})';
str='1:22:33:444';
vrgx=str.splitrgx(rgx);	// [1,22,33,444]
println(vrgx);

str='1:22:33:4444';
vrgx=str.splitrgx(rgx);	//[]
println(vrgx);


str="A_bcde";
if (str.regex('[a-zA-Z]_.+'))
	println("Yooo");	//Yooo


function display(int e,int j)  {
	println("DISPLAY:",e);
	e+=10+j;
	return(e);
}

call myfunc;
myfunc=display;
i=myfunc(100,1000);		//display: DISPLAY:100
print("I=",i,"\n");			//display: I=1110

string sp="[1,2,3,4,5]";
vector v=sp.vector();
v.clear();
v.push(display);
v.push(myfunc);
v.apply(-1,-1);

iterator ii;
ii=v;
for (ii.begin();ii.nend();ii.next())
	ii.apply(-1,-10);


