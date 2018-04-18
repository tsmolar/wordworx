#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#define HSN_MAXRETRY 3

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

int hsn_numt;
extern int hs_numtbls;
extern hstable scoretbl[8];
// These are taken from the curl examples
struct MemoryStruct {
   char *memory;
   size_t size;
};

// char urlpfx[]="http://auckland1:28080/hs/scores.php";
// char tblnm[]="wwscore";
// char tmpfl[]="/tmp/hsdslog";
char urlpfx[200], tblnm[64], tmpfl[200], grpid[20];

void hsnet_setparms(char *urlp, char *tnm, char *tmpf) {
   // Set parameters
   strcpy(urlpfx,urlp);
   strcpy(tblnm,tnm);
   strcpy(tmpfl,tmpf);
}

void hsnet_trimr(char *in) {
   int wl;
   wl = strlen(in);
   if((0 < wl)&&(10==in[wl-1]||13==in[wl-1]))
     in[wl-1] = '\0';
   if((0 < wl)&&(10==in[wl-2]||13==in[wl-2]))
     in[wl-2] = '\0';
}

void hsnet_loadconf(char *conffile) {
   
   char wwst[210], pfx[10];
   FILE *hfp;
   
   if(hfp=fopen(conffile,"r")) {
#ifdef HSNET_DEBUG
       printf("tmp file :%s\n",tmpfl);
#endif
      do {
	 fgets(wwst,208,hfp);
	 hsnet_trimr(wwst);
	 hss_index(pfx,wwst,0,'>');
	 hss_index(wwst,wwst,1,'>');
	 if(strcmp(pfx,"<URL")==0 && strcmp(wwst,"") != 0) strcpy(urlpfx,wwst);
	 if(strcmp(pfx,"<TBL")==0 && strcmp(wwst,"") != 0) strcpy(tblnm,wwst);
	 if(strcmp(pfx,"<TMP")==0 && strcmp(wwst,"") != 0) strcpy(tmpfl,wwst);
	 if(strcmp(pfx,"<GID")==0 && strcmp(wwst,"") != 0) strcpy(grpid,wwst);
	 // printf("%s\n",wwst);
      } while (!feof(hfp));
      // printf("tmp file :%s\n",tmpfl);
   }
}

void *myrealloc(void *ptr, size_t size) {
   /* There might be a realloc() out there that doesn't like reallocing
    *   NULL pointers, so we take care of it here */
   if(ptr)
     return realloc(ptr, size);
   else
     return malloc(size);
}

size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data) {
   
   size_t realsize = size * nmemb;
   struct MemoryStruct *mem = (struct MemoryStruct *)data;
   mem->memory = (char *)myrealloc(mem->memory, mem->size + realsize + 1);
   if (mem->memory) {
      memcpy(&(mem->memory[mem->size]), ptr, realsize);
      mem->size += realsize;
      mem->memory[mem->size] = 0;
   }
   return realsize;
}
// End example code!


int hsnet_getid() {
   CURL *curl;
   char url[80];
   int sc,rtn;
   
   struct MemoryStruct chunk,smchnk;
   chunk.memory=NULL; /* we expect realloc(NULL, size) to work */
   chunk.size = 0;    /* no data at this point */

   sprintf(url,"%s?gtid=%s&gtmd=ni",urlpfx,tblnm);
#ifdef HSNET_DEBUG   
   printf("hsnet_getid() URL is %s\n",url);
#endif
   curl_global_init(CURL_GLOBAL_ALL);
   
   curl = curl_easy_init();
   if(curl) {
      curl_easy_setopt(curl, CURLOPT_URL, url);      
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
      //  res = curl_easy_perform(curl);
      sc=-1;
      for(rtn=0;rtn<HSN_MAXRETRY;rtn++) {
	 // printf("ATMPT #%d\n",rtn);
	 if(curl_easy_perform(curl)==0) {
	    sc=atoi(chunk.memory);
	    break;
	 }
      }
      curl_easy_cleanup(curl);
   }
   
   return sc;
}

