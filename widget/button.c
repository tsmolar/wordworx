#include <stdio.h>
#ifdef USESDL
#include <SDL.h>
#include "sdl_allegro.h"
#else
#include <allegro.h>
#endif
#include "font.h"
#include "button.h"
//#include "graphics.h"
//#include "platform.h"
//#include "colordefs.h"

#define COLOR253 92,92,92
#define COLOR254 160,160,160
#define COLOR255 240,16,16
#define in_box(x1,y1,x2,y2,mx,my) ((mx)>(x1) && (mx)<(x2) && (my)>(y1) && (my)<(y2))

int mcolor;

//extern int just_popped; /* from widget.c */

/*
 * 
 * Following are functions, that implement buttons
 * 
 */

up_bmp_button(Widget *b,int x,int y) /* called by mouse_depress */
{
   if(just_popped==0)     
     scare_once();
//        rem_pointer(x,y);
//   printf("ubb handler cb check %d\n", b->handler);
        if(in_box(b->x1,b->y1,b->x2,b->y2,x,y) && b->handler) 
            (*(b->handler))(b,x-b->x1,y-b->y1,-1);
   
//   printf("ubb handler cb success\n");
// I get it now, if just popped, then this button is destroyed, and executing
// anything below might cause a segfault.
   	if(just_popped) return;

//   printf("didn't just pop\n");
	restore_under(b);
//   printf("did restore under\n");
        masked_blit(b->btbmp.btnupbmp,screen,b->btbmp.up_x1,b->btbmp.up_y1,b->x1,b->y1,b->btbmp.up_x2,b->btbmp.up_y2);
//   printf("did blit\n");
//#ifdef USESDL
//        SDL_UpdateRect(screen,b->x1,b->y1,b->x2-1,b->y2-1);
//        SDL_Flip(screen);
////   printf("Update Rect! %d,%d   %d,%d\n",b->x1,b->y1,b->x2-1,b->y2-1);
//#endif
   unscare_once();
   // What's this for?
   if(just_popped) return;
//        draw_pointer(x,y);	
}

down_bmp_button(Widget *b,int x,int y,int m) /* called by mouse_press */
{
        scare_once();
//        rem_pointer(x,y);
//printf("x1\n");
	restore_under(b);
        masked_blit(b->btbmp.btndnbmp,screen,b->btbmp.dn_x1,b->btbmp.dn_y1,b->x1,b->y1,b->btbmp.dn_x2,b->btbmp.dn_y2);
//printf("x2\n");
#ifdef USESDL
        SDL_UpdateRect(screen,b->x1,b->y1,b->x2-1,b->y2-1);
#endif
        unscare_once();
//        draw_pointer(x,y);	
}

up_button(Widget *b,int x,int y) /* called by mouse_depress */
{
   if(just_popped==0)
        scare_once();
//        rem_pointer(x,y);
        if(in_box(b->x1,b->y1,b->x2,b->y2,x,y) && b->handler) 
            (*(b->handler))(b,x-b->x1,y-b->y1,-1);
   
//   printf("handler cb success\n");
// Should this be here?   
   	if(just_popped) return;
	
	restore_under(b);
        mcolor=makecol(COLOR254);
	rectfill(screen,b->x1,b->y1,b->x2-1,b->y2-1,mcolor);
        mcolor=makecol(COLOR253);
	line(screen,b->x2,b->y1+1,b->x2,b->y2,mcolor);
	line(screen,b->x1+1,b->y2,b->x2,b->y2,mcolor);
//        set_font_fcolor(255,255,255);
	fnt_print_string(screen,b->x1+3,b->y1+1,b->text,makecol(255,255,255),-1,-1);
#ifdef USESDL
        SDL_UpdateRect(screen,b->x1,b->y1,b->x2-1,b->y2-1);
#endif
        unscare_once();
   // What's this for?
	if(just_popped) return;
//        draw_pointer(x,y);	
}

down_button(Widget *b,int x,int y,int m) /* called by mouse_press */
{
        scare_once();
//        rem_pointer(x,y);
	restore_under(b);
        mcolor=makecol(COLOR254);
	rectfill(screen,b->x1+1,b->y1+1,b->x2,b->y2,mcolor);
//        set_font_fcolor(225,225,225);
	fnt_print_string(screen,b->x1+4,b->y1+2,b->text,makecol(225,225,225),-1,-1);
#ifdef USESDL
        SDL_UpdateRect(screen,b->x1,b->y1,b->x2-1,b->y2-1);
#endif
        unscare_once();
//        draw_pointer(x,y);	
}

void destroy_button(Widget* w){
   free(w->under);
   if(w->extra) free(w->extra);
   if(w->text) free(w->text);
   // TODO: The btbmp structure
}

