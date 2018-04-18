#include <stdio.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <string.h>
#include "wwtypes.h"
#include "tiledata.h"

// For sleep
#ifdef WIN32
# ifndef USESDL
#  include<windows.h>
# endif
#else
# include<time.h>
# include<errno.h>
#endif

// disable hsnet
//char fsmode='y', dqmode='n', gmute='n', hsnmode='y';
char fsmode='y', dqmode='n', gmute='n', hsnmode='n';
#ifdef USEGFX
char restr[20];
int minx=640, miny=480, usex=640, usey=480;
int rx0, ry0, wordcount, WSmode; // Where screen starts
#endif
extern int hs_numtbls;
#ifdef HAVE_LIBCURL
char tilepilest[]="123456789abc123456789abc123456789abc123456789abc123456789abc123456789abc123456789abc";
#endif
// #include "fontdata.h"

int ww_sleep (int ms) {
#ifndef WIN32
   struct timespec tv;
   /* Construct the timespec from the number of whole seconds... */
   tv.tv_sec = (time_t) ms /1000;
   /* ... and the remainder in nanoseconds. */
   tv.tv_nsec = (long) ((ms - (tv.tv_sec*1000)) * 1e+6);
   
   while (1) {
      /* Sleep for the time specified in tv. If interrupted by a
       *     signal, place the remaining time left to sleep back into tv. */
      int rval = nanosleep (&tv, &tv);
      if (rval == 0)
	/* Completed the entire sleep time; all done. */
	return 0;
      else if (errno == EINTR)
	/* Interrupted by a signal. Try again. */
	continue;
      else 
	/* Some other error; bail out. */
	return rval;
   }
#else
# ifdef USESDL
   SDL_Delay(ms);
# else
   Sleep(ms);
# endif // USESDL
#endif
   return 0;
}

int d_random(int min, int max) {
   int x,c;
   x=rand();
   c=x%max + min;
   return(c);
}

fill_bag() {
   int i;
   
   for(i=0;i<98;i++) 
     bag[i]=tilear[i];
   num_inbag=98;
}

char pick_frombag() {
   int i;
   char r;
   i=d_random(0,num_inbag);
   r=bag[i];
   num_inbag--;
   bag[i]=bag[num_inbag];
   return(r);
}

void debug_setup() {
   // pull off letters
   int x,y;
   for(y=5;y<8;y++)
     for(x=1;x<=12;x++) {
	ltrpile[x][y]=' ';
	num_inpile--;
     }
}



void reset_pile() {
   int x,y;
   
   num_inpile=0;
   for(y=1;y<=numrows;y++) {
	for(x=1;x<=12;x++) {
	   ltrpile[x][y]=pick_frombag();
	   num_inpile++;
#ifdef HAVE_LIBCURL
	   tilepilest[(x-1)+((y-1)*12)]=ltrpile[x][y];
#endif
	}
     }
   currword[0]=0;
   cwidx=0;
}

void reset_game() {
   srand(time(NULL));
   fill_bag();
   reset_pile();
   totscore=0;
   currscore=0;
   mywords[0]=0;
#ifdef USEGFX
   gfx_reset_flags();
   if(WSmode==1) 
     gfx_set_widescreen();
   // This will be called everytime the game is restarted, check for 
   // memory leak!
   gfx_setup_buttons();
#endif
}


void down_col(int colno) {
   int y;
   for (y=numrows;y>1;y--)
     ltrpile[colno][y]=ltrpile[colno][y-1];
   ltrpile[colno][1]='*';
}

void up_col(int colno) {
   int y;
   for (y=1;y<numrows;y++)
     ltrpile[colno][y]=ltrpile[colno][y+1];
   ltrpile[colno][numrows]=' ';
}
int num_letincol(int colno) {
   int y,rv=0;
   for (y=1;y<=numrows;y++) {
      if(ltrpile[colno][y]!=' ')
	rv++;
   }
   return(rv);
}

