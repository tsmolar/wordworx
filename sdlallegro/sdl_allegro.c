#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include<SDL.h>
#include"sdl_allegro.h"
//#ifdef HAVE_SDLMIXER
//#include"SDL_mixer.h"
//#endif

#define SDL_ALLEGRO_VERS_C "0.6.5"

SDL_PixelFormat *sdlpixfmt;
SDL_Surface *screen;
int sa_depth;
char sdldriver[20];
#ifdef ZAURUS
int zcomp=0;
#endif
//#ifdef ZAURUS
//int zcomp=0;
//#endif
//extern int num_joysticks;

sa_version_info() {
   printf("SDL/Allegro Library Version %s\n",SDL_ALLEGRO_VERS_C);
#ifdef HAVE_LIBSDL_IMAGE
   printf("  * Have SDLImage for file loading\n");
#else
   printf("  * Not Using SDLImage\n");
#endif
#ifdef HAVE_LIBSDL_MIXER
   printf("  * Have SDLMixer for audio\n");
#else
   printf("  * Not Using SDLMixer\n");
#endif
#ifdef ZAURUS
   printf("  * Zaurus touchscreen compensation for bvdd/SDL\n");
#endif
}

sa_sdl_info() {
   const SDL_VideoInfo* xx=SDL_GetVideoInfo();
   printf("SDL Hardware info\n");
   printf("-----------------\n");
   printf("Can create hw surface?  %d\n",xx->hw_available);
   printf("Window Manager available?  %d\n",xx->wm_available);
   printf("Hardware->H Blits accellerated?  %d\n",xx->blit_hw);
   printf("Hardware->H CC Blits accellerated?  %d\n",xx->blit_hw_CC);
   printf("Hardware->H Alpha Blits accellerated?  %d\n",xx->blit_hw_A);
   printf("Software->H Blits accellerated?  %d\n",xx->blit_sw);
   printf("Software->H CC Blits accellerated?  %d\n",xx->blit_sw_CC);
   printf("Software->H Alpha Blits accellerated?  %d\n",xx->blit_sw_A);
   printf("Fills accellerated?  %d\n",xx->blit_fill);
}

sa_getmodes() {
   SDL_Rect **modes;
   int i;
   
//   modes=SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE);
   modes=SDL_ListModes(NULL,SDL_FULLSCREEN);
   /* Check if there are any modes available */
   if(modes == (SDL_Rect **)0) {
      printf("No modes available!\n");
      exit(-1);
   }
   
   /* Check if our resolution is restricted */
   if(modes == (SDL_Rect **)-1) {
      printf("All resolutions available.\n");
   } else {
      /* Print valid modes */
      printf("Available Modes\n");
      for(i=0;modes[i];++i)
	printf("  %d x %d\n", modes[i]->w, modes[i]->h);
   }
}

void sa_surface_info(SDL_Surface *asurf, char *name) {
   SDL_PixelFormat *fmt;
   fmt=asurf->format;
   
   printf("\n%s SDL Surface Info\n",name);
   printf("----------------------------------\n");
   printf("Bits Per Pixel: %d\n",fmt->BitsPerPixel);
   printf("colorkey: %d\n",fmt->colorkey);
   printf("alpha: %d\n",fmt->alpha);
   printf("flags: %d\n",asurf->flags);
   printf("clip: x:%d y:%d w:%d h:%d\n",asurf->clip_rect.x,asurf->clip_rect.y,asurf->clip_rect.w,asurf->clip_rect.h);
   printf("SDL_SRCALPHA: %d\n",SDL_SRCALPHA);
}

