#include <stdio.h>

#ifndef USESDL
#include <allegro.h>
#include <allegro/internal/aintern.h>
#else
#include <SDL.h>
#include "sdl_allegro.h"
#endif
#ifdef COMPFONT
//unsigned char fontdata[4096];
#include "fontdata.h"
#endif

#define FONT_VERSION "3.0.0"

unsigned char arps_font[16][256];
int font_fr, font_fg, font_fb;
int font_br, font_bg, font_bb;
int fnfgcol, fnbgcol, shdcol;
int fshadow, cachefont, colortable[8];
char fontdir[90], tfont[30], currfont[30];
int textbgr, textbgg, textbgb, textfgr, textfgg, textfgb;
int textsdr, textsdg, textsdb, texthlr, texthlg, texthlb;
int textier, textieg, textieb;
int descbgr, descbgg, descbgb;
int shadowr, shadowg, shadowb;
int banrbgr, banrbgg, banrbgb, banrfgr, banrfgg, banrfgb;

BITMAP *fntbmp;

void fnt_version_info() {
   printf("Font Library Version %s\n",FONT_VERSION);
   #ifdef BLITFONT
   printf("  * Using BLITFONT\n");
   #else
   printf("  * Not Using BLITFONT\n");
   #endif
   #ifdef COMPFONT
   printf("  * Using Compiled-in Font\n");
   #else
   printf("  * Using disk-loaded Fonts\n");
   #endif
}

#ifdef BLITFONT

int find_fontcolor_idx(int fcolor)  {
  /* When the font is loaded, it is stored in several different colors
   * The color is passed to this function, and the color table will be 
   * searched, returning the bitmap index offset for that color
   * (if available) */
   int i,r;

   r=0;
   for(i=0;i<8;i++) {
      if(colortable[i] == fcolor) {
	 r=i*16;
	 break;
      }
   }
   
   return(r);
}

load_colortbl() 
{
   colortable[0]=makecol16(textfgr,textfgg,textfgb);
   colortable[1]=makecol16(shadowr,shadowg,shadowb);
   colortable[2]=makecol16(banrfgr,banrfgg,banrfgb);
   colortable[3]=makecol16(textier,textieg,textieb);
   colortable[4]=makecol16(textsdr,textsdg,textsdb);
   colortable[5]=makecol16(255,255,255);      
}


bmp_font_load(char *filen)  {
   /* Load cached bitmap fonts rather than bios fonts */
   char fontname[120];
   PALETTE pal;
   
   get_palette(pal);
   sprintf(fontname, "%s/%s",fontdir, filen);
   
   load_colortbl();
   if((fntbmp = load_pcx(fontname,pal)) == NULL) 
     {
	destroy_bitmap(fntbmp);
	font_load(tfont);
     }
   strcpy(currfont,tfont);
}

font_load(char *filen)  {
   FILE *fp;
   char fontname[120];
   int a,ix,iy,cx,col0;
   PALETTE pal;
   
   fntbmp = create_bitmap(1024,96);
   col0=makecol16(255,0,255);
   
   sprintf(fontname, "%s/%s",fontdir, filen);
   // printf("font=%s current=%s\n",fontname,currfont);
 
   if ((fp = fopen(fontname,"rb"))==NULL)  {
      printf("Error opening file %s\n",fontname);
      exit(1);
   }

   load_colortbl();
//   printf("0:%d  1:%d   2:%d  2:%d",colortable[0],colortable[1],colortable[2],colortable[3]);
   
   for(ix=0;ix<128;ix++)  {
      for(iy=0;iy<16;iy++) {
	 a=(unsigned char)fgetc(fp);
	 for(cx=0;cx<8;cx++) {
	    a<<=1;
	    if(a>255) {		   
	       putpixel(fntbmp, (ix*8)+cx, iy, colortable[0]);
	       putpixel(fntbmp, (ix*8)+cx, iy+16, colortable[1]);
	       putpixel(fntbmp, (ix*8)+cx, iy+32, colortable[2]);
	       putpixel(fntbmp, (ix*8)+cx, iy+48, colortable[3]);
	       putpixel(fntbmp, (ix*8)+cx, iy+64, colortable[4]);
	       putpixel(fntbmp, (ix*8)+cx, iy+80, colortable[5]);
	       a=a-256;  
	    } else {
	       putpixel(fntbmp, (ix*8)+cx, iy, col0);
	       putpixel(fntbmp, (ix*8)+cx, iy+16, col0);
	       putpixel(fntbmp, (ix*8)+cx, iy+32, col0);
	       putpixel(fntbmp, (ix*8)+cx, iy+48, col0);
	       putpixel(fntbmp, (ix*8)+cx, iy+64, col0);
	       putpixel(fntbmp, (ix*8)+cx, iy+80, col0);
	    }
	    
	 }
      }
   }
   
   fclose(fp);
   strcpy(currfont,filen);
   if(cachefont == 1) 
     {
//	get_palette(pal);
//	save_pcx("/tmp/fontbmp.pcx", fntbmp, pal);
     }
}