int hsnet_fetchtables(char *gamename) {
   CURL *curl;
   char wst[50], wst2[50],url[80],tblst[120],mtblnm[20];
   int en,ti,sc,nt,tn,rtn;
//   CURLcode res;

   struct MemoryStruct chunk,smchnk;
   chunk.memory=NULL; /* we expect realloc(NULL, size) to work */
   chunk.size = 0;    /* no data at this point */
   
   hsn_numt=0;
   curl_global_init(CURL_GLOBAL_ALL);
   
   curl = curl_easy_init();
   if(curl) {
      // ******
      // * get number of tables
      sprintf(url,"%s?gtid=%s&gtmd=nt",urlpfx,gamename);
#ifdef HSNET_DEBUG
      printf("hsnet_fetchtables() (num tables) URL is %s\n",url);
#endif
      curl_easy_setopt(curl, CURLOPT_URL, url);      
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
      nt=0;
      for(rtn=0;rtn<HSN_MAXRETRY;rtn++) {
	 // printf("ATMPT #%d\n",rtn);
	 if(curl_easy_perform(curl)==0) {
	    nt=atoi(chunk.memory);
	    break;
	 }
      }
      // printf("Hmm, there seems to be %d tables\n",nt);
      // ******
      // * get list of tables
      sprintf(url,"%s?gtid=%s&gtmd=gt",urlpfx,gamename);
#ifdef HSNET_DEBUG
      printf("hsnet_fetchtables() (list of tables) URL is %s\n",url);
#endif
      curl_easy_setopt(curl, CURLOPT_URL, url);      
      chunk.memory=NULL; chunk.size = 0;    /* no data at this point */
      for(rtn=0;rtn<HSN_MAXRETRY;rtn++) {
	 // printf("ATMPT #%d\n",rtn);
	 if(curl_easy_perform(curl)==0) {
	    strcpy(tblst,chunk.memory);
	    break;
	    // printf("table list is %s\n",tblst);
	 }
      }
      // always cleanup 
      // curl_easy_cleanup(curl);

      for(tn=0;tn<nt;tn++) {
	 // ***
	 // * Get number of entries
	 hss_index(mtblnm,tblst,tn,'|');
	 sprintf(url,"%s?gtid=%s&gtmd=cn",urlpfx,mtblnm);
	 if(strcmp(grpid,"")!=0)
	   sprintf(url,"%s&pid=%s",url,grpid);
#ifdef HSNET_DEBUG
      printf("hsnet_fetchtables() (# of entries) URL is %s\n",url);
#endif
	 curl_easy_setopt(curl, CURLOPT_URL, url);
	 chunk.memory=NULL; chunk.size = 0;    /* no data at this point */
	 sc=0;
	 for(rtn=0;rtn<HSN_MAXRETRY;rtn++) {
	    // printf("ATMPT #%d\n",rtn);
	    if(curl_easy_perform(curl)==0) {
	       sc=atoi(chunk.memory);
	       break;
	    }
	 }
	 // printf("This table %s has %d entries",mtblnm,sc);
	 // ***
	 // * Get name of table
	 sprintf(url,"%s?gtid=%s&gtmd=nm",urlpfx,mtblnm);
#ifdef HSNET_DEBUG
      printf("hsnet_fetchtables() (name of tables) URL is %s\n",url);
#endif
	 curl_easy_setopt(curl, CURLOPT_URL, url);      
	 chunk.memory=NULL; chunk.size = 0;    /* no data at this point */
	 for(rtn=0;rtn<HSN_MAXRETRY;rtn++) {
	    // printf("ATMPT #%d\n",rtn);
	    if(curl_easy_perform(curl)==0) {
	       hs_numtbls++;
	       hsn_numt++;
	       ti=hs_numtbls;
	       strcpy(scoretbl[ti].desc,chunk.memory);
	       break;
	    }
	 }
	 
	 // ***
	 // * Get Table entries
	 sprintf(url,"%s?gtid=%s&limit=%d",urlpfx,mtblnm,sc);
	 if(strcmp(grpid,"")!=0)
	   sprintf(url,"%s&pid=%s",url,grpid);
	 // printf("fetch table with %s\n",url);
#ifdef HSNET_DEBUG
      printf("hsnet_fetchtables() (fetch table data) URL is %s group is %s\n",url,grpid);
#endif
	 curl_easy_setopt(curl, CURLOPT_URL, url);      
	 chunk.memory=NULL; chunk.size = 0;    /* no data at this point */
	 for(rtn=0;rtn<HSN_MAXRETRY;rtn++) {
	    // printf("ATMPT #%d\n",rtn);
	    if(curl_easy_perform(curl)==0)
	      break;
	 }
	 if(chunk.memory) {
	    scoretbl[ti].numscores=sc;
	    for(en=0;en<sc;en++) {
	       if(en<sc) {
		  // There are values in the table
		  hss_index(wst,chunk.memory,en,'|');
		  hss_index(wst2,wst,0,'~');
		  strcpy(scoretbl[ti].hscores[en+1].name,wst2);
		  hss_index(wst2,wst,1,'~');
		  strcpy(scoretbl[ti].hscores[en+1].detail,wst2);
		  hss_index(wst2,wst,2,'~');
		  strcpy(scoretbl[ti].hscores[en+1].date,wst2);
		  hss_index(wst2,wst,3,'~');
		  scoretbl[ti].hscores[en+1].score=atoi(wst2);
	       } else {
		  strcpy(scoretbl[ti].hscores[en+1].name,"");
		  strcpy(scoretbl[ti].hscores[en+1].detail,"");
		  strcpy(scoretbl[ti].hscores[en+1].date,"");
		  scoretbl[ti].hscores[en+1].score=0;
	       } // if(en<sc)
	    } // for(en=0;en<15;en++)
	 } // if(chunk.memory)
      } // for tn
      // always cleanup 
      curl_easy_cleanup(curl);
      free(chunk.memory);
   }
   return 0; 
}
   
