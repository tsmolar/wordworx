#include <stdio.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "wwtypes.h"
#include "window.h"
#include "font.h"
#include <button.h>

#ifdef USEALLEGRO
#include <allegro.h>
#endif
#ifdef USESDL
#include <SDL.h>
#include "sdl_allegro.h"
#endif
#if defined(HAVE_LIBSDL_IMAGE) || defined(USEALLEGRO)
#define GFXBITMAP
#endif
#if defined(HAVE_LIBSDL_MIXER) || defined(USEALLEGRO)
#define USESOUND
#endif

/* 
#ifdef ZAURUS
#define TILESTEP 4
#define TILESTEP2 4
#else
#define TILESTEP 1
#define TILESTEP2 2 
#endif
*/

#if defined(ZAURUS)
#define TILESTEP 4
#define TILESTEP2 4
#endif

#if defined(ASUSEEE)
#define TILESTEP 4
#define TILESTEP2 4
#endif

#if !defined(ZAURUS) && !defined(ASUSEEE)
#define TILESTEP 1
#define TILESTEP2 2 
#endif

#define TILEPILE_X 8
#define TILEPILE_Y 80
#define CURRWORD_X 8
#define CURRWORD_Y 20

// Globals
BITMAP *bgbmp, *ywbmp;
#ifdef GFXBITMAP
BITMAP *tilesbmp, *widgetbmp, *banrbmp, *wibmp, *gearbmp, *wlbmp;
int gearturn=0, gheight=354;
#endif
#ifdef USESOUND
SAMPLE *bubble_smp, *drop_smp, *music_smp, *chaching_smp, *non_smp;
SAMPLE *crash_smp, *excl1_smp, *excl2_smp, *excl3_smp, *excl4_smp; 
#endif
int currcmd;
style_t mystyle;
// Global Flags
int flg_morewords=-1, flg_numlet=3,flg_wc=0; // -1 unknown,  0=no, 1=yes

// effects
void gfx_shade_rect(BITMAP *bmp,int x1, int y1, int x2, int y2) {
   int x,y,c;
   
   c=makecol(128,128,128);
   for(y=y1;y<=y2;y++)
     for(x=x1;x<=x2;x++) {
	if(x%2==y%2)
	  putpixel(screen,x,y,c);
     }
}

void gfx_darken_rect(BITMAP *bmp,int x1, int y1, int x2, int y2) {
   int x,y,c,rr,gg,bb;
   
#ifdef USESDL
   //   if(SDL_MUSTLOCK(screen)){
   if(SDL_LockSurface(screen) < 0) return;
   //      printf("locking screen\n");
   //   } else 
   //      printf("no lockie\n");
#endif
   
   for(y=y1;y<=y2;y++) {
      for(x=x1;x<=x2;x++) {
	//	if(x%2==y%2)
	 c=getpixel(screen,x,y);
	 rr=getr(c)/2;
	 gg=getg(c)/2;
	 bb=getb(c)/2;
	 putpixel(screen,x,y,makecol(rr,gg,bb));
      }
   }
#ifdef USESDL
   SDL_UnlockSurface(screen);
   SDL_UpdateRect(screen,x1,y1,(x2-x1),(y2-y1));
#endif
}

// Animations

gfx_liftcol(int colno, int step) {
   int x,y,cl,cpl;
   cl=num_letincol(colno);
   cpl=cl*56;
   x=TILEPILE_X+((colno-1)*48);
   scare_once();
   for(y=56;y>=step;y=y-step) {
//      blit(screen,screen,x,TILEPILE_Y+step,48,392,x,TILEPILE_Y);
//      printf("blit ss:  %d,%d  %d,%d  %d,%d\n",x,TILEPILE_Y+y+step,x,TILEPILE_Y+y,48,cpl);
      blit(screen,screen,x+rx0,TILEPILE_Y+step+y+ry0,x+rx0,TILEPILE_Y+y+ry0,48,cpl);
//      printf("blit bs:  %d,%d  %d,%d  %d,%d\n",x,TILEPILE_Y+cpl+y-(step),x,TILEPILE_Y+cpl+y-(step),48,step);
      blit(bgbmp,screen,x,TILEPILE_Y+cpl+y-(step),x+rx0,TILEPILE_Y+cpl+y-(step)+ry0,48,step);
#ifdef GFXBITMAP
      gearturn++;
      if(gearturn>29) gearturn=0;
      blit(gearbmp,screen,gearturn*56,0,584+rx0,gheight+ry0,55,90);
#endif
      ww_sleep(5);
   }
   unscare_once();
   
}
gfx_dropcol(int colno, int step) {
   int x,y,cl,cpl;
   cl=num_letincol(colno);
   cpl=(cl-1)*56;
   x=TILEPILE_X+((colno-1)*48);
//   printf("ltrpile[colno][1]='%c'\n",ltrpile[colno][1]);
   scare_once();
   if(ltrpile[colno][2]!=' ')
     gfx_shade_rect(screen,x+rx0,TILEPILE_Y+ry0,x+47+rx0,TILEPILE_Y+55+ry0);
   for(y=0;y<=(56-step);y=y+step) {
//      blit(screen,screen,x,TILEPILE_Y+step,48,392,x,TILEPILE_Y);
      blit(screen,screen,x+rx0,TILEPILE_Y+y+ry0,x+rx0,TILEPILE_Y+y+step+ry0,48,cpl);
      blit(bgbmp,screen,x,TILEPILE_Y+y,x+rx0,TILEPILE_Y+y+ry0,48,step);
#ifdef GFXBITMAP
      gearturn--;
      if(gearturn<0) gearturn=29;
      blit(gearbmp,screen,gearturn*56,0,584+rx0,gheight+ry0,55,90);
#endif
      ww_sleep(5);
   }
   unscare_once();
}

