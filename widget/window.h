#ifndef WINDOW_H
#define WINDOW_H

#include "widget.h"
#define close_window pop_level

void new_window(int x1, int y1, int x2, int y2);
int wdg_alert(char *text,char *b1,char *b2,char *b3);
int status(char *text);
int progress(char *text, int closb, int num ,int max);
int close_status();

// If you want to create your own alert box,
extern int alert_button;
#endif /* window.h */
