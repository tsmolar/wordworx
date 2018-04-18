#include <stdio.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <string.h>
#include "wwtypes.h"

// Path separators
#ifdef WIN32
   char mysep='\\';
#else
   char mysep='/';
#endif

abs_dirname(char *paath, char *bin) {
   
   // This mess will try to return the absolute pathname for any directory
   // passed.   absolute, meaning that .'s are translated.   The only things
   // not currently translated are ~ and ..
   // 
   char seps[4], path[160],strs[200], lcarp[40], *carp=NULL;
   
   strcpy(strs,bin);
   strcpy(seps,"/\\");
   strcpy(path,"");  // Clear path, seems to have problems if you don't

//   printf("bin: %s\n",bin);
   
   if(bin[0]==mysep) { 
      sprintf(path,"%c",mysep);
   }
   carp=strtok(strs,seps);
   
   while(carp != NULL) {
      //      printf("carp: %s  lcarp %s\n",carp,lcarp);
      strcpy(lcarp,carp);
      carp=strtok(NULL,seps);
      if(carp!=NULL) {
	 if(strcmp(lcarp,".")==0) {
	   getcwd(path,sizeof(path));
	    //	    	    printf("Why did I do this?\n");
	    //      sprintf(path,"%c",mysep);
	 } else {
#ifdef WIN32
	    if(bin[1]!=':') 
#else
	    if(path[0]!=mysep)
#endif
	      {
		  //	       printf("I'm doint this: %s\n");
		  getcwd(path,sizeof(path));
		 //		  printf("This might be the problem? %c\n");
	       } 
//	       printf("adding %s\n",lcarp);
#ifdef WIN32
	    if(strlen(path)<2) {
	       strcpy(path,lcarp);
#else
	    if(strcmp(path,"/")==0) {
	       sprintf(path,"/%s",lcarp);
#endif
	       } else
		 sprintf(path,"%s%c%s",path,mysep,lcarp);
	       //	       printf("path is now %s\n",path);
	 } 
      }
	 //      printf("G:%s%c\n",lcarp,mysep);
   }
//printf("\nend loop\n");
//   printf("WEHERE: %s\n",path);
   strcpy(paath,path);
}

df_basename(char *paath, char *bin) {
   
   // df_basename   
   char seps[4], strs[200], *carp=NULL;
   strcpy(strs,bin);
   strcpy(seps,"/\\");

   carp=strtok(strs,seps);
   
   while(carp != NULL) {
      carp=strtok(NULL,seps);
      if(carp!=NULL) {
	 strcpy(paath,carp);
      }
   }
}

find_datadir(char *ddir, char *bin) {
   char path[160],dirname[160];

   abs_dirname(path,bin);
   if(strcmp(path,"")==0) {
      strcpy(path,INSTPREFIX);
   }
//   printf("AB HERE: %s\n",path);
   df_basename(dirname,path);
//   printf("df_basename: %s  path:%s\n",dirname,path);
   if(strcmp(dirname,"bin")==0) {
      abs_dirname(dirname,path);
      sprintf(ddir,"%s%cshare%cwordworx",dirname,mysep,mysep);
   } else {
      sprintf(ddir,"%s%cshare%cwordworx",path,mysep,mysep);
   }
//   printf("999999 I want my dirname to be: %s\n",ddir);
}