gfx_movetile(int sx, int sy, int dx, int dy, int step) {

   // Something wrong here, slow down?
   
   BITMAP *tile_sp, *tile_bg;
   int cy,lx,ly;
//   float xfac,cx,xa,ya;   
   int xfac,cx,cfx,xa,ya;

//   printf("sx:%d  sy:%d  dx:%d  dy:%d\n",sx,sy,dx,dy);
   tile_sp=create_bitmap(48,56);
   tile_bg=create_bitmap(48,56);
   
   scare_once();
   blit(screen,tile_sp,sx+rx0,sy+ry0,0,0,48,56);
//   blit(tile_sp,screen,0,0,0,0,48,56);
//   ww_sleep(2000);
   blit(bgbmp,screen,sx,sy,sx+rx0,sy+ry0,48,56);
   blit(screen,tile_bg,sx+rx0,sy+ry0,0,0,48,56);
   blit(tile_sp,screen,0,0,sx+rx0,sy+ry0,48,56);
   lx=sx;ly=sy;
   if(sy>dy) {
//      printf("sx:%d  sy:%d  dx:%d  dy:%d\n",sx,sy,dx,dy);
      xa=(dx-sx)*10;ya=(sy-dy)/step;
      xfac=xa/ya;
      cfx=sx*10;
//      printf("sx:%d  sy:%d  dx:%d  dy:%d\n",sx,sy,dx,dy);
//      printf("x factor is %d  step:%d  (%d-%d)/(%d-%d)\n",xfac,step,dx,sx,sy,dy);
      for(cy=sy;cy>=dy;cy=cy-step) {
	 cx=cfx/10;
	 blit(tile_bg,screen,0,0,lx+rx0,ly+ry0,48,56);
	 blit(screen,tile_bg,cx+rx0,cy+ry0,0,0,48,56);
	 //	  printf("IAM ER %d,%d\n",sx,cy);
	 blit(tile_sp,screen,0,0,cx+rx0,cy+ry0,48,56);
//	 rect(screen,cx,cy,cx+48,cy+56,makecol(0,0,0));
//	 printf("at %d,%d   %d=>%d  step:%d\n",cx,cy,sy,dy,step);
//   SDL_Flip(screen);
	 ly=cy; lx=cx;
	 cfx=cfx+xfac;
	 ww_sleep(5);
      }
   } else {
      xa=(dx-sx)*10;ya=(dy-sy)/step;
      xfac=xa/ya;
      cfx=sx*10;
//      printf("x factor is %d   (%d-%d)/(%d-%d)\n",xfac,dx,sx,sy,dy);
      for(cy=sy;cy<=dy;cy=cy+step) {
	 cx=cfx/10;
	 blit(tile_bg,screen,0,0,lx+rx0,ly+ry0,48,56);
	 blit(screen,tile_bg,cx+rx0,cy+ry0,0,0,48,56);
	 //	  printf("IAM ER %d,%d\n",sx,cy);
	 blit(tile_sp,screen,0,0,cx+rx0,cy+ry0,48,56);
	 ly=cy; lx=cx;
	 cfx=cfx+xfac;
	 ww_sleep(5);
      }
   }
   unscare_once();
   
   destroy_bitmap(tile_sp);
   destroy_bitmap(tile_bg);
}

int wcb_sound(Widget *w, int x, int y, int m) {
   if(gmute=='y')
     gmute='n';
   else
     gmute='y';
}

int wcb_ab(Widget *w, int x, int y, int m) {
   // Alert Button Callback
   alert_button=3;
   if(w->ksym==KEY_Y || w->ksym==KEY_ENTER)
     alert_button=1;
   if(w->ksym==KEY_N)
     alert_button=2;
   close_window();
}

gfx_windecor(int x, int y, int width, int height,int base) {
   int bcr,bcg,bcb,wh,wl,ir,ig,ib;
   bcr=getr(base); bcg=getg(base); bcb=getg(base); 
   wl=makecol(bcr/4,bcg/4,bcb/4);
   ir=bcr+80; if(ir>255) ir=255;
   ig=bcg+80; if(ig>255) ig=255;
   ib=bcb+80; if(ib>255) ib=255;
   wh=makecol(ir,ig,ib);
//   rectfill(screen,x,y,x+width,y+height,base); // Paint bg
   rectfill(screen,x+4,y+4,x+width-4,y+14,makecol(32,32,120));
   hline(screen,x,y,x+width,wh);
   vline(screen,x,y,y+height,wh);
   hline(screen,x,y+height,x+width,wl);
   vline(screen,x+width,y,y+height,wl);
   // End Decor
}

int gfx_alert(char *text,int k1, char *b1,int k2, char *b2,int k3,char *b3) {
   // Taken from widget lib,  to be more customizable
   
   Widget* nw;
   int width,height,x,y,mbw,bw,toff,tyoff;
#ifdef GFXBITMAP
   bmpbtn_t mybutton;
#endif
   mbw=40;
   alert_button=0;
   
   if(b1) {
      bw=calc_width(b1);
      if(bw > mbw) mbw=bw;
   }
   if(b2) {
      bw=calc_width(b2);
      if(bw > mbw) mbw=bw;
   }
   if(b3) {
      bw=calc_width(b3);
      if(bw > mbw) mbw=bw;
   }
   
   mbw+=2;
   width=calc_width(text);
   height=calc_height(text);
#ifdef GFXBITMAP
   width+=48;
   toff=40;
   tyoff=4;
   height+=40;
#else
   width+=16;
   toff=0;
   tyoff=0;
   height+=32;
#endif
   if (width < 3*mbw+6) width=3*mbw+6;
   height+=32; // For Banner
   x=(VIRTUAL_W-width)/2;
   y=(VIRTUAL_H-height)/2;
   new_window(x,y,x+width,y+height);
#ifdef GFXBITMAP
   gfx_windecor(x,y,width,height,makecol(182,170,170));
//   rectfill(screen,x,y,x+width,y+height,makecol(182,170,170));
#else
   gfx_windecor(x,y,width,height,makecol(128,128,128));
#endif
   if(b1) {
#ifdef GFXBITMAP
      mybutton.btnupbmp=widgetbmp;
      mybutton.btndnbmp=widgetbmp;
      mybutton.btnhlbmp=NULL;
      if(strcmp(b1,"OK")==0) {
	 mybutton.up_x1=0;mybutton.up_y1=139;mybutton.up_x2=41;mybutton.up_y2=19;
	 mybutton.dn_x1=41;mybutton.dn_y1=139;mybutton.dn_x2=41;mybutton.dn_y2=19;
      } else {
	 mybutton.up_x1=0;mybutton.up_y1=120;mybutton.up_x2=41;mybutton.up_y2=19;
	 mybutton.dn_x1=41;mybutton.dn_y1=120;mybutton.dn_x2=41;mybutton.dn_y2=19;
      }
      nw=add_bmp_button(x+8,y+height-21,x+49,y+height-2,mybutton,&wcb_ab);
#else
      nw=add_button(x+8,y+height-20,x+mbw,y+height-4,b1,&wcb_ab);
#endif
//      printf("b1:%d,%d %d,%d\n",x+8,y+height-20,x+mbw,y+height-4);
      wdg_bind_key(nw,k1,-1,0);
   }
   if(b2) {
#ifdef GFXBITMAP
      mybutton.up_x1=82;mybutton.up_y1=120;mybutton.up_x2=41;mybutton.up_y2=19;
      mybutton.dn_x1=123;mybutton.dn_y1=120;mybutton.dn_x2=41;mybutton.dn_y2=19;
      nw=add_bmp_button(x+51,y+height-21,x+51+41,y+height-2,mybutton,&wcb_ab);
#else
      nw=add_button(x+mbw+2,y+height-20,x+2*mbw+2,y+height-4,b2,&wcb_ab);
#endif
//      printf("b2:%d,%d %d,%d\n",x+mbw+2,y+height-20,x+2*mbw+2,y+height-4);
      wdg_bind_key(nw,k2,-1,0);
   }
   if(b3) {
     nw=add_button(x+2*mbw+4,y+height-20,x+3*mbw+4,y+height-4,b3,&wcb_ab);
     wdg_bind_key(nw,k3,-1,0);
   }
   fnt_print_string(screen,x+8+toff,y+24+tyoff,text,makecol(0,0,0),-1,-1);
#ifdef GFXBITMAP
   masked_blit(widgetbmp,screen,64,0,x+8,y+17+tyoff,32,32);
#endif
//   printf("b4 gfx alert\n");
   while(alert_button==0) {
      rest(0);
      event_loop(JUST_ONCE);
      kbd_loop(JUST_ONCE);
   }
//   printf("f24 gfx alert\n");
   return alert_button;
}