int allegro_init() {
   char driver[21];
   int rv;
//   printf("sdl_allegro version: %s  header: %s \n",SDL_ALLEGRO_VERS_C,SDL_ALLEGRO_VERS_H);
   SA_AUTOUPDATE=1;
   rv=SDL_Init(SDL_INIT_EVERYTHING);
   SDL_VideoDriverName(sdldriver,19);
//   SDL_VideoDriverName(driver,20);
#ifdef DEBUG
   printf("Video driver in use is %s\n",sdldriver);
#endif
#ifdef ZAURUS
   // Aparently, only the bvdd SDL suffers this problem
   if(strcmp(sdldriver,"bvdd")==0) {
      zcomp=1;
      printf("Enabling touchscreen compensation\n");
   }
#endif
//   By default, update rects will be done automatically
   atexit(SDL_Quit);
   return rv;
}


//
//  KEYBOARD SECTION
//
install_keyboard()  {
   SDL_EnableUNICODE(1);
//   printf("install_keyboard\n");
}

void remove_keyboard() {
}

void clear_keybuf() {
   SDL_Event event;
   while(SDL_PollEvent(&event)) 
     {
     }
   
}

void set_keyboard_rate(int delay, int repeat) {
   SDL_EnableKeyRepeat(delay,repeat);
}

simulate_keypress(int key) {
   SDL_Event event;
//   SDL_KeyboardEvent keyevent;
//   printf("GSK:%d\n",key); 
    
   event.type=SDL_KEYDOWN;
   event.key.type=SDL_KEYDOWN;
   event.key.state=SDL_PRESSED;
   event.key.keysym.sym=(key>>8);
   SDL_PushEvent(&event);
   event.type=SDL_KEYUP;
   event.key.type=SDL_KEYUP;
   event.key.state=SDL_RELEASED;
   SDL_PushEvent(&event);
}

int keypressed() {
   SDL_Event event;
   poll_mouse();
   if(SDL_PollEvent(&event) && event.type==SDL_KEYDOWN) {
      SDL_PushEvent(&event);
      //   if(SDL_PeepEvents(&event,1, SDL_PEEKEVENT, SDL_KEYDOWN)>0)
      return(1);
   } else
     return(0);
}

int readkey() {
   SDL_Event event;
   int key=0,key2;
//   SDL_WaitEvent(&event);
//   SDL_EnableUNICODE(1);
   while(SDL_PollEvent(&event)) {
//      printf("In event loop\n");
      if (event.type==SDL_KEYDOWN) {
	 key_shifts=0;
//	 mykeysym=event.key.keysym;
//	 myscancode=mykeysym.scancode;
	 key=event.key.keysym.sym;
//	 key2=event.key.keysym.sym;
	 key=key<<8;
//	 printf("UC: %d\n",event.key.keysym.unicode & 0x7f);
	 key=key+(event.key.keysym.unicode & 0x7F);
	 if(event.key.keysym.mod & KMOD_SHIFT) key_shifts=(key_shifts | KB_SHIFT_FLAG); 
	 if(event.key.keysym.mod & KMOD_CTRL) key_shifts=(key_shifts | KB_CTRL_FLAG); 
	 if(event.key.keysym.mod & KMOD_ALT) key_shifts=(key_shifts | KB_ALT_FLAG); 
	 if(event.key.keysym.mod & KMOD_LMETA) key_shifts=(key_shifts | KB_LWIN_FLAG); 
	 if(event.key.keysym.mod & KMOD_RMETA) key_shifts=(key_shifts | KB_RWIN_FLAG); 
	 if(event.key.keysym.mod & KMOD_NUM) key_shifts=(key_shifts | KB_NUMLOCK_FLAG); 
	 if(event.key.keysym.mod & KMOD_CAPS) key_shifts=(key_shifts | KB_CAPSLOCK_FLAG); 
	 if(event.key.keysym.mod & KMOD_MODE) key_shifts=(key_shifts | KB_MENU_FLAG); 
      }
      if (event.type==SDL_KEYUP) {
	 break;
      }
   }
//   SDL_EnableUNICODE(0);
   
   return(key);
      
}
// END Keyboard section

