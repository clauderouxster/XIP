s="["
for i in range(97,97+26,1):
    c=chr(i)
    if s=="[":
        s+=c
    else:
        s+=" | "+c
    if c in ['a','e','i','o','u']:
        for a in ['1','2','3','4']:
            s+=" | "+c+a
s+="];"
print s


        
    