int gfx_progressbar(char *text, int cmd, int num, int max) {
   if(cmd>-1) 
     progress(text,cmd,num,max);
   else
     close_window();
//#ifdef GFXBITMAP
//      gfx_windecor(x,y,width,height,makecol(182,170,170));
//#else
//      gfx_windecor(x,y,width,height,makecol(128,128,128));
//#endif
}

wcb_hswindowcb(Widget* b,int x,int y, int m) {
   // Extend unfocus callback
//   printf("I've been called!\n");
   unfocus_input(b,x,y,m);
//   alert_button=1;
   close_window();
}


void gfx_hswindow(char *username) {
   Widget* b;
   
   int width=320,height=128,x,y,bc;
   alert_button=0;
   x=(VIRTUAL_W-width)/2;
   y=(VIRTUAL_H-height)/2;
   bc=makecol(128,64,160);
   activestyle.window.bg=makecol(128,64,160);
   activestyle.window.sh=makecol(64,32,80);
   // Create Window (Why were there two create windows?)
   new_window(x,y,x+width,y+height);
//   new_window(x,y,x+width,y+height);
   gfx_windecor(x,y,width,height,bc);
   wdgst_apply(mystyle); // Replace org style
   
   fnt_print_string(screen,x+96,y+20,"Congratulations!",makecol(240,240,240),-1,makecol(64,64,64));
   fnt_print_string(screen,x+56,y+36,"You have a new high score.",makecol(208,208,208),-1,-1);
   fnt_print_string(screen,x+72,y+68,"Please Enter your name.",makecol(208,208,208),-1,-1);
   
   b=add_input(x+36,y+91,30,username);
   focus_input(b,1,-1,-1); // Probably should use a better way, maybe
                              // a generic focus routine?
//   printf("1 focused: %d\n",b);
   b->handler=&wcb_hswindowcb;
//   strcpy(b->text,"");
//   printf("2 focused: %d\n",b);
//   alert_button=0;
//   add_input(x+108,y+88,12,"XX");
//   b=add_button(x+width-30,y+height-20,x+width-2,y+height-2,"OK",&wcb_ab);
//   wdg_bind_key(b,KEY_ENTER,-1,0);
   
//   while(alert_button==0) {
//      event_loop(JUST_ONCE);
      kbd_loop(HALT_ON_POP);
//      if(has_focus(b)==0)
//	break;
//   }
//   printf("ok name retriving\n");
//   strcpy(username,b->text); // Callback wouldn't work b/c it destroys   
//   printf("ok name retrived\n");
//   close_window();           // the text b4 I could grab it.
//   printf("closed\n");
}

void gfx_validicon(int wv) {
   
   #ifdef GFXBITMAP
   blit(bgbmp,screen,CURRWORD_X+594,CURRWORD_Y+12,CURRWORD_X+594+rx0,CURRWORD_Y+12+ry0,32,32);
   switch(wv) {
    case 0: masked_blit(widgetbmp,screen,32,0,CURRWORD_X+594+rx0,CURRWORD_Y+12+ry0,32,32);
      break;
    case 1: masked_blit(widgetbmp,screen,0,0,CURRWORD_X+594+rx0,CURRWORD_Y+12+ry0,32,32);
      break;
    case 2: masked_blit(widgetbmp,screen,64,0,CURRWORD_X+594+rx0,CURRWORD_Y+12+ry0,32,32);
      break;
   }
   #else
   int col;
   
   switch(wv) {
    case 0: col=makecol(224,64,0);
      break;
    case 1: col=makecol(64,224,32);
      break;
    case 2: col=makecol(240,224,32);
      break;
    case 3: col=makecol(240,224,32);
      break;
   }
   rectfill(screen,CURRWORD_X+590+rx0,CURRWORD_Y+12+ry0,CURRWORD_X+621+rx0,CURRWORD_Y+43+ry0,col);
   rect(screen,CURRWORD_X+590+rx0,CURRWORD_Y+12+ry0,CURRWORD_X+621+rx0,CURRWORD_Y+43+ry0,makecol(0,0,0));
   #endif
}

int gfx_word_check() {
   int wordvalid;
   
   calc_cwscore();
   display_scores();
   if(cwidx==0)
     wordvalid=3;
   if(cwidx>0 && cwidx<3)
   wordvalid=2;
//   printf("b4 check:  %s, %d:%d\n",currword,cwidx,strlen(currword));
   if(cwidx>2)
     wordvalid=check_word(currword);
//   printf("af check:  %s\n",currword);
   gfx_validicon(wordvalid);
//   printf("af val:  %s\n",currword);
}

// Widget Callbacks
int wcb_hint(Widget *w, int x, int y, int m) {
   int ab;
   char amsg[50];
   if(strcmp(vword,"X")==0) 
     sprintf(amsg,"Still searching words!");
   else
     sprintf(amsg,"Your hint: %s",vword);
   ab=gfx_alert(amsg,KEY_ENTER,"OK",-1,NULL,-1,NULL);
}

int wcb_clearhs(Widget *w, int x, int y, int m) {
   int ab;
   ab=gfx_alert("Are You Sure you want to erase high scores?",KEY_Y,"Yes",KEY_N,"No",-1,NULL);
   if(ab==1) {
      hs_cleartable(1);
      hs_cleartable(2);
      hs_writetable(hsfile);
   }
}

int wcb_accept_word(Widget *w, int x, int y, int m) {
   char wcn[6];
   if(cwidx>2 && check_word(currword)==1) {
#ifdef USESOUND
      if (currscore<16 && gmute=='n')
	play_sample(excl1_smp,96,0,1000,0);
      if (currscore>=16 && currscore<60 && gmute=='n')
	play_sample(chaching_smp,96,0,1000,0);
      if (currscore>=60 && currscore<90 && gmute=='n')
	play_sample(excl2_smp,96,0,1000,0);
      if (currscore>=90 && currscore<120 && gmute=='n')
	play_sample(excl3_smp,96,0,1000,0);
      if (currscore>=120 && gmute=='n')
	play_sample(excl4_smp,96,0,1000,0);
#endif
      if(WSmode==1) {
	 wordcount++;
	 sprintf(wcn,"%3d",currscore);
	 if(wordcount<28) {
//	    fnt_print_string(screen,24+(rx0-160),24+ry0+(16*wordcount),wcn,makecol(240,230,200),-1,-1);
//	    fnt_print_string(screen,52+(rx0-160),24+ry0+(16*wordcount),currword,makecol(240,230,200),-1,-1);
//	    fnt_print_string(screen,52+(rx0-160),24+ry0+(16*wordcount),"ABCDEFGHIJKL",makecol(240,230,200),-1,-1);
	    fnt_print_string(screen,22+(rx0-160),24+ry0+(16*wordcount),currword,makecol(240,230,200),-1,-1);
	    fnt_print_string(screen,130+(rx0-160),24+ry0+(16*wordcount),wcn,makecol(250,240,120),-1,-1);
	 }
      }
      accept_word();
      scare_once();
      blit(bgbmp,screen,CURRWORD_X,CURRWORD_Y,CURRWORD_X+rx0,CURRWORD_Y+ry0,576,56);
      display_scores();
      gfx_validicon(3);
      unscare_once();
      flg_morewords=-1;
      flg_numlet=3;
      flg_wc=0;
      strcpy(vword,"X");
   } else {
#ifdef USESOUND
      if(gmute=='n')
	play_sample(non_smp,96,0,1000,0);
#endif
//      printf("I Can't accept This!\n");
   }
}