// Joystick section
//install_joystick(int type)  {
//   num_joysticks=SDL_NumJoysticks(void);
//   //   printf("install_joystick stub\n");
//}
// END Joystick section

set_color_depth(int depth)  {
//   printf("set_color_depth\n");
   sa_depth=depth;
}

//
// Mouse Section
//

install_mouse()  {
//   printf("install_mouse stub\n");
}
scare_mouse() {
//   printf("scare_mouse stub\n");
}
show_mouse(BITMAP *bmp) {
//   printf("scare_mouse stub\n");
}
unscare_mouse() {
//   printf("scare_mouse stub\n");
}

int mouse_needs_poll() {
   return(1);
}

int poll_mouse() {
   int const num = 32;
   SDL_Event event, events[num];
   mouse_b=0;
   SDL_PumpEvents();
   SDL_GetMouseState(&mouse_x,&mouse_y);
   int count, i;
#ifdef ZAURUS
   if(zcomp==1)
     mouse_y=mouse_y+33+(mouse_y/80);
#endif
   if(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))
     mouse_b=1;
   // I took this code off the internet
   count = SDL_PeepEvents(events, num, SDL_GETEVENT, SDL_EVENTMASK(SDL_MOUSEBUTTONDOWN));
   for( i = 0; i<count;++i) {
      if(events[i].button.button == SDL_BUTTON_WHEELUP)
	{ mouse_z--; }
      else if(events[i].button.button == SDL_BUTTON_WHEELDOWN)
	{ mouse_z++; }
   }
   //get the release events of the queue
   SDL_PeepEvents(events, num, SDL_GETEVENT, SDL_EVENTMASK(SDL_MOUSEBUTTONUP) );
   // End Internet code
   
//   if(SDL_PollEvent(&event) && event.type==SDL_MOUSEBUTTONDOWN) {
//      if(event.type!=SDL_MOUSEBUTTONDOWN && event.type!=SDL_MOUSEBUTTONUP) SDL_PushEvent(&event);
//      // Mouse Wheel
//      if (event.type==SDL_MOUSEBUTTONDOWN) {
//	 if(event.button.button & SDL_BUTTON_WHEELUP)
//	   mouse_z--;
//	 if(event.button.button & SDL_BUTTON_WHEELDOWN)
//	   mouse_z++;
//      }
////      if (event.type==SDL_MOUSEBUTTONUP)
////	break;
//}
   //     printf("Mouse Button 1(left) is pressed.\n");
}

// Graphics Section

int makecol(int r, int g, int b) {
   // Note: 8-bit would require doing a palette lookup and finding the
   // closest match, something that could be avoided in this day and age
   // 15-bit seems rather obscure, so only 16,24 and 32 bit are currently 
   // implemented
   int rv;

   switch(sa_depth) {
    case 16: rv=makecol16(r,g,b);
      break;
    case 24: rv=makecol24(r,g,b);
      break;
    case 32: rv=makecol32(r,g,b);
      break;
   }
   return(rv);
}

int makecol16(Uint8 r, Uint8 g, Uint8 b) {
   Uint32 pixel;
   pixel=SDL_MapRGB(screen->format,r,g,b);
   return(pixel);
}

int makecol24(Uint8 r, Uint8 g, Uint8 b) {
   Uint32 pixel;
   pixel=SDL_MapRGB(screen->format,r,g,b);
   return(pixel);
}

int makecol32(Uint8 r, Uint8 g, Uint8 b) {
   Uint32 pixel;
   pixel=SDL_MapRGB(screen->format,r,g,b);
   return(pixel);
}

int makeacol16(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
   Uint32 pixel;
   pixel=SDL_MapRGBA(screen->format,r,g,b,a);
//   printf("returning %d\n",pixel);
   return(pixel);
}

int makeacol24(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
   Uint32 pixel;
   pixel=SDL_MapRGBA(screen->format,r,g,b,a);
//   printf("returning %d\n",pixel);
   return(pixel);
}

