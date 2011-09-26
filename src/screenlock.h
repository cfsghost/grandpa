#ifndef GRANDPA_SCREENLOCK_H
#define GRANDPA_SCREENLOCK_H

void gpa_screenlock_init(GrandPa *gpa, GPaScreen *screen);
void gpa_screen_enter(GrandPa *gpa);
void gpa_screen_leave(GrandPa *gpa);

#endif