int wcb_clear_word(Widget *w, int x, int y, int m) {
   int colno;
   while (cwidx >0) {
      rm_letter();
      colno=cwreturn[cwidx];
      gfx_dropcol(colno,TILESTEP2);
      gfx_movetile(CURRWORD_X+(cwidx*48),CURRWORD_Y,TILEPILE_X+((colno-1)*48),TILEPILE_Y,TILESTEP2);
   } 
   display_curword();
   display_pile();
   gfx_validicon(3);
   currscore=0;
   cwidx=0;  // precaution
   currword[0]=0; // may fix a core dump
   display_scores();
   if(wdg_getlevel(W_HILIGHTED!=NULL)) {
      wdg_hilight(1);
      wdg_unhilight();
   }
}

int wcb_click_word(Widget *w, int x, int y, int m) {
   int colno=cwreturn[cwidx-1];
//   printf("clicked letter:  %d   %d\n",colno,cwidx);
   if(cwidx>0) {
      rm_letter();
#ifdef USESOUND
      if(gmute=='n')
	play_sample(drop_smp,96,0,1000,0);
#endif
      gfx_dropcol(colno,1);
      gfx_movetile(CURRWORD_X+(cwidx*48),CURRWORD_Y,TILEPILE_X+((colno-1)*48),TILEPILE_Y,TILESTEP);
      display_curword();
      display_pile();
      gfx_word_check();
//      if(w==wdg_getlevel(W_HILIGHTED)) {
      if(wdg_getlevel(W_HILIGHTED!=NULL)) {
	 wdg_hilight(1);
	 wdg_unhilight();
      }
   }
//   printf("done with callback %d %d %d \n",x,y,m);
   return(0);
}

int wcb_click_letter(Widget *w, int x, int y, int m) {
   int ltno=((x-(TILEPILE_X+rx0))/48)+1,gl;
//   printf("clicked letter:  %d\n",ltno);
   gl=pick_letter(ltno);
   //   gfx_movetile(CURRWORD_X+(cwidx*12),CURRWORD_Y,TILEPILE_X+((colno-1)*48),TILEPILE_Y);
   if(gl==0) {
#ifdef USESOUND
      if(gmute=='n')
	play_sample(bubble_smp,96,0,1000,0);
#endif
      gfx_movetile(TILEPILE_X+((ltno-1)*48),TILEPILE_Y,CURRWORD_X+((cwidx-1)*48),CURRWORD_Y,TILESTEP);
      display_curword();
      gfx_liftcol(ltno,TILESTEP);
//      gfx_liftcol(ltno,1);
      display_pile();
      gfx_word_check();
      if(w==wdg_getlevel(W_HILIGHTED)) {
	 wdg_hilight(1);
	 wdg_unhilight();
      }
   }
}

int wcb_exit(Widget *w, int x, int y, int m) {
   int ab;
   ab=gfx_alert("Are You Sure you want to Exit WordWorx?",KEY_Y,"Yes",KEY_N,"No",-1,NULL);
   if(ab==1)
     currcmd=KEY_Q;
}

int wcb_quit(Widget *w, int x, int y, int m) {
   int ab;
//   printf("b4 alert \n");
   ab=gfx_alert("Are You Sure you want to Abandon this Game?",KEY_Y,"Yes",KEY_N,"No",-1,NULL);
//   printf("f2r alert\n");
   
   if(ab==1)
     currcmd=KEY_Q;
}

int wcb_start(Widget *w, int x, int y, int m) {
   currcmd=KEY_S;
}
// End Callbacks
gfx_display_scores() {
   char cwscore_s[30],totscore_s[30];
#ifdef USESDL
   SA_AUTOUPDATE=1;
#endif
   blit(bgbmp,screen,0,0,0+rx0,0+ry0,639,19);
#ifdef GFXBITMAP
   sprintf(cwscore_s,"              %3d",currscore);
   sprintf(totscore_s,"             %4d",totscore);
#else
   sprintf(cwscore_s,"Current Word: %3d",currscore);
   sprintf(totscore_s,"Total Score: %4d",totscore);
   gfx_shade_rect(screen,(1*8)+rx0,0+ry0,(21*8)+rx0,17+ry0); // May be superflous
   gfx_shade_rect(screen,440+rx0,0+ry0,584+rx0,17+ry0); // May be superflous
#endif
   fnt_print_string(screen,CURRWORD_X+5+rx0,1+ry0,cwscore_s,makecol(255,255,224),-1,makecol(0,0,0));
   fnt_print_string(screen,CURRWORD_X+436+rx0,1+ry0,totscore_s,makecol(255,255,224),-1,makecol(0,0,0));
}

draw_bg() {
   #ifdef GFXBITMAP
   scare_once();
   blit(bgbmp,screen,0,0,rx0,ry0,639,479);
   blit(gearbmp,screen,gearturn*56,0,584+rx0,gheight+ry0,55,90);
   unscare_once();
   #else
   rectfill(screen,rx0,ry0,639+rx0,479+ry0,makecol(176,0,192));
   rectfill(screen,CURRWORD_X+rx0,CURRWORD_Y+ry0,CURRWORD_X+(12*48)+rx0,CURRWORD_Y+56+ry0,makecol(128,0,148));
   
   // Copy background to save under tiles
   blit(screen,bgbmp,rx0,ry0,0,0,639,479);
   #endif
   gfx_display_scores();
}

gfx_drawtile(BITMAP *mbmp, int x, int y, char l, int shade) {
#ifdef GFXBITMAP
   int ic=l-65,sx,sy;
   sy=(ic/7)*56;
   sx=(ic%7)*48;
   blit(tilesbmp,mbmp,sx,sy,x,y,48,56);
#else
   char ls[4];
   int ils;
   if(l != ' ') {
      rectfill(mbmp,x,y,x+47,y+55,makecol(224,216,192));
      fnt_display_char(mbmp,x+20,y+20,l,makecol(64,64,32));
      ils=get_letter_score(l);
      sprintf(ls,"%d",ils);
      fnt_print_string(mbmp, x+32, y+40, ls, makecol(32,32,32), -1, -1);
      rect(mbmp,x,y,x+47,y+55,makecol(16,16,16));
      rect(mbmp,x,y,x+46,y,makecol(250,250,240));
      rect(mbmp,x,y,x,y+54,makecol(250,250,240));
   }
#endif
   if(shade==1 && l != ' ')
     gfx_shade_rect(mbmp,x,y,x+47,y+55);
}

