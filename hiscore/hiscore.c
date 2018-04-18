#include <stdio.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <time.h>
#include <string.h>
#include "wwtypes.h"

#define HSVERSION "1.0.11"

typedef struct hscore_t {
   char name[30];
   char detail[40];
   char date[30];
   int score;
} hscore_t;

typedef struct hstable {
   char desc[80];
   char game[40];
   int numscores;
   hscore_t hscores[50];
} hstable;

hstable scoretbl[8];
int hs_numtbls=0,hs_cleanld=0;

// Query functions
int hs_numscores(int tno) {
   return(scoretbl[tno].numscores);
}

hs_names(int tno,char *desc,char *game) {
   strcpy(desc,scoretbl[tno].desc);
   strcpy(game,scoretbl[tno].game);
}

hs_getfield(char *ret, int tno, int sno, char *fname) {
   if(strcmp(fname,"name")==0)
     strcpy(ret,scoretbl[tno].hscores[sno].name);
   if(strcmp(fname,"detail")==0)
     strcpy(ret,scoretbl[tno].hscores[sno].detail);
   if(strcmp(fname,"date")==0)
     strcpy(ret,scoretbl[tno].hscores[sno].date);
   if(strcmp(fname,"score")==0)
     sprintf(ret,"%d",scoretbl[tno].hscores[sno].score);
}
// End Query Functions

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

hs_version_info() {
   printf("High Score Library version %s\n",HSVERSION);
#ifdef HAVE_LIBCURL
   printf("  * Using hsnet for internet scoring\n");
#else
   printf("  * hsnet disabled\n");
#endif
}

hs_getdate(char *now) {
   char *cd,mon[5],day[4],year[6];
   time_t mmtime;
   mmtime=time(NULL);
   cd=asctime(localtime(&mmtime));
//   printf("%s\n",cd);
   hss_index(mon,cd,1,' ');
   hss_index(day,cd,2,' ');
   if(strlen(day)==0) {
      hss_index(day,cd,3,' ');
      hss_index(year,cd,5,' ');
   } else 
     hss_index(year,cd,4,' ');
   sprintf(now,"%s %s, %s",mon,day,year);
}

hs_crypt(char *out, char *in, char *type, int key) { 
   int i;
//   unsigned char x;
   int x;
   if(strcmp(type,"RO")==0) {
      for(i=0;i<strlen(in);i++) {
	 x=in[i];
	 if(x>=32 && x<=125) {
	    x=x+key;
	    if(x>125)
	      x=x-94;
	 }
	 out[i]=x;
      }
      out[i]=0;
   } else {
      strcpy(out,in);
   }
}

hs_decrypt(char *out, char *in, char *type, int key) { 
   int i;
//   char x;
   int x;
   if(strcmp(type,"RO")==0) {
      for(i=0;i<strlen(in);i++) {
	 x=in[i];
	 if(x>=32 && x<=125) {
	    x=x-key;
	    if(x<32)
	      x=x+94;
	 }
	 out[i]=x;
      }
      out[i]=0;
   } else {
      strcpy(out,in);
   }
}


int hs_newtable(char *desc, char *game,int nscore) {
   hs_numtbls++;
   strcpy(scoretbl[hs_numtbls].desc,desc);
   strcpy(scoretbl[hs_numtbls].game,game);
   scoretbl[hs_numtbls].numscores=nscore;
   hs_cleartable(hs_numtbls);
   return (hs_numtbls);
}

hs_loadtable(char *fn) {
   FILE *fp;
   char al[256],dal[256],frcc[6],wst[50],cmth[5],es;
   int ver,hskey,ti,hi,tmp1;
   
   fp=fopen(fn,"rb");
   if(fp!=NULL) {
      // Read Header
      fgets(al,256,fp);
      hss_index(frcc,al,0,'|');  // Header ID
      hss_index(wst,al,1,'|');   // File Version   
      ver=atoi(wst);
      hss_index(cmth,al,2,'|');  // Crypt Method
      hss_index(wst,al,3,'|');  // Crypt key
      hskey=atoi(wst);
      hss_index(wst,al,4,'|');  // Entry Start character
      es=wst[0];
      hss_index(wst,al,5,'|');   // Number of tables
      hs_numtbls=atoi(wst);
//      printf("Loading a version %d.%d file\n",ver/10,ver%10);
      while(!feof(fp) && strcmp(frcc,"HSTB")==0) {
	 fgets(al,256,fp);
	 hs_decrypt(dal,al,cmth,hskey);
	 hss_index(wst,dal,0,'|');
	 
	 if(strcmp(wst,"TABLE")==0) { 
	    hss_index(wst,dal,1,'|');
	    ti=atoi(wst);
	    hss_index(wst,dal,2,'|');
	    strcpy(scoretbl[ti].desc,wst);
	    hss_index(wst,dal,3,'|');
	    strcpy(scoretbl[ti].game,wst);
	    hss_index(wst,dal,4,'|');
	    scoretbl[ti].numscores=atoi(wst);
	    hi=1;
	 } 
	 if(strlen(wst)==1 && wst[0]==es) {
//	    printf("%s",dal);
	    if(hi<=scoretbl[ti].numscores) {
	       hss_index(wst,dal,3,'|');
	       strcpy(scoretbl[ti].hscores[hi].name,wst);
	       hss_index(wst,dal,4,'|');
	       strcpy(scoretbl[ti].hscores[hi].detail,wst);
	       hss_index(wst,dal,1,'|');
	       strcpy(scoretbl[ti].hscores[hi].date,wst);
	       hss_index(wst,dal,2,'|');
	       scoretbl[ti].hscores[hi].score=atoi(wst);
	       hi++;
	    }
	 }
	 
	 if(strcmp(wst,"ENDTABLE")==0) { 
	    hss_index(wst,dal,1,'|');
	    if(atoi(wst)==hs_numtbls) {
	       hs_cleanld=1;
	       break;
	    }
	 }
      }
   fclose(fp);
   }
}

