import compilateur
def lance():
    i=compilateur.items('C:\\Documents and Settings\\roux\\workspace\\Genere\\items.xml')
    r=compilateur.rules('C:\\Documents and Settings\\roux\\workspace\\Genere\\rules.xml')
    p=compilateur.plan('C:\\XIP\\Test\\english\\Generation\\plan.xml')
    f=open("C:\\Documents and Settings\\roux\\workspace\\Genere\\src\\testage.py","w")
    f.write(i)
    f.write(r)
    f.write(p)
    f.close()

lance()
from testage import *


xx=plan()


