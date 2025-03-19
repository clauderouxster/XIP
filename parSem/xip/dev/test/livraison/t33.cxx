int i=65535;
vector v=["FULL",'ONE','TWO','THREE','FOUR','FIVE','SIX','SEVEN','EIGHT','NINE','TEN','ELEVEN','TWELVE','THIRTEEN','FOURTEEN','FIFTEEN'];
vector vx=["NONE",'ONE','TWO','THREE','FOUR','FIVE','SIX','SEVEN','EIGHT','NINE','TEN','ELEVEN','TWELVE','THIRTEEN','FOURTEEN','FIFTEEN'];
int j=1;

while (j<16) {
    println("const unsigned int P_ATLEAST"+v[j]+' = P_ATLEAST'+v[j-1]+'-P_'+vx[j-1]+';');
   j++;
}