void gfx_display_pile() {
   int x,y,c;
   
   scare_once();
#ifdef USESDL
   SA_AUTOUPDATE=0;
#endif
//   set_font_fcolor(64,64,0);
   for(y=1;y<=numrows;y++)
     for(x=1;x<=12;x++) {
	if(y>1)
	  gfx_drawtile(screen,TILEPILE_X+((x-1)*48)+rx0,TILEPILE_Y+((y-1)*56)+ry0,ltrpile[x][y],1);
	else
	  gfx_drawtile(screen,TILEPILE_X+((x-1)*48)+rx0,TILEPILE_Y+((y-1)*56)+ry0,ltrpile[x][y],0);
     }

#ifdef USESDL
   SA_AUTOUPDATE=1;
   SDL_UpdateRect(screen,TILEPILE_X+rx0,TILEPILE_Y+ry0,576,392);
#endif
   if(TILESTEP2==2)
     blit(bgbmp,screen,0,TILEPILE_Y-1,rx0,TILEPILE_Y+ry0-1,640,1); // fix visual bug?
   unscare_once();
}

void gfx_display_curword() {
   int i;
   for(i=0;i<12;i++) {
      if(currword[i]==0) break;
      gfx_drawtile(screen,CURRWORD_X+(i*48)+rx0,CURRWORD_Y+ry0,currword[i],0);
   }
}

gfx_anim_pile() {
   // Completely superflous animation
   BITMAP *bmp, *bmp2;
   int x,y,y2;
   bmp=create_bitmap(576,392);
   bmp2=create_bitmap(576,56);
   for(y=1;y<=numrows;y++)
     for(x=1;x<=12;x++)
       gfx_drawtile(bmp,(x-1)*48,(y-1)*56,ltrpile[x][y],0);

#ifndef USESDL
   // One Chunk
  for(y=-371;y<=TILEPILE_Y;y=y+TILESTEP) {
      if(y>20) {
	 blit(bmp,screen,0,0,TILEPILE_X+rx0,y+ry0,576,392);
	 blit(bgbmp,screen,TILEPILE_X,y-2,TILEPILE_X+rx0,y+ry0-2,576,2);
      }
      else {
	 x=y+372;
	 blit(bmp,screen,0,392-x,TILEPILE_X+rx0,CURRWORD_Y+ry0,576,x);
      }
   }
#else
   // 7 Chunks
   for(x=6;x>=0;x--) {
      for(y=-371;y<=TILEPILE_Y;y=y+TILESTEP2) {
	 if(y>(20-(x*56))) {
	    blit(screen,bmp2,TILEPILE_X+rx0,y+(x*56)+ry0,0,0,576,56);
	    blit(bmp,screen,0,(x*56),TILEPILE_X+rx0,y+(x*56)+ry0,576,56);
//	    blit(bgbmp,screen,TILEPILE_X,y+(x*56)-2,TILEPILE_X,y+(x*56)-2,576,2);
	    ww_sleep(1);
	    if(y<TILEPILE_Y-1) {
	       SA_AUTOUPDATE=0;
	       blit(bmp2,screen,0,0,TILEPILE_X+rx0,y+(x*56)+ry0,576,56);
	       SDL_UpdateRect(screen,TILEPILE_X+rx0,y+(x*56)+ry0,576,TILESTEP2);
//	       SDL_UpdateRect(screen,TILEPILE_X+rx0,y+(x*56)+ry0,576,1);
	       SA_AUTOUPDATE=1;
	    }
	 } else {
	    y2=y+((x*56)+36);
//	    printf("here:%d\n",y2);
	    if(y2>0 && y2<56) {
//	       blit(screen,bmp2,TILEPILE_X,CURRWORD_Y,0,0,576,56);
	       blit(bmp,screen,0,((x*56)+56)-y2,TILEPILE_X+rx0,CURRWORD_Y+ry0,576,y2);
	       ww_sleep(1);
	       if(y2>52)
		 blit(bgbmp,screen,TILEPILE_X,CURRWORD_Y,TILEPILE_X+rx0,CURRWORD_Y+ry0,576,56);
	    }
	 }
      }
      if(TILESTEP2==4)
	blit(bmp,screen,0,(x*56),TILEPILE_X+rx0,TILEPILE_Y+(x*56)+ry0,576,56);
   }
#endif
   destroy_bitmap(bmp);
   destroy_bitmap(bmp2);
}

gfx_setup_buttons() {
   Widget *mb;
   int x;
#ifdef GFXBITMAP
   bmpbtn_t mybutton;
#endif
   
//   widget_init();
//   printf("about to fail\n");
   widget_clear_level();
   wdg_install_hilight(makecol(255,255,128),0,0,0,0,0,0);
//printf("failed\n");
#ifdef GFXBITMAP
   mybutton.btnupbmp=widgetbmp;
   mybutton.btndnbmp=widgetbmp;
   mybutton.btnhlbmp=NULL;
   mybutton.up_x1=0;mybutton.up_y1=32;mybutton.up_x2=41;mybutton.up_y2=19;
   mybutton.dn_x1=41;mybutton.dn_y1=32;mybutton.dn_x2=41;mybutton.dn_y2=19;
# ifdef ZAURUS
   mb=add_bmp_button(596,324,636,342,mybutton,&wcb_quit);
# else
   mb=add_bmp_button(596+rx0,452+ry0,636+rx0,470+ry0,mybutton,&wcb_quit);
# endif
   wdg_bind_key(mb,KEY_Q,-1,1);
   mybutton.up_x1=82;mybutton.up_y1=32;mybutton.up_x2=41;mybutton.up_y2=19;
   mybutton.dn_x1=123;mybutton.dn_y1=32;mybutton.dn_x2=41;mybutton.dn_y2=19;
   mb=add_bmp_button(596+rx0,CURRWORD_Y+90+ry0,636+rx0,CURRWORD_Y+108+ry0,mybutton,&wcb_clear_word);
   wdg_bind_key(mb,KEY_ESC,-1,1);
   mybutton.up_x1=164;mybutton.up_y1=32;mybutton.up_x2=41;mybutton.up_y2=19;
   mybutton.dn_x1=205;mybutton.dn_y1=32;mybutton.dn_x2=41;mybutton.dn_y2=19;
   mb=add_bmp_button(596+rx0,CURRWORD_Y+70+ry0,636+rx0,CURRWORD_Y+88+ry0,mybutton,&wcb_accept_word);
   wdg_bind_key(mb,KEY_SPACE,-1,1);
//   mb=add_button(596,CURRWORD_Y+130,636,CURRWORD_Y+148," SND",&wcb_sound);
   mb=add_invisible_button(596+rx0,CURRWORD_Y+130+ry0,636+rx0,CURRWORD_Y+148+ry0,&wcb_sound);
   wdg_bind_key(mb,KEY_M,-1,0);
   mb=add_invisible_button(596+rx0,CURRWORD_Y+150+ry0,636+rx0,CURRWORD_Y+168+ry0,&wcb_hint);
   wdg_bind_key(mb,KEY_H,-1,0);
#else
   mb=add_button(592+rx0,452+ry0,632+rx0,470+ry0,"Quit",&wcb_quit);
   wdg_bind_key(mb,KEY_Q,-1,1);
   mb=add_button(592+rx0,CURRWORD_Y+90+ry0,632+rx0,CURRWORD_Y+108+ry0," CLR",&wcb_clear_word);
   wdg_bind_key(mb,KEY_ESC,-1,1);
   mb=add_button(592+rx0,CURRWORD_Y+130+ry0,632+rx0,CURRWORD_Y+148+ry0," SND",&wcb_sound);
   wdg_bind_key(mb,KEY_M,-1,0);
   mb=add_button(592+rx0,CURRWORD_Y+150+ry0,632+rx0,CURRWORD_Y+168+ry0,"HINT",&wcb_hint);
   wdg_bind_key(mb,KEY_H,-1,0);
   mb=add_button(592+rx0,CURRWORD_Y+70+ry0,632+rx0,CURRWORD_Y+88+ry0," OK ",&wcb_accept_word);
   wdg_bind_key(mb,KEY_SPACE,-1,1);
#endif
   gfx_validicon(3);
   mb=add_invisible_button(CURRWORD_X+590+rx0,CURRWORD_Y+12+ry0,CURRWORD_X+621+rx0,CURRWORD_Y+43+ry0,&wcb_accept_word);
//   wdg_bind_key(mb,-1,-1,1);
   mb=add_invisible_button(CURRWORD_X+rx0,CURRWORD_Y+ry0,CURRWORD_X+575+rx0,CURRWORD_Y+55+ry0,&wcb_click_word);
   wdg_bind_key(mb,KEY_BACKSPACE,-1,1);
   for(x=1;x<=12;x++) {
      mb=add_invisible_button(TILEPILE_X+((x-1)*48)+rx0,TILEPILE_Y+ry0,TILEPILE_X+((x-1)*48)+47+rx0,TILEPILE_Y+55+ry0,&wcb_click_letter);
      wdg_bind_key(mb,-1,-1,1);
   }   
   // Might not be the best place for this
   blit(bgbmp,screen,CURRWORD_X,CURRWORD_Y,CURRWORD_X+rx0,CURRWORD_Y+ry0,576,56);
}