display_char(int x, int y, unsigned char chr) {
   int coly;
   coly=find_fontcolor_idx(fnfgcol);
   masked_blit(fntbmp,screen,(chr*8),coly,x,y,8,16);
}

shadow_char(int x, int y, unsigned char chr) {
   int coly;
   coly=find_fontcolor_idx(fnfgcol);
   masked_blit(fntbmp,screen,(chr*8),16,x+1,y+1,8,16);
   masked_blit(fntbmp,screen,(chr*8),coly,x,y,8,16);
}

solid_char(int x, int y, unsigned char chr) {
   int coly;
   coly=find_fontcolor_idx(fnfgcol);
   rectfill(screen,x,y,x+7,y+15,fnbgcol);
   masked_blit(fntbmp,screen,(chr*8),coly,x,y,8,16);
}
#else
font_load(char *filen) {
   FILE *fp;
   char a;
   int ix,iy;
   char fontname[120];
   

   strcpy(fontname, fontdir);
   strcat(fontname, "/");
   strcat(fontname, filen);
# ifdef DEBUG
   printf("fontname is %s\n", fontname);
# endif
   // NOTE: Chaned fontname to filen b/c it's having trouble
   //     with paths.   Probably a cygwin/mingwin issue
   if ((fp = fopen(filen,"rb"))==NULL)  {
      printf("Error opening file %s",fontname);
      exit(1);
   }
   
# ifdef DEBUG
   printf("Loading fonts....");
# endif	
   for(iy=0;iy<256;iy++)  {
	   for(ix=0;ix<16;ix++) {
	      a=(unsigned char)fgetc(fp);
# ifdef COMPFONT
	      fontdata[(iy*16)+ix]=a;
# else
	      arps_font[ix][iy]=a; 
# endif
	   }
   }
# ifdef DEBUG
   printf("Done.\n");
# endif
   fclose(fp);
   strcpy(currfont,filen);
}

// Improved Font Routines
fnt_display_char(BITMAP *b,int x, int y, unsigned char chr,int fcolor) {
   int t;
   int c;
   int dc,cx;

   for(t=0; t<16; t++) {
#ifdef COMPFONT
      c=fontdata[(chr*16)+t];
#else
      c=arps_font[t][chr];
#endif
      for(cx=0;cx<8;cx++) {
	 c<<=1;
	 if(c>255) {
//	    putpixel(b, x+cx, y+t, fnfgcol);
	    putpixel(b, x+cx, y+t, fcolor);
	   c=c-256;  
	 }
      }
   }
} // fnt_display_char

// Legacy Char Functions
display_char(int x, int y, unsigned char chr) {
   fnt_display_char(screen,x, y, chr,fnfgcol);
}

shadow_char(int x, int y, unsigned char chr) {
   fnt_display_char(screen,x+1, y+1, chr,shdcol);
   fnt_display_char(screen,x, y, chr,fnfgcol);
}

solid_char(int x, int y, unsigned char chr) {
   rectfill(screen,x,y,x+7,y+15,fnbgcol);
   fnt_display_char(screen,x, y, chr,fnfgcol);
}
// End Legacy Char Functions
#endif

