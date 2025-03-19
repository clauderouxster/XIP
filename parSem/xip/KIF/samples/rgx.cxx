kif:

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
	print("DISPLAY:",e,"\n");
	e+=10+j;
	return(e);
}

call myfunc;
myfunc=display;
i=myfunc.apply(100,1000);		//display: DISPLAY:100
print("I=",i,"\n");			//display: I=1110

pause(100);
