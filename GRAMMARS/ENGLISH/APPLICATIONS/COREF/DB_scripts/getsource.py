Python:

getsource(string path).

def getsource(pth):
    global cursor

    if pth=="":
        pth="unknwon pathname"
    ss='select max(idsource) from textsourceindexes;'
    v=0
    try:
        cursor.execute(ss)
        dd=cursor.fetchone()
        v=dd[0]
        if v==None:
            v=0
        else:
            v=v+1
        pth='"'+pth.replace('\\','\\\\')+'"'
        ss='insert into textsourceindexes values('+str(v)+','+pth+');'
        cursor.execute(ss)
    except MySQLdb.Error,(error,description):
        print description
    return v

    