int makeacol32(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
   Uint32 pixel;
   pixel=SDL_MapRGBA(screen->format,r,g,b,a);
//   printf("returning %d\n",pixel);
   return(pixel);
}

unsigned int makeacol(int r, int g, int b, int a) {
   // Note: 8-bit would require doing a palette lookup and finding the
   // closest match, something that could be avoided in this day and age
   // 15-bit seems rather obscure, so only 16,24 and 32 bit are currently 
   // implemented
   int rv;
   switch(sa_depth) {
    case 16: rv=makeacol16(r,g,b,a);
      break;
    case 24: rv=makeacol24(r,g,b,a);
      break;
    case 32: rv=makeacol32(r,g,b,a);
      break;
   }
//   printf("got makeacol: returning %d\n",rv);
   return(rv);
}



/* Modes */
int set_gfx_mode(int card, int w, int h, int v_w, int v_h) {
   Uint32 sdlflag,wasinit;
   int retval=0;
   
   /* Isn't this redundant?  See allegro_init */
//   if (SDL_Init(SDL_INIT_VIDEO) != 0) {
//      printf("Unable to initialize SDL: %s\n", SDL_GetError());
//      return 1;
//   }
   
   /* Start here */
   switch(card) {
    case GFX_AUTODETECT_WINDOWED:
      if (!(screen = SDL_SetVideoMode(w, h, sa_depth, SDL_ANYFORMAT|SDL_DOUBLEBUF)))
	retval=1;
      break;
    case GFX_AUTODETECT:
      if (!(screen = SDL_SetVideoMode(w, h, sa_depth, SDL_ANYFORMAT)))
	retval=1;
      break;
    case GFX_AUTODETECT_FULLSCREEN:
      if (!(screen = SDL_SetVideoMode(w, h, sa_depth, SDL_FULLSCREEN)))
//      if (!(screen = SDL_SetVideoMode(w, h, sa_depth, SDL_FULLSCREEN|SDL_HWSURFACE)))
	retval=1;
      break;
    case GFX_TEXT:
//      wasinit=SDL_WasInit(SDL_INIT_EVERYTHING);
//      if(subsystem_init&SDL_INIT_VIDEO) {
	 SDL_QuitSubSystem(SDL_INIT_VIDEO);
	 SDL_InitSubSystem(SDL_INIT_VIDEO);
//      }
      retval=1;
//      if(!SDL_WM_IconifyWindow())
//	retval=1;
      break;
   }
   if(retval!=1) {
      SCREEN_W=w; SCREEN_H=h;
      VIRTUAL_W=w; VIRTUAL_H=h;
//      SDL_SetAlpha(screen,0,255);
   }
   
   return(retval);
}

// Palette Routines
get_palette(PALETTE *p) {
//   printf("get_palette stub\n");
}

/* BITMAP routines */

SDL_Surface *create_bitmap(int width, int height) {
   SDL_Surface *new_surface, *xyz;
   Uint16 rmask,gmask,bmask,amask;
   
   #if SDL_BYTEORDER == SDL_BIG_ENDIAN
     rmask = 0xf000;
     gmask = 0x0f00;
     bmask = 0x00f0;
     amask = 0x000f;
   #else
     rmask = 0x000f;
     gmask = 0x00f0;
     bmask = 0x0f00;
     amask = 0xf000;
   #endif
   
   xyz = SDL_CreateRGBSurface(SDL_SWSURFACE,width,height,sa_depth,rmask,gmask,bmask,amask);
//   new_surface = SDL_CreateRGBSurface(SDL_SWSURFACE,width,height,sa_depth,rmask,gmask,bmask,amask);
   new_surface = SDL_DisplayFormat(xyz);
//     printf("created\n"); else printf("failed to convert surface\n") ;
   SDL_FreeSurface(xyz);
   SDL_SetAlpha(new_surface,0,255);
//   SDL_SetAlpha(new_surface,SDL_SRCALPHA,SDL_ALPHA_OPAQUE);
   return(new_surface);
}