#ifdef USESOUND
gfx_load_samples() {
   char wavf[255];
   sprintf(wavf,"%s%csounds%cbubbles.wav",datadir,mysep,mysep);
   bubble_smp=load_wav(wavf);
   sprintf(wavf,"%s%csounds%cdrop.wav",datadir,mysep,mysep);
   drop_smp=load_wav(wavf);
   sprintf(wavf,"%s%csounds%cchaching.wav",datadir,mysep,mysep);
   chaching_smp=load_wav(wavf);
   sprintf(wavf,"%s%csounds%cno.wav",datadir,mysep,mysep);
   non_smp=load_wav(wavf);
   sprintf(wavf,"%s%csounds%cexcl1.wav",datadir,mysep,mysep);
   excl1_smp=load_wav(wavf);
   sprintf(wavf,"%s%csounds%cexcl2.wav",datadir,mysep,mysep);
   excl2_smp=load_wav(wavf);
   sprintf(wavf,"%s%csounds%cexcl3.wav",datadir,mysep,mysep);
   excl3_smp=load_wav(wavf);
   sprintf(wavf,"%s%csounds%cexcl4.wav",datadir,mysep,mysep);
   excl4_smp=load_wav(wavf);
//   sprintf(wavf,"%s%csounds%cmusic.wav",datadir,mysep,mysep);
//   music_smp=load_wav(wavf);
}
#endif
#ifdef GFXBITMAP
void gfx_load_bitmaps() {
   PALETTE p;
   char bmpf[255];
   get_palette(p);
   sprintf(bmpf,"%s%cgfx%cscrabble.pcx",datadir,mysep,mysep);
   tilesbmp=load_bitmap(bmpf,p);
   sprintf(bmpf,"%s%cgfx%cvicons.pcx",datadir,mysep,mysep);
   widgetbmp=load_bitmap(bmpf,p);
   sprintf(bmpf,"%s%cgfx%cwwbg.pcx",datadir,mysep,mysep);
   bgbmp=load_bitmap(bmpf,p);
   sprintf(bmpf,"%s%cgfx%cbanner.pcx",datadir,mysep,mysep);
   banrbmp=load_bitmap(bmpf,p);
   sprintf(bmpf,"%s%cgfx%cgears.pcx",datadir,mysep,mysep);
   gearbmp=load_bitmap(bmpf,p);
   if(!gearbmp) printf("no load %s\n",bmpf);
   if(WSmode==1) {
      sprintf(bmpf,"%s%cgfx%cwordlist.pcx",datadir,mysep,mysep);
      wlbmp=load_bitmap(bmpf,p);
      if(!wlbmp) printf("no load %s\n",bmpf);
   }
}
#endif

gfx_reset_flags() {
   currcmd=0;
   flg_morewords=-1;
   flg_numlet=3;
   flg_wc=0;
   wordcount=0;
   strcpy(vword,"X");
}

gfx_unset_widescreen() {
   int i, step;
   
#if defined(ASUSEEE) || defined(ZAURUS)
   step=20;
#else
   step=10;
#endif
//   printf("NOW1 %d\n",rx0);
   rectfill(screen,rx0-160,ry0,rx0-1,ry0+480,makecol(0,0,0));
   for(i=0;i<(80/step);i++) {
      blit(screen,screen,rx0,ry0,rx0-step,ry0,640,480);
      rx0=rx0-step;
      blit(screen,screen,0,ry0,rx0+640,ry0,step,480);
   }
//   printf("NOW2 %d\n",rx0);
}

gfx_set_widescreen() {
   int i, step;
   
#if defined(ASUSEEE) || defined(ZAURUS)
   step=20;
#else
   step=10;
#endif
//   printf("NOW1 %d\n",rx0);
   for(i=0;i<(80/step);i++) {
      blit(screen,screen,rx0-step,ry0,rx0,ry0,640+step,480);
      rx0=rx0+step;
   }
#ifdef GFXBITMAP
   step=5;
   for(i=0;i<=(160/5);i++)
     blit(wlbmp,screen,160-(5*i),0,rx0-160,ry0,(i*5),479);
#endif
//   printf("NOW2 %d\n",rx0);
}

gfx_findwords() {
   char wst[40];
   int chsz=50,wrst; //word status
   if(flg_wc>=chsz) {
//      printf("Invoked yellow banner---  %d\n",flg_wc);
//      sprintf(wst,"Checking Words...(%d) %d",flg_numlet,flg_wc);
      sprintf(wst,"Checking Words.......%d",flg_wc);
      blit(bgbmp,screen,CURRWORD_X+200,0,CURRWORD_X+200+rx0,0+ry0,224,16);
      fnt_print_string(screen,CURRWORD_X+200+rx0,0+ry0,wst,makecol(240,240,0),-1,-1);
   }
   if(flg_wc==0)
     wrst=find_words_chunk(3,0); // start new search
   wrst=find_words_chunk(flg_numlet,chsz);
   flg_wc=flg_wc+chsz;
   if(wrst==0) {
      flg_numlet++;
      wrst=find_words_chunk(flg_numlet,0); // start a new letter length
   }
   if(wrst==1) {
      flg_morewords=1;
      blit(bgbmp,screen,CURRWORD_X+200,0,CURRWORD_X+200+rx0,0+ry0,224,16);
   }   
   if(wrst==2)
     flg_morewords=0;
}

