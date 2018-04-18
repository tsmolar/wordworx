// Note, ltrpile only needs to be 12x7,  I had to pad it on the right
// side b/c the ai was causing the right column to be wiped for some reason
// changing it to 14 fixed the problem.

char ltrpile[14][8],bckpile[14][8],bag[100];
char currword[14],mywords[256];
int num_inbag, num_inpile, cwidx, cwreturn[12];
int currscore, totscore,numrows;
char datadir[200],mysep;
char hsfile[255]; /* Hi Score file */
char vword[14]; /* Last Valid Word Found */
extern char fsmode, dqmode, gmute;
#ifdef USEGFX
extern int usex, usey, rx0, ry0, wordcount, WSmode;  // for alt screen params
#endif