// More Improved Font Routines
fnt_print_string(BITMAP *b, int x, int y, char *str, int fg, int bg, int sd) {
   // New method that can draw on any bitmap, and hand solid, shadow and plain
   int c=0, l=0, cx,cy, sl=strlen(str)*8;
   
   if(bg>-1)
     rectfill(b,x,y,x+sl,y+15,bg);
#ifdef USESDL
   // Put here for speed optimization
//   if(SDL_MUSTLOCK(screen)){
   if(b==screen)
     if(SDL_LockSurface(b) < 0) return;
#endif

   while(*str) {
      if (*str == '\n'){
	 l++;
	 str++;
	 c=0;
      } else {
	 cx=x+(c++)*8;
	 cy=y+l*9;
	 if(sd>-1) // shadow
	   fnt_display_char(b, cx+1, cy+1, *(str),sd);
	 fnt_display_char(b, cx, cy, *(str++),fg);
      }
   }
   
#ifdef USESDL
   if(b==screen) {
      SDL_UnlockSurface(b);
      if(SA_AUTOUPDATE==1)
	SDL_UpdateRect(screen,x,y,sl,16);
//      printf("hokee: %d %d %d\n",x,y,sl);
   }
#endif
} // fnt_print_string

// Legacy Routines

set_font_fcolor(int r, int g, int b) {
   font_fr = r; font_fg = g; font_fb = b;
   fnfgcol=makecol16(r,g,b);
}

set_font_bcolor(int r, int g, int b) {
   font_br = r; font_bg = g; font_bb = b;
   fnbgcol=makecol16(r,g,b);
}

set_font_scolor(int r, int g, int b) {
   /* Seems reversed, but why? */
   shdcol=makecol16(r,g,b);
}

solid_string(int x, int y, char *stt) {
   if(fshadow==1)
     fnt_print_string(screen,x,y,stt,fnfgcol,fnbgcol,shdcol);
		      else
     fnt_print_string(screen,x,y,stt,fnfgcol,fnbgcol,-1);
}

show_string(int x, int y, char *stt) {
   if(fshadow==1)
     fnt_print_string(screen,x,y,stt,fnfgcol,-1,shdcol);
		      else
     fnt_print_string(screen,x,y,stt,fnfgcol,-1,-1);
}
// End Legacy Routines

/* Calculates the width and height of the string in pixels.
 * width and height have to be non-constant pointers to int.	
 */
 
calc_width(char *stt) {
   int tmp_width=8;
   int width=8;
   
   while(*stt) {
      if (*stt == '\n') {
         if(tmp_width > width)
            width=tmp_width;
         tmp_width=8;
      } else
         tmp_width+=8;
      stt++;
   }
   if(tmp_width > width)
      width=tmp_width;
   return width;
      
}
calc_height(char *stt) {
   int height=9;
   
   while(*stt) {
      if (*stt == '\n')
         height+=9;
      stt++;
   }
   
   return height ;  
}