void change_handler_invisible(Widget* w, int(*handler)(Widget*,int,int,int)) {
   /* Change the handler for an invisible button */
   w->press=handler;
}

void change_handler(Widget* w, int(*handler)(Widget*,int,int,int)) {
   /* Change the handler, same as activate button*/
   w->press=&down_button;
   w->depress=&up_button;
   w->handler=handler;
}

void deactivate_button(Widget* w) {
   /* Change the handler for an invisible button */
/*   down_button(w,0,0,0); */
   w->press=NULL;
   w->depress=NULL;
   w->handler=NULL;
}

/* Creates a new button */
Widget* add_button
  (int x1,int y1,int x2,int y2,char* text,int(*handler)(Widget*,int,int,int))
{
	Widget *w=new_widget();
	
	w->press=&down_button;
	w->depress=&up_button;
	w->draggable=0; /* &down_button is only called once */
	
	w->x1=x1;
	w->x2=x2;
	w->y1=y1;
	w->y2=y2;
	w->text=text;
	w->handler=handler;
	w->destroy=&destroy_button;
  	w->btbmp.btnupbmp = w->btbmp.btndnbmp = w->btbmp.btnhlbmp =NULL;
        w->extra=NULL;
	
	save_under(w);
	up_button(w,-1,-1);
	return w;
}

Widget* add_bmp_button
  (int x1,int y1,int x2,int y2,bmpbtn_t bbmp,int(*handler)(Widget*,int,int,int))
{
	Widget *w=new_widget();
	
	w->press=&down_bmp_button;
	w->depress=&up_bmp_button;
	w->draggable=0; /* &down_button is only called once */
	
	w->x1=x1;
	w->x2=x2;
	w->y1=y1;
	w->y2=y2;
   
	w->handler=handler;
//   printf("adding handler: %d\n",w->handler);
	w->destroy=&destroy_button;
  	w->btbmp.btnupbmp = w->btbmp.btndnbmp = w->btbmp.btnhlbmp =NULL;
	w->btbmp=bbmp;
        // Make sure this is null to prevent segfaults
        w->text=w->extra=NULL;
     
	save_under(w);
//   printf("pop the button!\n");
	up_bmp_button(w,-1,-1);
//   printf("popped the button!\n");
	return w;
}

/*
 * 
 * Following are functions, that implement arrow buttons
 * 
 */

up_up_arrow_button(Widget *b,int x,int y) /* called by mouse_depress */
{
	int i;
	restore_under(b);
        mcolor=makecol(COLOR254);
	for(i=0;i<7;i++) {
	   line(screen,b->x1+i,b->y1+6-i,b->x1+12-i,b->y1+6-i,mcolor);
	}
        mcolor=makecol(COLOR254);
	line(screen,b->x1+1,b->y1+7,b->x1+13,b->y1+7,mcolor);
	if(in_box(b->x1,b->y1,b->x2,b->y2,x,y) && b->handler)
		(*(b->handler))(b,x-b->x1,y-b->y1,-1);
}

down_up_arrow_button(Widget *b,int x,int y,int m) /* called by mouse_press */
{
	int i;
	restore_under(b);
        mcolor=makecol(COLOR254);
	for(i=0;i<7;i++) {
	  line(screen,b->x1+i+1,b->y1+7-i,b->x1+13-i,b->y1+7-i,mcolor);
	}
}

up_down_arrow_button(Widget *b,int x,int y) /* called by mouse_depress */
{
	int i;
	restore_under(b);
	for(i=0;i<7;i++) {
          mcolor=makecol(COLOR254);
	  line(screen,b->x1+i,b->y1+i,b->x1+12-i,b->y1+i,mcolor);
          mcolor=makecol(COLOR254);
	  line(screen,b->x1+i+1,b->y1+i+1,b->x1+13-i,b->y1+i+1,mcolor);
	}
	if(in_box(b->x1,b->y1,b->x2,b->y2,x,y) && b->handler)
		(*(b->handler))(b,x-b->x1,y-b->y1,-1);
}

down_down_arrow_button(Widget *b,int x,int y,int m) /* called by mouse_press */
{
	int i;
	restore_under(b);
        mcolor=makecol(COLOR254);
	for(i=0;i<7;i++) {
	  line(screen,b->x1+i+1,b->y1+i+1,b->x1+13-i,b->y1+i+1,mcolor);
	}
}


