#include <stdio.h>

hss_index(char* rstr, char *ostr, int idx, char del) {
   // Alternative to strtok
   char *gidx;
   int t=0,i=0,si=0,ei=0,sl;
   
   gidx=ostr;
       
   do {
      if(gidx[t]==del || gidx[t]=='\n' || gidx[t]==0) {
	 i++;
	 if(i==idx) si=t+1;
	 if(i==idx+1) { ei=t; break; }
      }
      
      if(gidx[t]==0) break;
      t++;
   } while(1);
   
   sl=ei-si;
   if(sl<1) strcpy(rstr,"");
   else {
      gidx=gidx+si;
      for(i=0;i<sl;i++) {  
	   rstr[i]=gidx[i];
	} 
      rstr[i]=0;
   }   
}

main() 
{
   char xx[30],aw[20],as[10];
   int sc;
   char ws[]="test(10)hest(20)jest(23)frog(33)goonba(19)";
   hss_index(xx,ws,6,')');
   hss_index(aw,xx,0,'(');
   hss_index(as,xx,1,'(');
   sc=atoi(as);
   printf("%s %d |%d|\n",aw,strlen(xx),sc);
}