destroy_bitmap(SDL_Surface *bmp) 
{
   SDL_FreeSurface(bmp);
}


blit(SDL_Surface *src, SDL_Surface *dest, int srx, int sry, int dsx, int dsy, int wdt, int hgt)  
{
   SDL_Rect srect, drect;
   
//   if(SDL_MUSTLOCK(dest))
//     if(SDL_LockSurface(dest) < 0) return;
   
   srect.x = srx;
   srect.y = sry;
   srect.w = wdt;
   srect.h = hgt;
   drect.x = dsx;
   drect.y = dsy;
   drect.w = wdt;
   drect.h = hgt;

//   printf("1call to blit\n");
   SDL_BlitSurface(src, &srect, dest, &drect);
//   printf("2call to blit\n");
   if(SA_AUTOUPDATE==1) {	
      if(dest == screen)
	SDL_UpdateRect(dest, dsx, dsy, wdt, hgt);
//      printf("SDL UP: %d %d %d %d \n",dsx,dsy,wdt,hgt);
   }
//     SDL_Flip(dest);
}

masked_blit(SDL_Surface *src, SDL_Surface *dest, int srx, int sry, int dsx, int dsy, int wdt, int hgt)  
{
   SDL_Rect srect, drect;
   Uint32 key;
// Test removal
   key=SDL_MapRGB(src->format,255,0,255);
   SDL_SetColorKey(src,SDL_SRCCOLORKEY,key);

   srect.x = srx;
   srect.y = sry;
   srect.w = wdt;
   srect.h = hgt;
   drect.x = dsx;
   drect.y = dsy;
   drect.w = wdt;
   drect.h = hgt;
   
   SDL_BlitSurface(src, &srect, dest, &drect);
//   printf("call to masked blit\n");
   if(SA_AUTOUPDATE==1) {	
      if(dest == screen)
	SDL_UpdateRect(dest, dsx, dsy, wdt, hgt);
   }
//     SDL_Flip(dest);
}

int show_video_bitmap(BITMAP *bitmap) {
   int i,x,y;
   // emulates routine used for double buffering
   // Since SDL has built-in double buffering, this just calls a flip
   // If allegro-compatible double buffering is needed, this may beed to
   // be rewritten
   
   // It may already be compatible,  the SDL docs aren't clear what happens
   // if you pass anything other than the screen surface to this:
   i=SDL_Flip(bitmap);   
   return(i);
}

// Audio Section
// Currently SDL_mixer is needed,  it may be possible to implement
// with straight SDL, but it would require a little more work.
#ifdef HAVE_LIBSDL_MIXER
int install_sound(int digi, int midi, const char *cfg_path) {
   Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024);
}

Mix_Chunk *load_sample(const char *filename) {
   Mix_Chunk *sampl=Mix_LoadWAV(filename);
   if(sampl==NULL) fprintf(stderr, "Couldn't load %s\n",filename);
   return(sampl);
}

SAMPLE *load_wav(const char *filename) {
   SAMPLE *sampl=Mix_LoadWAV(filename);
   if(sampl==NULL) fprintf(stderr, "Couldn't load %s\n",filename);
   return(sampl);
}
int play_sample(const Mix_Chunk *spl, Uint8 vol, Uint8 pan, Uint16 freq, int loop) { 
   int r;
   if(loop>0) loop=-1;
   Mix_VolumeChunk(spl, (vol/2));
   r=Mix_PlayChannel(-1, spl, loop);
   return(r);
}
#endif
// End Audio Section

// GUI section
int file_select_ex(const char *message, char *path, const char *ext, int size, int w, int h ) {
   printf("File selector not (yet) implemented\n");
}

// End GUI Section

// WM section
void set_window_title(const char *name) {
   SDL_WM_SetCaption(name, name);
}
// END WM section

