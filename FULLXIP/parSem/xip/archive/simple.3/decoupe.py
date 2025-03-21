f=open(r"C:\ANL\Mekalinga\ANL\Mekalinga\simple\base.txt","r")
txt=[]
for i in f:
    l=i.replace("\n"," ")
    j=l.split(". ")
    txt.extend(j)
f.close()

w=open(r"C:\ANL\Mekalinga\ANL\Mekalinga\simple\input.txt","w")
for t in txt:
    t=t.strip()
    if t=="":
        continue
    t=unicode(t,"latin1").encode("utf8")
    w.write(t+".\n")
w.close()

