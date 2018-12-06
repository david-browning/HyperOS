#ifndef SNAKEH
#define SNAKEH

#include "annotations.h"
#include "kernel_procs.h"

enum direction { NW, N, NE, W, E, SW, S, SE };
#define NUMDIRS 8

typedef struct point_t
{
   int x;
   int y;
} sn_point;

typedef struct snake_st
{
   direction       dir;
   int             len;
   int             color;        /* color pair to use if colored snakes */
   sn_point        *body;
   proc_id_t        pid;       /* useful for playing with scheduling */
   struct snake_st *others;      /* a utility link to find all snakes again */
} *snake;

/* Colors range from 1 (blue on black) to 8 ( black on black).
 */
#define MAX_VISIBLE_SNAKE 7

extern "C"
extern int          start_windowing();

extern "C"
extern void         end_windowing();

extern "C"
extern snake        new_snake(int y, int x, int len, int dir, int color);

extern "C"
extern void         free_snake(snake s);

extern "C"
extern void         draw_all_snakes();

extern "C"
extern void         run_snake(void *s);

extern "C"
extern void         run_hungry_snake(void *s);

extern "C"
extern void         kill_snake();

extern "C"
extern unsigned int get_snake_delay();

extern "C"
extern void         set_snake_delay(unsigned int msec);

extern "C"
extern snake        snakeFromLWpid(int lw_pid);

extern "C"
extern proc_id_t setup_snakes(int);

#endif
