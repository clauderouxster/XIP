function myfunc(string s) {
	println(s);
}

function mymap(map m) {
	println(m);
}

map m={"a":1,"b":2};

myfunc("Test");
myfunc(18);
myfunc(m);

mymap("{'a':1,'b':2}");

//A very simple declaration
function myfuncs(self s) {
	s+= "!";
}

string t= "toto";
myfuncs(t);	     
println(t); //t is now toto!

vector v=["a","b","c"]; //my vector

v.apply(myfuncs); //we apply our function
println(v); //v=["a!","b!","c!"]
