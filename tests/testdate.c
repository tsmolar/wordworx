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

hs_getdate(char *now) {
   char *cd,mon[5],day[4],year[6];
   time_t mmtime;
   mmtime=time(NULL);
   cd=asctime(localtime(&mmtime));
//   printf("%s\n",cd);
   hss_index(mon,cd,1,' ');
   hss_index(day,cd,2,' ');
   hss_index(year,cd,4,' ');
   if(strlen(day)==0) {
      hss_index(day,cd,3,' ');
      hss_index(year,cd,5,' ');
   }   
   sprintf(now,"%s %s, %s",mon,day,year);
}

main() {
   char *cd,mstr[20];
   time_t mmtime;
   
   mmtime=time(NULL);
   cd=asctime(localtime(&mmtime)); 
   hss_index(mstr,cd,3," ");
   printf("%s  :%s:\n",cd,mstr);
   hs_getdate(mstr);
   printf("%s\n",mstr);
}
