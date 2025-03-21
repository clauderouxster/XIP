Python:

getnbsent().



def getnbsent():
    global cursor

    ss='select count(*) from sentences;'
    cursor.execute(ss)
    dd=cursor.fetchone()
    v=dd[0]
    return v

    