hsnet_addscore(int hsid,char *name, char *detail, int score) {
   CURL *curl;
   CURLcode res;
   FILE *fp;
   char url[80], wst[40], wst2[40];
   int ckey, rtn;
   
   struct curl_httppost *formpost=NULL;
   struct curl_httppost *lastptr=NULL;
   struct curl_slist *headerlist=NULL;
   char buf[] = "Expect:";
    
   // printf("In addscore: %d '%s' '%s' %d\n",hsid,name,detail,score);
   if(hsid>-1) {
      curl_global_init(CURL_GLOBAL_ALL);
      /* Fill in the file upload field */
      // TODO: add ds (detail) field to form!
      // This should not need escapes unless encryption is added
      curl_formadd(&formpost,&lastptr,
		   CURLFORM_COPYNAME, "nm",
		   CURLFORM_COPYCONTENTS, name,CURLFORM_END);
      sprintf(wst,"%d",hsid);
      curl_formadd(&formpost,&lastptr,
		   CURLFORM_COPYNAME, "id",
		   CURLFORM_COPYCONTENTS, wst,CURLFORM_END);
      sprintf(wst,"%d",score);
      // printf("I am submitting score: %s ok, score: %d\n",wst,score);
      curl_formadd(&formpost,&lastptr,
		   CURLFORM_COPYNAME, "sc",
		   CURLFORM_COPYCONTENTS, wst,CURLFORM_END);
      curl = curl_easy_init();
      /* initalize custom header list (stating that Expect: 100-continue is not
       *  wanted */
      headerlist = curl_slist_append(headerlist, buf);
      if(curl) {
	 /* what URL that receives this POST */
	 sprintf(url,"%s?gtid=%s&gtmd=sc",urlpfx,tblnm);
#ifdef HSNET_DEBUG
      printf("hsnet_addscore() URL is %s\n",url);
#endif
	 curl_easy_setopt(curl, CURLOPT_URL, url);
	 //   if ( (argc == 2) && (!strcmp(argv[1], "noexpectheader")) )
	 /* only disable 100-continue header if explicitly requested */
	 //	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
	 curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
	 for(rtn=0;rtn<HSN_MAXRETRY;rtn++) {
	    // printf("ATMPT #%d\n",rtn);
	    res = curl_easy_perform(curl);
	    if(res==0) break;
	 }
	 
	 /* always cleanup */
	 curl_easy_cleanup(curl);
      
	 /* then cleanup the formpost chain */
	 curl_formfree(formpost);
	 /* free slist */
	 curl_slist_free_all (headerlist);
      }
   } else {
      // printf("Delayed submission in progress\n");
      if(fp=fopen(tmpfl, "a")) {
	 ckey=hs_random(12,22);
	 //ckey=14;
	 sprintf(wst2,"ATBL:%s",tblnm);  
	 hs_crypt(wst,wst2,"RO",3);
	 // printf("writing %s\n",wst);
	 fprintf(fp,"%s\n",wst);

	 sprintf(wst2,"A%s",name);  
	 hs_crypt(wst,wst2,"RO",ckey);
	 // printf("writing %s\n",wst2);
	 fprintf(fp,"%s\n",wst);  // Name
	 
	 sprintf(wst2,"A%s",detail);  
	 hs_crypt(wst,wst2,"RO",ckey);
	 // printf("writing %s\n",wst2);
	 fprintf(fp,"%s\n",wst);  // Detail

	 sprintf(wst2,"A%d",score);  
	 hs_crypt(wst,wst2,"RO",ckey);
	 // printf("writing %s\n",wst2);
	 fprintf(fp,"%s\n",wst);  // Score
	 fclose(fp);
      }
   }
//   return 0;
}

