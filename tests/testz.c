#include <stdio.h>
#include <zlib.h>

main() {
   gzFile *fp;
   char l[256];
   fp = gzopen("testfile.txt.gz","rb");
   while(!gzeof(fp)) {
      gzgets(fp,l,256);
      printf("%s\n",l);
   }
   gzclose(fp);
   
}
