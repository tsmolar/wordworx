#include <stdio.h>
#include <string.h>
#include "wwtypes.h"

int wordidx[14],colidx[14];

backup_pile() {
   int x,y;
   for(y=1;y<=8;y++) {
     for(x=1;x<=12;x++) {
	bckpile[x][y]=ltrpile[x][y];
     }
   }
//   printf("NOTE!!!! BACKUP\n");
}

restore_pile() {
   int x,y;
   for(y=1;y<=8;y++) {
     for(x=1;x<=12;x++) {
	ltrpile[x][y]=bckpile[x][y];
     }
   }
}


char ai_pick_letter(int colno) {
   char rc;
   if(bckpile[colno][colidx[colno]]!=' ' && colidx[colno]<8) {
      rc=bckpile[colno][colidx[colno]];
      colidx[colno]++;
   } else {
      rc='_';
      colidx[colno]=9;
   }
   return(rc);
}

reset_widx() {
   int i;
   for(i=0;i<=12;i++)
     wordidx[i]=1;
}

reset_colidx() {
   int i;
   for(i=0;i<=12;i++) {
      if(bckpile[i][1]==' ')
	colidx[i]=9;
      else
	colidx[i]=1;
   }
   
}

int incr_widx(int len) {
   int rv=0;
   
   do {
      wordidx[len]++;
   } while(colidx[wordidx[len]]==9);
    
   if(wordidx[len]>12) {
      if(len>1) {
	 wordidx[len]=1;
	 rv=incr_widx(len-1);
      } else { 
	 rv=1;
      }
   }
   return(rv);
}

print_widx(int len) {
   int i;
   for(i=1;i<=len;i++)
     printf("%d ",wordidx[i]);
   printf("\n");
}

int find_words() {
   // AI Entry point,  determine if valid words can still be made from 
   // the pile
   char tword[14],ll;
   int rv=0,i,nm=0,wl,ml,vw, nw;
   if(num_inpile<6) 
     ml=num_inpile;
   else
     ml=6;
   for(wl=3;wl<=ml;wl++) {
//   for(wl=3;wl<=6;wl++) {
      nw=0;
      backup_pile();
      reset_widx();
      reset_colidx();
      do {
//	       print_widx(wl);
	 for(i=0;i<wl;i++) {
	    ll=ai_pick_letter(wordidx[i+1]);
	    if(ll=='_') break;
	    tword[i]=ll;
	 }
	 
	 tword[wl]=0;
	 reset_colidx();
	 if(ll!='_') {
	    nw++;
	    vw=check_word(tword);
	    if(vw==1) {
	       rv=1;
//	       printf("test word: %s\n",tword);
	       break;
	    }
	 }
	 nm=incr_widx(wl);
      } while (nm==0);
//      printf("done w/ %d letter words num checked: %d\n",wl,nw);
      if(rv==1) break;
   }
//   if(rv==0) printf("NO MORE WORDS!\n");
   return(rv);
}

int find_words_chunk(int wl, int chunksz) {
   // Alternate AI entry point, this allows word searching in chunks
   // to allow word checking concurrently with user input
   // set chunksz and wl to 0 to initialize a new search!
   // set chunksz to 0 and wl>3 to search a new letter length
   // 
   // Return Values:
   // -1 : Search incomplete
   //  0 : No words found
   //  1 : Word Found
   //  2 : Word length problem, word too long, short or exceed num of tiles
   // 
   // 
   //   int wl;
   char tword[14],ll;
   int rv=0,nm=0,nw,i,vw,ckwd=0;

   if((wl>num_inpile) || wl>12 ||wl<3)
     return(2);
   if(chunksz==0) {
      if(wl<4) {
	 nw=0;
	 backup_pile();
      }
      reset_widx();
      reset_colidx();
      return(-1);
   }
   
   do {
      //	       print_widx(wl);
      for(i=0;i<wl;i++) {
	 ll=ai_pick_letter(wordidx[i+1]);
//	 printf("picked letter %c\n",ll);
	 if(ll=='_') break;
	 tword[i]=ll;
      }
      tword[wl]=0;
      reset_colidx();
      if(ll!='_') {
	 nw++;
	 vw=check_word(tword);

	 if(vw==1) {
	    rv=1;
	    strcpy(vword,tword);
//	    printf("FOUND:%s\n",tword);
	    //	       printf("test word: %s\n",tword);
	    break;
	 }
	 ckwd++; // checked words
	 if(ckwd>=chunksz) {  // This might need to be done after incr_widx
	    rv=-1;            // Check for chunk
	    break;
	 }	 
      }
      nm=incr_widx(wl);
   } while (nm==0);
   return(rv);
   }
