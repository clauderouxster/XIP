f=open(r"C:\eclipse\XIPLING\input.txt","r")
lists=[]
dicos={}
for e in f:
    ws=e.strip().split(" ");
    lists.extend(ws)

for iw in range(0,len(lists),2):
    try:
        w=lists[iw]+" "+lists[iw+1]
    except:
        w=lists[iw]
    s=list(w)
    keys=[0,0]
    for i in s:
        toggle=1
        for k in range(0,len(keys)):
            keys[k]+=ord(i)+k*toggle
            toggle*=-1

    for k in range(0,len(keys)):
        keys[k]%=1000
    key=keys[1]
    try:
        if w not in dicos[key]:
            dicos[key].append(w)
    except:
        dicos[key]=[w]

print dicos
f.close()