gfx_display_hs(int tno) {
   int nscs,i,txtcolor,bidx;
   int hx=TILEPILE_X+32,hy=TILEPILE_Y+32,hc;
   char sname[30],sdetail[30],sdate[30],sscore[8],outln[82];
   scare_once();
   hs_names(tno,sname,sdetail);
//   printf("displaying score table %d\n",tno);
//   printf("\n%s\n--------------------------\n",sname);
   blit(bgbmp,screen,hx,hy,hx+rx0,hy+ry0,512,304);
//   ww_sleep(2222);
   gfx_darken_rect(screen,hx+rx0,hy+ry0,hx+512+rx0,hy+304+ry0);
#ifdef USESDL
   SDL_UpdateRect(screen,hx+rx0,hy+ry0,512,304);
#endif
   nscs=hs_numscores(tno);
   //header
#ifdef GFXBITMAP
   bidx=tno-1;
   if(bidx==2 || bidx==3) bidx=bidx+2;
   masked_blit(banrbmp,screen,0,bidx*41,hx+rx0,hy+ry0,513,41);
//   masked_blit(banrbmp,screen,0,(tno-1)*41,hx,hy,513,41);
#else
   hc=(512-(strlen(sname)*8))/2;
   fnt_print_string(screen,hx+hc+rx0,hy+2+ry0,sname,makecol(240,230,0),-1,makecol(200,0,0));
   if(tno==2 || tno==4)
     sprintf(outln,"  %2s  %-18s%-12s%-18s%4s\n","#","Name","Word","Date","Score");
   else
     sprintf(outln,"  %2s  %-20s %-20s %10s\n","#","Name","Date","Score");
   fnt_print_string(screen,hx+rx0,hy+20+ry0,outln,makecol(250,230,64),-1,-1);
   hline(screen,hx+20+rx0,hy+39+ry0,hx+480+rx0,makecol(250,230,64));
   hline(screen,hx+20+rx0,hy+40+ry0,hx+480+rx0,makecol(180,30,32));
#endif
   for(i=1;i<=nscs;i++) {
      hs_getfield(sname,tno,i,"name");
      hs_getfield(sdate,tno,i,"date");
      hs_getfield(sscore,tno,i,"score");
      txtcolor=makecol(240,230,200);
      if(tno==2 || tno==4) {
	 hs_getfield(sdetail,tno,i,"detail");
	 sprintf(outln,"  %2d) %-18s%-12s%-18s%4s\n",i,sname,sdetail,sdate,sscore);
      } else {
	 if(atoi(sscore)==totscore && totscore>0) 
	   txtcolor=makecol(255,255,100);
	 sprintf(outln,"  %2d) %-20s %-20s %10s\n",i,sname,sdate,sscore);
      }
      fnt_print_string(screen,hx+rx0,hy+32+(i*16)+ry0,outln,txtcolor,-1,-1);
   }   
   unscare_once();
}

char gfx_title(int numtbl) {
   char rv,title[]="  WORDWORX  ",vers[50];
   int i,di=0,dela=5;
   long lt;
   Widget *mb;
#ifdef GFXBITMAP
   bmpbtn_t mybutton;
#endif

//   printf("title 1\n");
   currcmd=0;
   draw_bg();
//   printf("title 2\n");
   for(i=0;i<12;i++) {
      gfx_drawtile(screen,CURRWORD_X+(i*48)+rx0,CURRWORD_Y+ry0,title[i],0);
   }
//   printf("title 3\n");
   widget_clear_level();
//   printf("title 4\n");
   wdg_install_hilight(makecol(255,255,255),KEY_TAB,KEY_ENTER,0,KEY_DOWN,KEY_LEFT,KEY_RIGHT);
//   printf("title 5\n");
   sprintf(vers,"Version %s   (C)2007-2018 Tony Smolar",VERSION);
   fnt_print_string(screen,CURRWORD_X+140+rx0,CURRWORD_Y+60+ry0,vers,makecol(240,128,250),-1,makecol(80,32,90));
#ifdef GFXBITMAP
   mybutton.btnupbmp=widgetbmp;
   mybutton.btndnbmp=widgetbmp;
   mybutton.btnhlbmp=widgetbmp;
   // Start
   mybutton.up_x1=0;mybutton.up_y1=51;mybutton.up_x2=58;mybutton.up_y2=23;
   mybutton.dn_x1=0;mybutton.dn_y1=74;mybutton.dn_x2=58;mybutton.dn_y2=23;
   mybutton.hl_x1=0;mybutton.hl_y1=97;mybutton.hl_x2=58;mybutton.hl_y2=23;
   mb=add_bmp_button(50+rx0,432+ry0,108+rx0,455+ry0,mybutton,&wcb_start);
   wdg_bind_key(mb,KEY_S,-1,1);
   // Exit
   mybutton.up_x1=58;mybutton.up_y1=51;mybutton.up_x2=58;mybutton.up_y2=23;
   mybutton.dn_x1=58;mybutton.dn_y1=74;mybutton.dn_x2=58;mybutton.dn_y2=23;
   mybutton.hl_x1=58;mybutton.hl_y1=97;mybutton.hl_x2=58;mybutton.hl_y2=23;
   mb=add_bmp_button(115+rx0,432+ry0,173+rx0,455+ry0,mybutton,&wcb_quit);
   wdg_bind_key(mb,KEY_Q,-1,1);
   // Clear Scores
   mybutton.up_x1=116;mybutton.up_y1=51;mybutton.up_x2=118;mybutton.up_y2=23;
   mybutton.dn_x1=116;mybutton.dn_y1=74;mybutton.dn_x2=118;mybutton.dn_y2=23;
   mybutton.hl_x1=116;mybutton.hl_y1=97;mybutton.hl_x2=118;mybutton.hl_y2=23;
   mb=add_bmp_button(431+rx0,432+ry0,549+rx0,455+ry0,mybutton,&wcb_clearhs);
   wdg_bind_key(mb,-1,-1,1);
#else
   mb=add_button(50+rx0,432+ry0,106+rx0,450+ry0," Start ",&wcb_start);
//   mb=add_button(50,432,108,455," Start ",&wcb_start);
   wdg_bind_key(mb,KEY_S,-1,1);
   mb=add_button(115+rx0,432+ry0,171+rx0,450+ry0," Exit ",&wcb_exit);
//   mb=add_button(115,432,173,455," Exit ",&wcb_quit);
   wdg_bind_key(mb,KEY_Q,-1,1);
   mb=add_button(450+rx0,432+ry0,549+rx0,450+ry0,"Clear Scores ",&wcb_clearhs);
//   mb=add_button(431,432,549,455,"Clear Scores ",&wcb_clearhs);
   wdg_bind_key(mb,-1,-1,1);
#endif

   lt=0;
   
   while(1) {
      rest(0);
      if(time(NULL)>(lt+dela-1)) {
	 if(di==0) {
	    if(ywbmp) {
	       blit(bgbmp,screen,TILEPILE_X+32,TILEPILE_Y+32,TILEPILE_X+32+rx0,TILEPILE_Y+32+ry0,512,304);
	       gfx_darken_rect(screen,TILEPILE_X+32+rx0,TILEPILE_Y+32+ry0,TILEPILE_X+544+rx0,TILEPILE_Y+336+ry0);
	       masked_blit(ywbmp,screen,0,0,TILEPILE_X+32+rx0,TILEPILE_Y+32+ry0,511,303);
	    } else
	      di++;
	 }
	 if(di>=1 && di<=numtbl) 
	   display_hs(di);
	 di++;
	 if(di>numtbl) di=0;
	 lt=time(NULL);
      }
      
      event_loop(JUST_ONCE);
      kbd_loop(JUST_ONCE);
      if(currcmd==KEY_Q) {
	 rv='Q';
	 break;
      }
      if(currcmd==KEY_S) {
	 rv='S';
	 break;
      }
   }
   return(rv);
}

