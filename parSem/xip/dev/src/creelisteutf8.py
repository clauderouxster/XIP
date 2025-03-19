liste=[]
for i in range(33,1024,1):
    if i in [133,170,186]:
        continue
    s=unichr(i)

    try:
        x=ord(s.encode("latin1"));
    except:
        x=0
        pass

    if x==i:
        print x,i
    typ="n"
    equ="_"
    if s.isalpha():        
        if s.isupper():
            typ="u";
            equ=s.lower()
        else:
            typ="l";
            equ=s.upper()

    a1='"'+str(i)+'"'
    if s in ['"',"\\"]:
        a2='"\\'+s+'"'
    else:
        a2='"'+s+'"'
    a3='"'+str(x)+'"'
    a4='"'+typ+'"'
    a5='"'+equ+'"'
    item=[a1,a2,a3,a4,a5]
    liste.append(item)

        
f=open("tableutf8.txt","wb");
f.write("static char* table[]={")
debut=True
nb=0
for i in liste:
    if debut==False:
        f.write(",")
    debut=False
    s=",".join(i)
    s=s.encode("utf8")
    f.write(s)
    nb+=5

f.write("};\n");
f.write("static const int maxtable="+str(nb)+";\n");
f.close()