int pick_letter(int colno) {
   int rv=0;
   if(cwidx<12 && ltrpile[colno][1]!=' ') {
      currword[cwidx]=ltrpile[colno][1];
      cwreturn[cwidx]=colno;
      cwidx++;
      currword[cwidx]=0;
      up_col(colno);
      num_inpile--;
   } else {
      rv=1;
#ifndef USEGFX
      if(cwidx>11) 
	printf("Maximum length reached!\n");
      if(ltrpile[colno][1]==' ')
	printf("Column is empty!\n");
#endif
   }
   return(rv);
}

void rm_letter() {
   int curcol;
   if(cwidx>0) {
      cwidx--;
      curcol=cwreturn[cwidx];
      down_col(curcol);
      ltrpile[curcol][1]=currword[cwidx];
      currword[cwidx]=0;
      num_inpile++;
   } else 
      printf("Can't remove Letter\n");
    
}


void display_pile() {
#ifdef USEGFX
   gfx_display_pile();
#else
   int x,y;

   printf(" 1  2  3  4  5  6  7  8  9 10 11 12\n");
   printf("-----------------------------------\n");
   for(y=1;y<=numrows;y++) {
	for(x=1;x<=12;x++) {
	   printf(" %c ",ltrpile[x][y]);
	}
      printf("\n");
   }
#endif
}

void display_curword() {
#ifdef USEGFX
   gfx_display_curword();
#else
   int i;
   for(i=0;i<12;i++) {
      if(currword[i]==0) break;
      printf(" %c ",currword[i]);
   }
   printf("\n\n");
#endif
}

int get_letter_score(char letter) {
   int ilt=letter-65;
//   printf("%c is %d\n",letter,scorear[ilt]);
   return(scorear[ilt]);
}


void calc_cwscore() {
   int i,ls;
   currscore=0;
   if(cwidx>2) {
      for(i=0;i<cwidx;i++) {
//	 ls=1;
	 ls=get_letter_score(currword[i]);
	 currscore=currscore+ls;
      }
      currscore=currscore*cwidx;
   }
}

int get_next_word(char *nextword,int cwc) {
//   char *cword;
   char wordp[24],cword[14],cscore[6];
   int rv;
   // return words and scores,  uses s_index
   hss_index(wordp,mywords,cwc,')');
   hss_index(cword,wordp,0,'(');
   hss_index(cscore,wordp,1,'(');
   rv=atoi(cscore);
   if(rv==0) 
      strcpy(nextword,"0");
   else
      strcpy(nextword,cword);
   return(rv);
}

//int get_next_word0(char *nextword,int cwc) {
//   char *cword;
//   int rv;
//   // return words and scores,  uses strtok
//   if(cwc==0) {
//      cword=strtok(mywords,"(");
//   } else {
//      cword=strtok(NULL,"(");
//   }
//   if(cword==NULL) {
//      rv=0;
//      strcpy(nextword,"0");
//   } else {
//      strcpy(nextword,cword);
//      cword=strtok(NULL,")");
//      rv=atoi(cword);
//   }
//   return(rv);
//}

void accept_word() {
   char ent[20];
   calc_cwscore();
   sprintf(ent,"%s(%d)",currword,currscore);
//   sprintf(ent,"%s (%d)\n",currword,currscore);
   strcat(mywords,ent);
   totscore=totscore+currscore;
   currscore=0;
   cwidx=0;
   currword[0]=0;
}

void display_scores() {
#ifdef USEGFX
   gfx_display_scores();
#else
   printf("\nTiles Left: %d",num_inpile);
   printf("\n\nCurrent Word: %d      Total Score: %d\n\n",currscore,totscore);
#endif
}

get_input(char *s, char *prompt) {
   char w;
   printf("%s",prompt);
   scanf("%2s",s);
   scanf("%c",&w);
}

