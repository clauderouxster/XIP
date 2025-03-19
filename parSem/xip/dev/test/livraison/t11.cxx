//a thread waiting on strings…
thread mythread(int k) {
       string s=wait("test","off");
       println(s+","+k);  // print: “off,10”
}

time ti;
println(_args);
mythread(10); 	//We launch it
pause(_args[0]);
cast("off"); 	//We release it
time tf;

float d=tf-ti;
println(d);

