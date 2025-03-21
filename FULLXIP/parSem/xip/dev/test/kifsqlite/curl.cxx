use('kifcurl');

function fonc(curl c,string s,self o) {
    vector v;
    v=s.extract(0,'"','"');
    println(v);
}


curl c;

c.proxy("http://cornillon.grenoble.xrce.xerox.com:8000");
c.url("http://www.liberation.fr/",'/opt/ParSem/users/Roux/xip/test.txt');
c.url("http://www.liberation.fr/",'/opt/ParSem/users/Roux/xip/test2.txt');
//c.url("http://www.liberation.fr/");