int hs_iscorrupt(int xnumtables) {
   int rv=0;
   // printf("hs_cleanld: %d\n",hs_cleanld);
   if(hs_cleanld!=1)
     rv=1;
   // printf("xnumt: %d  hs_numtbls: %d\n",xnumtables,hs_numtbls);
   if(xnumtables!=hs_numtbls)
     rv=1;
   return(rv);
}

hs_deleteall(){
   hs_numtbls=0;
}

int hs_random(int min, int max) {   
   int x,c,sp;
   
   sp=max-min;
   x=rand();
   c=x%sp + min;
   return(c);
}

hs_writetable(char *fn) {
   FILE *fp;
   char dbuf[256], cbuf[256], etype[]="RO";
   unsigned char hchar;
   int i,h,ckey;
   
   ckey=hs_random(12,57);
   hchar=hs_random(35,64);
   // printf("attempting to write to %s\n",fn);
   // printf("hchar is %c (%d) ckey is %d\n",hchar,hchar,ckey);
   if(fp=fopen(fn,"wb")) {
      fprintf(fp,"HSTB|010|%s|%d|%c|%d\n",etype,ckey,hchar,hs_numtbls);
      for(i=1;i<=hs_numtbls;i++) {
	 sprintf(dbuf,"TABLE|%d|%s|%s|%d",i,scoretbl[i].desc,scoretbl[i].game,scoretbl[i].numscores);
	 hs_crypt(cbuf,dbuf,etype,ckey);
	 fprintf(fp,"%s\n",cbuf);
	 for(h=1;h<=scoretbl[i].numscores;h++) {
//	    sprintf(dbuf,"%c|%s|%s|%s|%d|",hchar,scoretbl[i].hscores[h].name,scoretbl[i].hscores[h].detail,scoretbl[i].hscores[h].date,scoretbl[i].hscores[h].score);
	    sprintf(dbuf,"%c|%s|%d|%s|%s|",hchar,scoretbl[i].hscores[h].date,scoretbl[i].hscores[h].score,scoretbl[i].hscores[h].name,scoretbl[i].hscores[h].detail);
	    hs_crypt(cbuf,dbuf,etype,ckey);
	    fprintf(fp,"%s\n",cbuf);
	 }
	 sprintf(dbuf,"ENDTABLE|%d",i);
	 hs_crypt(cbuf,dbuf,etype,ckey);
	 fprintf(fp,"%s\n",cbuf);
      }
      fclose(fp);   
   } else
     printf("WARNING! Could not write to highscore file!\n");
}

hs_printtable() {
   int i,h;
   
   printf("\n-------------------------------\n\n");
   for(i=1;i<=hs_numtbls;i++) {
      printf("TABLE %d %s %s %d\n",i,scoretbl[i].desc,scoretbl[i].game,scoretbl[i].numscores);
      for(h=1;h<=scoretbl[i].numscores;h++) {
	 printf("# %s %s %s %d \n",scoretbl[i].hscores[h].name,scoretbl[i].hscores[h].detail,scoretbl[i].hscores[h].date,scoretbl[i].hscores[h].score);
      }
      printf("ENDTABLE %d\n",i);
   }
}

int hs_check(int tno,char *desc, char *game) {
   int rv=1;
   if(strcmp(desc,scoretbl[tno].desc)!=0)
     rv=0;
   if(strcmp(game,scoretbl[tno].game)!=0)
     rv=0;
   return(rv);
}

hs_insertscore(int tno,char *name, char *detail, char *date, int score) {
   int h,i;
   
   for(h=1;h<=scoretbl[tno].numscores;h++) {
      if(score>scoretbl[tno].hscores[h].score) {
	 for(i=scoretbl[tno].numscores;i>h;i--) {
	    scoretbl[tno].hscores[i].score=scoretbl[tno].hscores[i-1].score;
	    strcpy(scoretbl[tno].hscores[i].name,scoretbl[tno].hscores[i-1].name);
	    strcpy(scoretbl[tno].hscores[i].detail,scoretbl[tno].hscores[i-1].detail);
	    strcpy(scoretbl[tno].hscores[i].date,scoretbl[tno].hscores[i-1].date);
	 }
	 strcpy(scoretbl[tno].hscores[h].name,name);
	 strcpy(scoretbl[tno].hscores[h].detail,detail);
	 strcpy(scoretbl[tno].hscores[h].date,date);
	 scoretbl[tno].hscores[h].score=score;
	 break;
      }
   }
}

int hs_ishiscore(int tno, int score) {
   int h, rv=0,j;
   
   
   j=scoretbl[tno].numscores;
   if(score>scoretbl[tno].hscores[j].score)
     rv=1;
   if(tno>hs_numtbls) rv=0; // To allow for checking against internet tables
                            // That may not always be present
   return(rv);
}

hs_cleartable(int tno) {
   int j,i;
   j=scoretbl[tno].numscores;
   for(i=1;i<=j;i++) {
      scoretbl[tno].hscores[i].score=0;
      scoretbl[tno].hscores[i].name[0]=0;
      scoretbl[tno].hscores[i].detail[0]=0;
      scoretbl[tno].hscores[i].date[0]=0;
   }
}

