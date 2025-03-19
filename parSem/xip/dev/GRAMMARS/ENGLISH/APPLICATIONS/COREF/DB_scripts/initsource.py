Python:

initsource(string pathname).

def initsource(pathname):
    global cursor

    print pathname
    ss='select count(*) from textsourceindexes;'
    cursor.execute(ss)
    dd=cursor.fetchone()
    v=dd[0]
    pathname=pathname.replace("\\","\\\\")
    ss='insert into textsourceindexes values('+str(v)+',"'+pathname+'");'
    print ss
    cursor.execute(ss)
    return v


    