hsnet_adddata(int hsid,int docrypt, char *xdata) {
   CURL *curl;
   CURLcode res;
   FILE *fp;
   char url[80], wst[40],xxdata[200],wdata[200], *escstr;
   int ckey,rtn;
   
   struct curl_httppost *formpost=NULL;
   struct curl_httppost *lastptr=NULL;
   struct curl_slist *headerlist=NULL;
   char buf[] = "Expect:";
   // Should the A be added here?
   // printf("In adddata: %d  docrypt: %d\n",hsid, docrypt);
   sprintf(wdata,"A%s",xdata);
   if(docrypt==1) {
      ckey=hs_random(12,22);
      hs_crypt(xxdata,wdata,"RO",ckey);
   } else {
      strcpy(xxdata,xdata);
   }
	
   if(hsid>-1) {
      curl_global_init(CURL_GLOBAL_ALL);
      // formadds were here
      curl = curl_easy_init();
      /* initalize custom header list (stating that Expect: 100-continue is not
       *  wanted */
      headerlist = curl_slist_append(headerlist, buf);
      if(curl) {
	 /* Fill in the file upload field */
//	 escstr=curl_easy_escape(curl,xxdata,0);
	 escstr=curl_escape(xxdata,0);
	 curl_formadd(&formpost,&lastptr,
		      CURLFORM_COPYNAME, "xd",
		      CURLFORM_COPYCONTENTS, escstr,CURLFORM_END);
	 curl_free(escstr); // Should this be moved until after the upload?
	 sprintf(wst,"%d",hsid);
	 curl_formadd(&formpost,&lastptr,
		      CURLFORM_COPYNAME, "id",
		      CURLFORM_COPYCONTENTS, wst,CURLFORM_END);
	
	 /* what URL that receives this POST */
	 sprintf(url,"%s?gtid=%s&gtmd=xd",urlpfx,tblnm);
	 curl_easy_setopt(curl, CURLOPT_URL, url);
	 //   if ( (argc == 2) && (!strcmp(argv[1], "noexpectheader")) )
	 /* only disable 100-continue header if explicitly requested */
	 //	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
	 curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
      	 for(rtn=0;rtn<HSN_MAXRETRY;rtn++) {
	    // printf("ATMPT #%d\n",rtn);
	    res = curl_easy_perform(curl);
	    if(res==0) break;
	 }

	 /* always cleanup */
	 curl_easy_cleanup(curl);
      
	 /* then cleanup the formpost chain */
	 curl_formfree(formpost);
	 /* free slist */
	 curl_slist_free_all (headerlist);
      }
   } else {
//      printf("Delayed submission wip\n");
      if(fp=fopen(tmpfl, "a")) {
	 fprintf(fp,"%s\n",xxdata);
	 fclose(fp);
      }
   }
//   return 0;
}

int hsnet_submit(int hsid) {
   // Finalize Submission
   CURL *curl;
   char url[80];
   int sc,res,rtn;
   
   struct MemoryStruct chunk,smchnk;
   chunk.memory=NULL; /* we expect realloc(NULL, size) to work */
   chunk.size = 0;    /* no data at this point */

   if(hsid>-1) {
      sprintf(url,"%s?gtid=%s&gtmd=ps&id=%d",urlpfx,tblnm,hsid);
      if(strcmp(grpid,"")!=0)
	sprintf(url,"%s&pid=%s",url,grpid);
      // printf("submit URL is %s\n",url);
      curl_global_init(CURL_GLOBAL_ALL);
   
      curl = curl_easy_init();
      if(curl) {
	 curl_easy_setopt(curl, CURLOPT_URL, url);      
	 curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	 curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
	 // May need better checking here
	 for(rtn=0;rtn<HSN_MAXRETRY;rtn++) {
	    // printf("ATMPT #%d\n",rtn);
	    res = curl_easy_perform(curl);
	    if(res==0) break;
	 }
#ifdef HSNET_DEBUG
	 if(res==0) {
	 //	    // always cleanup 
	    printf("Results: %s\n",chunk.memory);
	 } else
	   printf("submission problem\n");
#endif
	 curl_easy_cleanup(curl);
      }
   }
   // Unload hsn tables, may not be best place for this
   hs_numtbls=hs_numtbls-hsn_numt;
   return sc;
}

int hsnet_chkdelayed() {
   FILE *fp;
   char aln[255];
   int hsid=-1;
   
   if(fp=fopen(tmpfl,"r")) {
      while(!feof(fp)) {
	 fgets(aln,253,fp);
	 if(feof(fp)) break;
	 // Note, depends on a key of 3!!!
	 if(strncmp(aln,"DWEO=",5)==0) {
	    if(hsid>0) hsnet_submit(hsid);
	    hsid=hsnet_getid();
	    if(hsid<0) break;
//	    printf("\nGot ID: %d\n\n",hsid);
	 }
	 // printf("ohoh: calling adddata%s\n",aln);
	 hsnet_adddata(hsid,0,aln);
      }
      
      fclose(fp);
      if(hsid>0) {
	 hsnet_submit(hsid);
	 remove(tmpfl);
      }
      // printf("HOW TO DELAY?  I HAVE WORK TO DO!\n");
   }
}
