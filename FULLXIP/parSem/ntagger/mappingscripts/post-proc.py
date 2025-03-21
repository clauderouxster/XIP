#! /usr/bin/env python

import re
import os 
import string
import sys

inputfile = open(sys.argv[1],'r')
doc = inputfile.read()
doc=doc.replace("./.\n\n./.\n\n./.\n\n./.\n\n","..../:\n")
doc=doc.replace("./.\n\n./.\n\n./.\n\n",".../:\n")


lines=doc.split("\n")
done={}
for i in range(len(lines)-1):
    if (lines[i].find("/CD") != -1 and lines[i+1].find("./.") !=-1 and done.has_key(i) == -1 ):
        toprint=lines[i].replace("/CD","./CD")
        print toprint
        done[i]=1
        done[i+1]=1     
    if lines[i+1] == "///RL" :
        mot1=lines[i]
        i1=mot1.index("/")
        if lines[i+3] == "///RL" :
            mot2=lines[i+2]
            i2=mot2.index("/")
            toprint =  lines[i][0:i1] + "\/" + lines[i+2][0:i2] + "\/" +  lines[i+4]
            done[i]=1
            done[i+1]=1
            done[i+2]=1
            done[i+3]=1
            done[i+4]=1
            print toprint
        else:
            if ( done.has_key(i)!=1):
                toprint =  lines[i][0:i1] + "\/" + lines[i+2]
                done[i]=1
                done[i+1]=1
                done[i+2]=1
                print toprint
    if ( lines[i+1] == "-/-RL" and lines[i] !=""):
        mot1=lines[i]
        i1=mot1.index("/")
        if lines[i+3] == "-/-RL" :
            mot2=lines[i+2]
            i2=mot2.index("/")
            if  lines[i+5] == "-/-RL" :
                mot3=lines[i+4]
                i3=mot3.index("/")
                toprint = lines[i][0:i1] + "-" + lines[i+2][0:i2] + "-" + lines[i+4][0:i3] +  "-" + lines[i+6]
                done[i]=1
                done[i+1]=1
                done[i+2]=1
                done[i+3]=1
                done[i+4]=1
                done[i+5]=1
                done[i+6]=1
                print toprint
            else:
                if  ( done.has_key(i)!=1):
                    toprint =  lines[i][0:i1] + "-" + lines[i+2][0:i2] + "-" +  lines[i+4]
                    done[i]=1
                    done[i+1]=1
                    done[i+2]=1
                    done[i+3]=1
                    done[i+4]=1
                    print toprint
        else:
            if ( done.has_key(i)!=1):
                toprint =  lines[i][0:i1] + "-" + lines[i+2]
                done[i]=1
                done[i+1]=1
                done[i+2]=1
                print toprint
    else:
        if done.has_key(i) != 1:
            print lines[i]
#print lines[len(lines)]
    
    