up_left_arrow_button(Widget *b,int x,int y) /* called by mouse_depress */
{
	int i;
	
	restore_under(b);
	
        mcolor=makecol(COLOR253);
        for(i=0;i<7;i++) {
		line(screen,b->x1+7-i,b->y1+i+1,b->x1+7,b->y1+i+1,mcolor);
		line(screen,b->x1+7-i,b->y1+13-i,b->x1+7,b->y1+13-i,mcolor);
	}
        mcolor=makecol(COLOR254);
	for(i=0;i<7;i++) {
		line(screen,b->x1+6-i,b->y1+i,b->x1+6,b->y1+i,mcolor);
		line(screen,b->x1+6-i,b->y1+12-i,b->x1+6,b->y1+12-i,mcolor);
	}
	if(in_box(b->x1,b->y1,b->x2,b->y2,x,y) && b->handler)
		(*(b->handler))(b,x-b->x1,y-b->y1,-1);
	
}

down_left_arrow_button(Widget *b,int x,int y,int m) /* called by mouse_press */
{
	int i;
	
	restore_under(b);
   
        mcolor=makecol(COLOR254);
	for(i=0;i<7;i++) {
	   line(screen,b->x1+7-i,b->y1+i+1,b->x1+7,b->y1+i+1,mcolor);
	   line(screen,b->x1+7-i,b->y1+13-i,b->x1+7,b->y1+13-i,mcolor);
	}
	

}

up_right_arrow_button(Widget *b,int x,int y) /* called by mouse_depress */
{
	int i;
	
	restore_under(b);
	
        mcolor=makecol(COLOR253);
	for(i=0;i<7;i++) {
		line(screen,b->x1+1,b->y1+i+1,b->x1+i+1,b->y1+i+1,mcolor);
		line(screen,b->x1+1,b->y1+13-i,b->x1+i+1,b->y1+13-i,mcolor);
	}
        mcolor=makecol(COLOR254);
	for(i=0;i<7;i++) {
		line(screen,b->x1,b->y1+i,b->x1+i,b->y1+i,mcolor);
		line(screen,b->x1,b->y1+12-i,b->x1+i,b->y1+12-i,mcolor);
	}
	if(in_box(b->x1,b->y1,b->x2,b->y2,x,y) && b->handler)
		(*(b->handler))(b,x-b->x1,y-b->y1,-1);
	
}

down_right_arrow_button(Widget *b,int x,int y,int m) /* called by mouse_press */
{
	int i;
	
	restore_under(b);
	
        mcolor=makecol(COLOR254);
	for(i=0;i<7;i++) {
		line(screen,b->x1+1,b->y1+i+1,b->x1+i+1,b->y1+i+1,mcolor);
		line(screen,b->x1+1,b->y1+13-i,b->x1+i+1,b->y1+13-i,mcolor);
	}
	

}

/* Creates a new button */
Widget* add_arrow_button
  (int x,int y,int dir,int(*handler)(Widget*,int,int,int))
{
	Widget *w=new_widget();
	
	if (dir == UP) {
		w->press=&down_up_arrow_button;
		w->depress=&up_up_arrow_button;
		w->x1=x-6;
		w->x2=x+7;
		w->y1=y;
		w->y2=y+7;
		w->text="UP";
	} else if (dir == DOWN) {
		w->press=&down_down_arrow_button;
		w->depress=&up_down_arrow_button;
		w->x1=x-6;
		w->x2=x+7;
		w->y1=y-7;
		w->y2=y;
		w->text="DOWN";
	} else if (dir == LEFT) {
		w->press=&down_left_arrow_button;
		w->depress=&up_left_arrow_button;
		w->x1=x;
		w->x2=x+7;
		w->y1=y-6;
		w->y2=y+7;
		w->text="RIGHT";
	} else if (dir == RIGHT) {
		w->press=&down_right_arrow_button;
		w->depress=&up_right_arrow_button;
		w->x1=x-6;
		w->x2=x+1;
		w->y1=y-6;
		w->y2=y+7;
		w->text="RIGHT";
	}
	
	w->draggable=0; /* &down_button is only called once */
	
	w->handler=handler;
	w->destroy=&destroy_button;
        w->extra=NULL;
	
	save_under(w);
	(*(w->depress))(w,-1,-1);
	return w;
}

/*
 * 
 * Following are functions, that implement invisible buttons
 * (to implement the icon editor and icon chooser, etc...)
 *
 */


Widget* add_invisible_button
   (int x1,int y1,int x2,int y2,int(*handler)(Widget*,int,int,int))
{
	Widget *w;
	w=new_widget();
	
	w->press=handler;
	w->depress=NULL; /* no action on mouse-up */
	w->draggable=0; /* initially, the handler is only called once
			   on mouse-press */
	
	w->x1=x1;
	w->x2=x2;
	w->y1=y1;
	w->y2=y2;
  	w->btbmp.btnupbmp = w->btbmp.btndnbmp = w->btbmp.btnhlbmp =NULL;
        w->extra=NULL;
	w->handler=NULL; /* the handler is called directly on mouse-press,
			    so it's assigned to w->press instead */
	
	
	return w;
}

