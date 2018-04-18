#include <stdio.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#if defined(HAVE_LIBZ) || defined(HAVE_LIBZDLL) 
#include<zlib.h>
#endif
#include <string.h>
#include "wwtypes.h"

/* Dictionary Check, see if words are valid */

int check_word(char *word) {
   int rv=0,i,wl;
   char pf[3],myword[11],dict[80],lb[15];
#if defined(HAVE_LIBZ) || defined(HAVE_LIBZDLL) 
   gzFile *fp;
#else
   FILE *fp;
#endif
//   printf("G AM HERE! %c\n",mysep);
   pf[0]=word[0]; pf[1]=word[1]; pf[2]=0;
//   printf("2G AM HERE! %c\n",mysep);
   for(i=2;i<strlen(word);i++)
     myword[i-2]=word[i];
//   printf("3g AM HERE! %c\n",mysep);
   myword[i-2]=0;
//   printf("4G AM HERE! %c\n",mysep);
   myword[i-1]=0;
//   printf("5G AM HERE! %c\n",mysep);

#if defined(HAVE_LIBZ) || defined(HAVE_LIBZDLL) 
//   printf("O AM HERE! %c\n",mysep);
//   sprintf(dict,"share/wordworx/zdict/dict_%s",pf);
   sprintf(dict,"%s%czdict%cdict_%s",datadir,mysep,mysep,pf);
//   printf("word is: %s\n",myword);
//   printf("dict file: %s\n",dict);
   fp=gzopen(dict,"rb");
//   printf("opened file: %s\n",dict);
   if(fp != NULL) {
//      wc=0;
	while(!gzeof(fp)) 
	{
	   gzgets(fp,lb,20);
//	   if(gzeof(fp)) break;
	   wl = strlen(lb);
//	   wc++;
	   if((0 < wl)&&(10==lb[wl-1]||13==lb[wl-1]))
	        lb[wl-1] = '\0';
	   if((0 < wl)&&(10==lb[wl-2]||13==lb[wl-2]))
	        lb[wl-2] = '\0';
	   
	   
//	   for(i=0;i<strlen(lb);i++)
//	     printf("%d ",lb[i]);
//	   printf("\n%s:%s  %d:%d(%d)\n",myword,lb,strlen(myword),strlen(lb),wl);
	   if(strcmp(myword,lb)==0)
	     rv=1;
	}
   }
//   printf("last word: %s count %d\n",lb,wc);
   gzclose(fp);
#else
   sprintf(dict,"%s%cdict%cdict_%s",datadir,mysep,mysep,pf);
//   printf("dict file: %s\n",dict);
   fp=fopen(dict,"r");
 //  if(fp==NULL) printf("Fileo no existo\n");
 //  printf("opened dict file: %s %d\n",dict,fp);
   if(fp!=NULL) {
      while(!feof(fp)) {
	 fgets(lb,20,fp);
//	   if(gzeof(fp)) break;
	 wl = strlen(lb);
	 if((0 < wl)&&(10==lb[wl-1]||13==lb[wl-1]))
	   lb[wl-1] = '\0';
	 if((0 < wl)&&(10==lb[wl-2]||13==lb[wl-2]))
	   lb[wl-2] = '\0';
	 
	 if(strcmp(myword,lb)==0)
	   rv=1;
      }
      fclose(fp);
   }
#endif
   return(rv);
}

//main(int argc, char **argv) {
//   int x;
//   x=check_word(argv[1]);
//   if(x==1)
//     printf("%s is valid!\n",argv[1]);
//   else
//     printf("can't use %s\n",argv[1]);
//}