get_input_str(char *s, char *prompt, int len) {
   char w;
   char pict[5];
   
   sprintf(pict,"%%%ds",len);
   printf("%s",prompt);
   scanf(pict,s);
   scanf("%c",&w);
}

char game_loop() {
   char st[12];
   int colno,wordvalid;

   while(1) {
      calc_cwscore();
      display_scores();
      display_curword();
      display_pile();
      if(cwidx>2)
	wordvalid=check_word(currword);
      else
	wordvalid=0;
      if(wordvalid==0) 
	get_input(st,"(word bad) action> ");
      else
	get_input(st,"(word OK) action> ");
      if(st[0]=='Q' || st[0]=='q') break;
      if(st[0]=='B' || st[0]=='b') {
	 rm_letter();
      }
      if(st[0]=='F' || st[0]=='f') {
	 find_words();
      }
      if(st[0]=='A' || st[0]=='a') {
	 if(wordvalid==1) {
	    accept_word();
	    if(num_inpile<=16) { 
	       if(find_words()==0) {
		  printf("No More Words!\n");
		  break;
	       }	       
	    }
//	    if(num_inpile<3) break;
	 } else 
	   printf("\nInvalid Word!\n");
      }
      
      colno=atoi(st);
      if(colno>0 && colno <13) {
	 pick_letter(colno);
//	 printf("ok\n");
      }
      
   } /* End While */
   return(st[0]);
}

ww_nprogress(char *text,int cmd, int num, int max) {
#ifdef USEGFX
   gfx_progressbar(text,cmd,num,max);
#else
   if(cmd>0)
     printf("%s\n",text);
   printf("Step %d/%d\n",num,max);
#endif
}

reg_hi_scores() {
   char st[40],nword[15],cdait[30];
   int wscore,wc=0;
#ifdef HAVE_LIBCURL
   char iwords[130];
   int hsid;
#endif
#ifdef USEGFX
   strcpy(st,"");
   gfx_hswindow(st);

#else
   printf("\n\n!!You have a high score!!\n\n");
   get_input_str(st,"Enter Your Name> ",30);
#endif
   hs_getdate(cdait);
   hs_insertscore(1,st,"",cdait,totscore);
   do {
      wscore=get_next_word(nword,wc);
//   printf("here s loop %d  wc:%d\n",wscore,wc);
      if(wscore!=0) {
#ifdef HAVE_LIBCURL
	 if(wc==0)
	   strcpy(iwords,nword);
	 else
	   sprintf(iwords,"%s|%s",iwords,nword);
//	 printf("iwords: %s\n",iwords);
#endif
	 hs_insertscore(2,st,nword,cdait,wscore);
	 wc++;
      }   
   } while(wscore!=0);
#ifdef HAVE_LIBCURL
   if(hsnmode == 'y') {
      // printf("submitting to internet\n");
      // printf("pile is %s\n",tilepilest);
      ww_nprogress("Submitting Score...",1,0,4);
      hsid=hsnet_getid();
      ww_nprogress("Submitting Score...",0,1,4);
      hsnet_addscore(hsid,st,"",totscore);
      ww_nprogress("Submitting Score...",0,2,4);
      hsnet_adddata(hsid,1,iwords);
      ww_nprogress("Submitting Score...",0,3,4);
      hsnet_adddata(hsid,1,tilepilest);
      ww_nprogress("Submitting Score...",0,4,4);
      hsnet_submit(hsid);
      ww_nprogress("Submitting Score...",-1,0,4);
   }
#endif
   // write table after submitting (this will ensure hs_numtbls is correct,
   // and prevents it from getting erased due to 'corruption'
   hs_writetable(hsfile);
#ifdef HAVE_LIBCURL
   // reload internet scores (to get your changes)
   if(hsnmode == 'y') {
      hsnet_fetchtables("wordworx");
   }
#endif
}

