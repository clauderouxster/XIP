"""
python:

testunique(#1),
testuniquecat(#1).
"""

def testunique(i):
    n=XipNode(i)
#    print 'all: ', n.surface
#    print 'all: ', n.pos
    if type(n.pos)==type([]):
       return 0
    return 1




def testuniquecat(i):    
    n=XipNode(i)
#    print 'cat: ', n.surface
#    print 'cat: ', n.pos
    if type(n.pos)==type([]):
        s=set(n.pos)
    else :
        s=[n.pos]
        
#    print 'set: ', s
#    print 'set: ', len(s)
    if len(s)==1 :
        return 1
    return 0




