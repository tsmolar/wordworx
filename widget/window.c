#include <stdio.h>
#ifdef USESDL
#include <SDL.h>
#include "sdl_allegro.h"
#else
#include <allegro.h>
#endif
#define WIDTH VIRTUAL_W
#define HEIGHT VIRTUAL_H
//#include "graphics.h"
#include "window.h"
//#include "platform.h"
//#include "colordefs.h"

/* These are routines to create simple windows which can be used for the
 * basis of Alert Boxes, Dialogs, etc. 
 */

/* Modified on 1-5-2006 Tony Smolar */
/* Modified on 4-30-2007 Tony Smolar 
    - Alert Decor */

int mx,my,mk;

void new_window(int x1, int y1, int x2, int y2) {
   int c252,c253,al;
   
   c252=activestyle.window.bg;
   c253=activestyle.window.hl;
   al=activestyle.window.al;
   push_level(x1,y1,x2,y2); /* disable the widgets underneath */
   style_rectfill(screen,x1,y1,x2-1,y2-1,c252,al);
   style_hline(screen,x1+1,y2,x2,c253,al);
   style_vline(screen,x2,y1+1,y2,c253,al);
//   arps_pbox32(x1,y1,x2-1,y2-1,COLOR252);
//   arps_line32(x1+1,y2,x2,y2,COLOR253);
//   arps_line32(x2,y1+1,x2,y2,COLOR253);
}

int alert_button;

ab1(Widget* w,int x,int y,int m){
	alert_button=1;
	close_window();
}

ab2(Widget* w,int x,int y,int m){
	alert_button=2;
	close_window();
}

ab3(Widget* w,int x,int y,int m){
	alert_button=3;
	close_window();
}

int wdg_alert(char *text,char *b1,char *b2,char *b3) {
	int width,height,x,y,mbw,bw;
	mbw=40;
		
	if(b1){
		bw=calc_width(b1);
		if(bw > mbw) mbw=bw;
	}
	if(b2){
		bw=calc_width(b2);
		if(bw > mbw) mbw=bw;
	}
	if(b3){
		bw=calc_width(b3);
		if(bw > mbw) mbw=bw;
	}
	mbw+=2;
	width=calc_width(text);
	height=calc_height(text);
	width+=16;
	if (width < 3*mbw+6) width=3*mbw+6;
	height+=32;
	height+=32; // For Banner
	x=(WIDTH-width)/2;
	y=(HEIGHT-height)/2;
	new_window(x,y,x+width,y+height);
	if(b1)
	   add_button(x+8,y+height-20,x+mbw,y+height-4,b1,&ab1);
	if(b2)
	   add_button(x+mbw+2,y+height-20,x+2*mbw+2,y+height-4,b2,&ab2);
	if(b3)
	   add_button(x+2*mbw+4,y+height-20,x+3*mbw+4,y+height-4,b3,&ab3);
	show_string(x+8,y+24,text);
   // Decor
        style_rectfill(screen,x+4,y+4,x+width-4,y+14,makecol(32,32,120),255);
        style_hline(screen,x,y,x+width,makecol(220,220,220),255);
        style_vline(screen,x,y,y+height,makecol(220,220,220),255);
        style_hline(screen,x,y+height,x+width,makecol(24,24,24),255);
        style_vline(screen,x+width,y,y+height,makecol(24,24,24),255);
   // End Decor
	event_loop(HALT_ON_POP);
	return alert_button;
}

int status(char *text) {
	int width,height,x,y;
	width=calc_width(text);
	height=calc_height(text);
	width+=16;
	height+=16;
	x=(WIDTH-width)/2;
	y=(HEIGHT-height)/2;
	new_window(x,y,x+width,y+height);
	show_string(x+8,y+8,text);
}

// extern int just_popped;

int close_status() {
	close_window();
	just_popped=0;
}

int progress(char *text, int creatb, int num, int max) {
	int width,height,x,y,barw,barf;
        float pct;
        pct=(float)num/max;
        
	width=calc_width(text);
	height=calc_height(text);
	width+=16;
	height+=48;
	x=(WIDTH-width)/2;
	y=(HEIGHT-height)/2;
        if (creatb > 0) {
	   new_window(x,y,x+width,y+height);
	   show_string(x+8,y+8,text);
	}
        barw=(x+width-9)-(x+5);
        barf=pct*barw;
        style_rect(screen, x+4,y+32,x+width-8,y+height-4,makecol(224,224,224),255);
        style_rectfill(screen,x+5,y+33,x+5+barf,y+height-5,makecol(224,60,60),255);
}