end_of_game(char lc) {
   int wc=0, wscore, hhs=0;
   char nword[20], hsca;
   // Do hi score here
#ifdef USEGFX
   hhs=gfx_endgame();
#else
   printf("No more words\n");
   printf("Your Words: (* denotes hi scoring word)\n----------\n");
   do {
      hsca=' ';
      wscore=get_next_word(nword,wc);
//   printf("here s loop %d  wc:%d\n",wscore,wc);
      if(wscore!=0) {
	 wc++;
	 if(hs_ishiscore(2,wscore)==1 || hs_ishiscore(4,wscore)==1) {
	    hhs++;
	    hsca='*';
	 }	 
	 printf("%2d) %-12s %3d%c\n",wc,nword,wscore,hsca);
      }   
   } while(wscore!=0);
//   printf("Done with s loop %d\n",wscore);
#endif
   if(hs_ishiscore(1,totscore)==1 || hs_ishiscore(3,totscore)==1) {
      hhs++;
   }
   if((lc=='Q' || lc=='q') && dqmode=='n')
     hhs=0;
   if (hhs>0) {
      reg_hi_scores();
   }
}

display_hs(int tno) {
#ifdef USEGFX
   gfx_display_hs(tno);
#else
   int nscs,i;
   char sname[30],sdetail[30],sdate[30],sscore[8];
   hs_names(tno,sname,sdetail);
   printf("\n%s\n--------------------------\n",sname);   
   nscs=hs_numscores(tno);
   for(i=1;i<=nscs;i++) {
      hs_getfield(sname,tno,i,"name");
      hs_getfield(sdate,tno,i,"date");
      hs_getfield(sscore,tno,i,"score");
      if(tno==2 || tno==4) {
	 hs_getfield(sdetail,tno,i,"detail");
	 printf("%2d) %-18s%-12s%-18s%4s\n",i,sname,sdetail,sdate,sscore);
      } else {
	 printf("%2d) %-20s %-20s %10s\n",i,sname,sdate,sscore);
      }
   }
#endif
}

char title_screen() {
   char rv,st[80];
   int tti;
#ifdef USEGFX
   rv=gfx_title(hs_numtbls);
#else
   printf("\nWordWorx v.%s\n(C)2007 Tony Smolar\n\n",VERSION); 
   rv='.';
   while(1) {
      get_input(st,"S)tart H)igh Scores Q)uit > ");
      if(st[0]=='Q' || st[0]=='q') { rv='Q'; break; }
      if(st[0]=='H' || st[0]=='h') { 
	 for(tti=1;tti<=hs_numtbls;tti++)
	   display_hs(tti);
      }
      if(st[0]=='S' || st[0]=='s') break;
   }
   
#endif
   return(rv);
}

print_version() {
   printf("WordWorx Version %s\n",VERSION);
   hs_version_info();
#ifdef USEGFX
   fnt_version_info();
   widget_version_info();
# ifdef USESDL
   sa_version_info();
# endif 
#endif
}

