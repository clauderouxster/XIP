
//we test wether one value is larger than the other
function test(int i,int j) {
	if (j>=i)
		return(true);
	return(false);
}

int s=10;

//We test through test
switch (s) with test {
	1:println("1");
	2:println("2");
	20:println("20");
	default: println("aucune valeur");
}

