//We connect to our local server.
socket s;

string name=s.gethostname();
s.connect(name,2012);
//The we create a remote object: r
remote r;
//We get from the server a handle on toto, which is implemented on the server side
r=s.getframe("toto");

//We display the available methods from the frame description of toto, which have been returned by the server.
println("Functions:",r);

println("Values of toto:",r.Value(),r.Compute(345));

//We then fetch a function: create
r=s.getfunction("Create");


//Which we can use to execute the object create on the server side...
r.Create("titi",123);

//we can now fetch a titi object, that was created thanks to the above instruction
r=s.getframe("titi");
//And we display its values
println("Valeur de titi:",r.Value(),r.Compute(345));

//We kill the server, by sending the killing string.
s.write("stop");
s.close();

