def invertsort(x,y):
   if x[0]<y[0]:
       return 1
   if x[0]==y[0]:
       return 0
   return -1


l=[[4,4],[6,9],[1,2],[3,2],[10,6]]
l.sort(invertsort)