// Deprecated Functions
// BITMAP
// 
// NON-BITMAP
//display_char(int x, int y, unsigned char chr) {
//   int t;
//   int c;
//   int dc,cx;
//   
//   for(t=0; t<16; t++) {
//# ifdef COMPFONT
//      c=fontdata[(chr*16)+t];
//# else
//      c=arps_font[t][chr];
//# endif
//      for(cx=0;cx<8;cx++) {
//	 c<<=1;
//	 if(c>255) {
//	    /* 	   arps_setpixel32(x+cx,y+t,font_fr, font_fg, font_fb); */
//	    putpixel(screen, x+cx, y+t, fnfgcol);
//	    c=c-256;  
//	 }
//    }
//   }
//}
//
//shadow_char(int x, int y, unsigned char chr) {
//   int t;
//   int c;
//   int dc,cx, tcol;
//   
//   /* Draw drop shadow */
//   tcol=fnfgcol;
//   fnfgcol=shdcol;
//   display_char(x+1,y+1,chr);
//   fnfgcol=tcol;
//   /* End drop shadow */
//   
//   for(t=0; t<16; t++) {
//#ifdef COMPFONT
//      c=fontdata[(chr*16)+t];
//#else
//      c=arps_font[t][chr];
//#endif
//      for(cx=0;cx<8;cx++) {
//	 c<<=1;
//	 if(c>255) {
///* 	   arps_setpixel32(x+cx,y+t,font_fr, font_fg, font_fb); */
//	    putpixel(screen, x+cx, y+t, fnfgcol);
//	   c=c-256;  
//	 }
//      }
//   }
//}
//
//solid_char(int x, int y, unsigned char chr) {
//   int t;
//   int c;
//   int dc,cx;
//   
//   for(t=0; t<16; t++) {
//#ifdef COMPFONT
//      c=fontdata[(chr*16)+t];
//#else
//      c=arps_font[t][chr];
//#endif
//      for(cx=0;cx<8;cx++) {
//	 c<<=1;
//	 if(c>255) {
// 	/*   arps_setpixel32(x+cx,y+t,font_fr, font_fg, font_fb); */
//	    putpixel(screen, x+cx, y+t, fnfgcol);
//	   c=c-256;  
//	 } else {
//	   /* arps_setpixel32(x+cx,y+t,font_br, font_bg, font_bb); */
//	    putpixel(screen, x+cx, y+t, fnbgcol);
//	 }
//      }
//   }
//}
//
//show_string(int x, int y, char *stt) {
//
//   int c=0;
//   int l=0;
//   
//#ifdef USESDL
//   // Put here for speed optimization
//   if(SDL_MUSTLOCK(screen)){
//      if(SDL_LockSurface(screen) < 0) return;
//   }
//#endif
//      
//   while(*stt) {
//      if (*stt == '\n'){
//      	l++;
//      	stt++;
//      	c=0;
//      } else {
//	 if(fshadow==1) { 
////	    printf("shadow!\n");
//	    shadow_char(x+(c++)*8,y+l*9,*(stt++));
//	 } else {
//	    display_char(x+(c++)*8,y+l*9,*(stt++));    
//	 }
//      }
//   }
//#ifdef USESDL
//   if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
////   SDL_Flip(screen);
//#endif
//}
//
//solid_string(int x, int y, char *stt) {
//
//   int c=0;
//   int l=0;
//   int cx,cy;
//
//#ifdef USESDL
//   // Put here for speed optimization
//   if(SDL_MUSTLOCK(screen)){
//      if(SDL_LockSurface(screen) < 0) return;
//   }
//#endif
//   while(*stt) {
//      if (*stt == '\n'){
//      	l++;
//      	stt++;
//      	c=0;
//      } else {
//	 cx=x+(c++)*8;
//	 cy=y+l*9;
//	 if(fshadow==1) { 
//	    rectfill(screen,cx,cy,cx+7,cy+15,fnbgcol);
//	    shadow_char(cx,cy,*(stt++));
//	 } else {
///*	    solid_char(x+(c++)*8,y+l*9,*(stt++)); */
//	    solid_char(cx,cy,*(stt++));
//	 }
//      }
//   }
//   
//#ifdef USESDL
//   if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
////   SDL_Flip(screen);
//#endif
//      
//}
// 
//solid_string1(int x, int y, char *stt) {
//
//   int c=0;
//   int l=0;
//   int cx,cy,sl;
//
//#ifdef USESDL
//   // Put here for speed optimization
//   if(SDL_MUSTLOCK(screen)){
//      if(SDL_LockSurface(screen) < 0) return;
//   }
//#endif
//   sl=strlen(stt)*8;
//   rectfill(screen,x,y,x+sl,y+15,fnbgcol);
//   
//   while(*stt) {
//      if (*stt == '\n'){
//      	l++;
//      	stt++;
//      	c=0;
//      } else {
//	 cx=x+(c++)*8;
//	 cy=y+l*9;
//	 if(fshadow==1) { 
////	    rectfill(screen,cx,cy,cx+7,cy+15,fnbgcol);
//	    shadow_char(cx,cy,*(stt++));
//	 } else {
///*	    solid_char(x+(c++)*8,y+l*9,*(stt++)); */
//	    display_char(cx,cy,*(stt++));
//	 }
//      }
//   }
//   
//#ifdef USESDL
//   if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
////   SDL_Flip(screen);
//#endif
//      
//}
// End Deprecated Function
