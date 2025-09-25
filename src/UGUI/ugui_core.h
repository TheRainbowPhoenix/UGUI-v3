#ifndef __UGUI_CORE_H
#define __UGUI_CORE_H

#include "ugui.h"



/* -------------------------------------------------------------------------------- */
/* -- µGUI CORE STRUCTURE                                                        -- */
/* -------------------------------------------------------------------------------- */
typedef struct
{
   void (*pset)(UG_S16,UG_S16,UG_COLOR);
   UG_S16 x_dim;
   UG_S16 y_dim;
   UG_TOUCH touch;
   UG_AREA clip;
   UG_WINDOW* next_window;
   UG_WINDOW* active_window;
   UG_WINDOW* last_window;
   struct
   {
      UG_S16 x_pos;
      UG_S16 y_pos;
      UG_S16 x_start;
      UG_S16 y_start;
      UG_S16 x_end;
      UG_S16 y_end;
      UG_COLOR fore_color;
      UG_COLOR back_color;
   } console;
   UG_FONT font;
   UG_S8 char_h_space;
   UG_S8 char_v_space;
   UG_COLOR fore_color;
   UG_COLOR back_color;
   UG_COLOR desktop_color;
   UG_U8 state;
   UG_DRIVER driver[NUMBER_OF_DRIVERS];
} UG_GUI;

#define UG_STATUS_WAIT_FOR_UPDATE                      (1<<0)


#endif