main(int argc, char **argv) {

   char le;
   int i;
#ifdef HAVE_LIBCURL
   char hstmp[210], hsset[210], hsurl[210], hstbl[96];
#endif
#ifdef WIDESCREEN
   WSmode=1;
#else
   WSmode=0;
#endif
   
   if(WSmode==1) {
      usex=800;
      rx0=80;
   }
   find_datadir(datadir,argv[0]);
   sprintf(hsfile,"%s%chs%c%s",datadir,mysep,mysep,"scores.dat");

   for(i=0;i<argc;i++) {
      if(strcmp("-f",argv[i])==0) fsmode='y';
      if(strcmp("-w",argv[i])==0) fsmode='n';
      if(strcmp("-n",argv[i])==0) hsnmode='n';
      if(strcmp("-q",argv[i])==0) dqmode='y';
      if(strcmp("-m",argv[i])==0) gmute='y';
      if(strcmp("-v",argv[i])==0) { print_version(); exit(0); }
      #ifdef USEGFX
      if(strcmp("-res",argv[i])==0) {
	 hss_index(restr,argv[i+1],0,'x');
	 usex=atoi(restr);
	 hss_index(restr,argv[i+1],1,'x');
	 usey=atoi(restr);
	 if(usex<minx || usey<miny) {
	    printf("Error: Minimum resolution is %dx%d\n",minx,miny);
	    exit(0);
	 }
	 rx0=(usex-minx)/2;
	 ry0=(usey-miny)/2;
      }
      if(strcmp("-W",argv[i])==0) {
	 // set widescreen 800x480
	 WSmode=1;
	 if(usex<800) usex=800;
	 if(usex<480) usey=480;
	 rx0=(usex-minx)/2;
      }
      if(strcmp("-nows",argv[i])==0) {
	 // set normal aspect 4:3
	 WSmode=0;
	 if(usex==800 && usey==480) {
	    usex=640;
	    usey=480;
	 }
	 rx0=(usex-minx)/2;
      }
#endif
   }

//   printf("load hs\n");
   hs_loadtable(hsfile);
//   printf("done load hs\n");
   
   if(hs_check(1,"High Score Table","WordWorx")!=1 || hs_check(2,"Highest Word Values","WordWorx")!=1 || hs_iscorrupt(2)==1) {
//      printf("High Score corruption detected! %d\n",hs_iscorrupt(2));
      hs_deleteall();
      hs_newtable("High Score Table","WordWorx",10);
      hs_newtable("Highest Word Values","WordWorx",15);
      hs_writetable(hsfile);
//   } else {
//      printf("High Score intact!\n");
   }
   
   numrows=7;
   
#ifdef USEGFX
   gfx_init();
   draw_bg();
//   printf("h1\n");
   widget_init();
//   printf("h2\n");
   gfx_setstyle();
//   printf("h3\n");
#endif
   
#ifdef HAVE_LIBCURL
//   strcpy(hsurl,"http://auckland1:28080/hs/scores.php");
   strcpy(hsurl,"http://yaki.sourceforge.net/hs/scores.php");
   strcpy(hstbl,"wwscore");
   sprintf(hsset,"%s%chs%c%s",datadir,mysep,mysep,"hsnet.txt");
# ifdef WIN32
   sprintf(hstmp,"%s%chs%c%s",datadir,mysep,mysep,"wwhssc.log");
# else
   sprintf(hstmp,"/tmp/%s","wwhssc.log");
# endif
//   printf("%s\n",hstmp);
//   printf("url %s\n",urlpfx);
   if(hsnmode == 'y') {
      ww_nprogress("Retrieving Internet Scores...",1,0,4);
      hsnet_setparms(hsurl,hstbl,hstmp);
      ww_nprogress("Retrieving Internet Scores...",0,1,4);
      hsnet_loadconf(hsset);
      ww_nprogress("Retrieving Internet Scores...",0,2,4);
      hsnet_chkdelayed();
      ww_nprogress("Retrieving Internet Scores...",0,3,4);
      hsnet_fetchtables("wordworx");
      ww_nprogress("Retrieving Internet Scores...",0,4,4);
      ww_nprogress("Submitting Score...",-1,0,4);
   }
#endif
   
   
   while(1) { // Master Loop
//      printf("master loop start\n");
      le=title_screen();
//      printf("done title\n");
      if(le=='Q' || le=='q') break;
      reset_game();
      
#ifdef USEGFX
#ifndef ZAURUS
      gfx_anim_pile();
#endif
      display_pile();
      gfx_display_scores();
      le=gfx_game_loop();
//      printf("le: %c\n",le);
#else
      le=game_loop();  
#endif
      if(WSmode==1) 
	gfx_unset_widescreen();
      end_of_game(le);
//      if(le=='Q' || le=='q') break;
   }
//   printf("done got here\n");
   //   printf("numin: %d\n",num_inbag);
}