gfx_setstyle() {
   mystyle=activestyle;
#ifdef GFXBITMAP
   mystyle.window.bg=makeacol(182,170,170,200);
//   printf("A comp is %d\n",geta(mystyle.window.bg));
#else
   mystyle.window.bg=makeacol(128,128,128,224);
#endif
   mystyle.input.bg=makecol(80,0,90);
   mystyle.input.tx=makecol(255,224,0);
   mystyle.input.sh=makecol(65,65,65);
   mystyle.input.hl=makecol(192,192,192);
//   printf("Before:  abg: %d %d %d\n",getr(activestyle.window.bg),getg(activestyle.window.bg),getb(activestyle.window.bg));
   wdgst_apply(mystyle);
//   printf("Did it work?  abg: %d %d %d %d\n",getr(activestyle.window.bg),getg(activestyle.window.bg),getb(activestyle.window.bg),geta(activestyle.window.bg));
}

gfx_init() {
   int w;
   //   PALETTE p;
   // char fullscr='a';
   char fullscr=fsmode;
   //    char tfont[]="gamefont.fnt";
   char gtitle[40],ifn[200];
   //    strcpy(fontdir,"/tmp");
   // 
   // Version Info, move elsewhere
   // printf("WordWorx Version %s\n",VERSION);
   if(allegro_init()!=0) {
#ifdef DEBUG
      printf("allegro_init failed!\n");
#endif
//      printf("c xx\n");
      exit(33);
   }

//   printf("c 2\n");
#if defined(USESDL) && defined(GFXBITMAP) && !defined(ZAURUS)
   // This was a pain to get to work
   sprintf(ifn,"%s%cicons%cicon32.bmp",datadir,mysep,mysep);
//   printf("loading %s\n",ifn);
   wibmp=SDL_LoadBMP(ifn);
   w=SDL_MapRGB(wibmp->format, 255, 0, 255);
   SDL_SetColorKey(wibmp, SDL_SRCCOLORKEY,w);
   w=0;
   SDL_WM_SetIcon(wibmp,NULL);
   //   SDL_WM_SetIcon(SDL_LoadBMP(ifn),NULL);
#endif
//   printf("c 5\n");

   install_mouse();
   install_keyboard();
#ifdef USESOUND
   install_sound(DIGI_AUTODETECT, MIDI_NONE, NULL);
#endif
//   install_joystick(JOY_TYPE_AUTODETECT);
   set_color_depth(32);
//   printf("c 6\n");

   if(fullscr=='n')
     w=set_gfx_mode(GFX_AUTODETECT_WINDOWED,usex,usey,0,0);
   if(fullscr=='y')
     w=set_gfx_mode(GFX_AUTODETECT_FULLSCREEN,usex,usey,0,0);
   if(fullscr!='n' && fullscr!='y')
     w=set_gfx_mode(GFX_AUTODETECT,usex,usey,0,0);
   if(w!=0) {
#ifdef DEBUG
      printf("Could not set graphics mode!\n");
#endif
      exit(34);
   }
//   w=getpixel(wibmp,0,0);
//   printf("r:%d g:%d b:%d\n",getr(w),getg(w),getb(w));
   
   sprintf(gtitle,"WordWorx %s",VERSION);
   set_window_title(gtitle);
   show_mouse(screen);
   bgbmp=create_bitmap(640,480);
   // Bitmaps
#ifdef GFXBITMAP
   gfx_load_bitmaps();
#endif
#ifdef USESOUND
   gfx_load_samples();
//   play_sample(music_smp,255,0,1000,-1);
#endif
//   gfx_setstyle();
}

int gfx_endgame() {
   int wx=0,wy=0,wc=0,wscore=-1,hhs=0;
   char nword[14],dword[40],hsca;
   
   scare_once();
   rectfill(screen,CURRWORD_X+224+rx0,CURRWORD_Y+16+ry0,CURRWORD_X+352+rx0,CURRWORD_Y+40+ry0,makecol(0,0,0));
   if(currcmd==KEY_Q) {
      fnt_print_string(screen,CURRWORD_X+232+rx0,CURRWORD_Y+20+ry0,"   User Quit  ",makecol(240,240,240),-1,-1);
   } else {
      fnt_print_string(screen,CURRWORD_X+232+rx0,CURRWORD_Y+20+ry0,"No More Words!",makecol(240,240,240),-1,-1);
   }

   if(!ywbmp) {
      ywbmp=create_bitmap(512,304);
   }
   rectfill(ywbmp,0,0,511,303,makecol(255,0,255));
   gfx_darken_rect(screen,TILEPILE_X+32+rx0,TILEPILE_Y+32+ry0,TILEPILE_X+544+rx0,TILEPILE_Y+336+ry0);
   fnt_print_string(ywbmp,200,0,"Your Words",makecol(240,240,240),-1,makecol(0,0,0));
   fnt_print_string(ywbmp,140,284,"* Denotes hi-scoring word",makecol(240,240,240),-1,-1);

   do {
      hsca=' ';
      wscore=get_next_word(nword,wc);
      if(wscore!=0) {
	 wx=((wc/14)*288)+4;
	 wy=((wc%14)*16)+23;
//	 wx=((wc/14)*288)+TILEPILE_X+36;
//	 wy=((wc%14)*16)+TILEPILE_Y+64;
	 wc++;
	 if(hs_ishiscore(2,wscore)==1) {
	    hhs++;
	    hsca='*';
	 }
	 sprintf(dword,"%2d)%c%-12s        %3d",wc,hsca,nword,wscore);
	 // fnt_print_string(screen,wx,wy,dword,makecol(250,250,200),-1,makecol(32,32,32));   
	 fnt_print_string(ywbmp,wx,wy,dword,makecol(250,250,200),-1,makecol(32,32,32));
      }
   } while(wscore!=0);
   masked_blit(ywbmp,screen,0,0,TILEPILE_X+32+rx0,TILEPILE_Y+32+ry0,511,303);
   unscare_once();

   if(currcmd != KEY_Q) 
     ww_sleep(2000);
   return(hhs);
}

char gfx_game_loop() {
   char rv='.';
   // printf("About to enter loop\n");
   while(1) {
      if(flg_morewords>0) // Don't rest if checking words
	rest(0);
      event_loop(JUST_ONCE);
      kbd_loop(JUST_ONCE);
      if(flg_morewords<0)
	gfx_findwords();
      if(flg_morewords==0)
	break;
      if(currcmd==KEY_Q) {
	 rv='Q';
	 break;
      }
   }
   return(rv);
}
