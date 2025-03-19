# -*- coding: utf8 -*-
######################################################
#Author: Claude Roux
#Date: 2008/01/01
#E-mail: mekalinga@free.fr
#File: numbers.py
#Comment: This file implements the computing of a number out of a mekalinga number expression
#Copyright: This file can be freely copied and used as long as the original author (Claude Roux) is present

################################################################################
#Numbers

numbers={"zo":0,"u":1,"do":2,"tri":3,"tre":4,"ci":5,"si":6,"se":7,"o":8,"no":9,
         "di":10,"cen":100,"tun":1000,"mi":1000000,"bi":1000000000}

def stringnumbers():
    nb={}
    for i in numbers.keys():
        nb[numbers[i]]=i
    s="Numbers:\n"
    for n in [0,1,2,3,4,5,6,7,8,9,10,100,1000,1000000,1000000000]:
        s+="o\t"+str(n)+":"+nb[n]+"\n"
    print s

def evalue(s,val,vlx,sz):
    ln=len(s)
    if ln==0:
        return True
    if ln<sz:
        return False
    start=s[:sz]
    if start in numbers:
        val.append(numbers[start])
        return number(s[sz:],val,False,vlx)
    return False
            
def number(s,val,first,vlx):
    #we start with three letters
    if not evalue(s,val,vlx,3):
        if not evalue(s,val,vlx,2):
            if not evalue(s,val,vlx,1):
                return False
    if first:
        vl=0
        i=0
        while i<len(val):
            nb=val[i]
            if i+1<len(val):
                nbnext=val[i+1]
                if nb<nbnext:
                    inter=nb*nbnext
                    val[i+1]=inter
                else:
                    vl+=nb
            else:
                vl+=nb
            i+=1
        vlx[0]=vl
    return True

def test():
    vl=[0]
    number("tuncicendodino",[],True,vl)
    print vl
    
#test()

