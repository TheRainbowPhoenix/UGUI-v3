#ifndef __UGUI_SLIDER_H
#define __UGUI_SLIDER_H

#include "ugui.h"

/* -------------------------------------------------------------------------------- */
/* -- SLIDER OBJECT                                                              -- */
/* -------------------------------------------------------------------------------- */

#if !defined(C_PAL_SLIDER_TRACK) || !defined(C_PAL_SLIDER_HANDLE)
#error "You must define a theme!"
#endif

/* Slider structure */
typedef struct
{
   UG_S32 min_value;
   UG_S32 max_value;
   UG_S32 current_value;
   UG_S32 tick_interval;
   UG_U8 state;
   UG_U8 style;
   UG_COLOR track_color;
   UG_COLOR handle_color;
   UG_COLOR tick_color;
   UG_COLOR alt_track_color;
   UG_COLOR alt_handle_color;
   UG_U8 handle_size;
   UG_AREA handle_area;
   UG_AREA prev_handle_area;
   UG_U8 orientation; /* 0 = horizontal, 1 = vertical */
}UG_SLIDER;

/* Object type */
#define OBJ_TYPE_SLIDER                               2

/* Default slider IDs */
#define SLD_ID_0                                      OBJ_ID_0
#define SLD_ID_1                                      OBJ_ID_1
#define SLD_ID_2                                      OBJ_ID_2
#define SLD_ID_3                                      OBJ_ID_3
#define SLD_ID_4                                      OBJ_ID_4
#define SLD_ID_5                                      OBJ_ID_5
#define SLD_ID_6                                      OBJ_ID_6
#define SLD_ID_7                                      OBJ_ID_7
#define SLD_ID_8                                      OBJ_ID_8
#define SLD_ID_9                                      OBJ_ID_9
#define SLD_ID_10                                     OBJ_ID_10
#define SLD_ID_11                                     OBJ_ID_11
#define SLD_ID_12                                     OBJ_ID_12
#define SLD_ID_13                                     OBJ_ID_13
#define SLD_ID_14                                     OBJ_ID_14
#define SLD_ID_15                                     OBJ_ID_15
#define SLD_ID_16                                     OBJ_ID_16
#define SLD_ID_17                                     OBJ_ID_17
#define SLD_ID_18                                     OBJ_ID_18
#define SLD_ID_19                                     OBJ_ID_19

/* Slider states */
#define SLD_STATE_RELEASED                            (0<<0)
#define SLD_STATE_PRESSED                             (1<<0)
#define SLD_STATE_DRAGGING                            (1<<1)

/* Slider style */
#define SLD_STYLE_2D                                  (0<<0)
#define SLD_STYLE_3D                                  (1<<0)
#define SLD_STYLE_NO_TICKS                            (1<<1)
#define SLD_STYLE_NO_TRACK                            (1<<2)
#define SLD_STYLE_SNAP_TO_TICKS                       (1<<3)
#define SLD_STYLE_USE_ALTERNATE_COLORS                (1<<4)
#define SLD_STYLE_VERTICAL                            (1<<5)

/* Slider events */
#define SLD_EVENT_VALUE_CHANGED                       OBJ_EVENT_CLICKED

/* Slider orientation */
#define SLD_ORIENTATION_HORIZONTAL                    0
#define SLD_ORIENTATION_VERTICAL                      1

/* Default handle size */
#define SLD_DEFAULT_HANDLE_SIZE                       12

/* Slider functions */
UG_RESULT UG_SliderCreate( UG_WINDOW* wnd, UG_SLIDER* sld, UG_U8 id, UG_S16 xs, UG_S16 ys, UG_S16 xe, UG_S16 ye );
UG_RESULT UG_SliderDelete( UG_WINDOW* wnd, UG_U8 id );
UG_RESULT UG_SliderShow( UG_WINDOW* wnd, UG_U8 id );
UG_RESULT UG_SliderHide( UG_WINDOW* wnd, UG_U8 id );
UG_RESULT UG_SliderSetTrackColor( UG_WINDOW* wnd, UG_U8 id, UG_COLOR color );
UG_RESULT UG_SliderSetHandleColor( UG_WINDOW* wnd, UG_U8 id, UG_COLOR color );
UG_RESULT UG_SliderSetTickColor( UG_WINDOW* wnd, UG_U8 id, UG_COLOR color );
UG_RESULT UG_SliderSetAlternateTrackColor( UG_WINDOW* wnd, UG_U8 id, UG_COLOR color );
UG_RESULT UG_SliderSetAlternateHandleColor( UG_WINDOW* wnd, UG_U8 id, UG_COLOR color );
UG_RESULT UG_SliderSetRange( UG_WINDOW* wnd, UG_U8 id, UG_S32 min, UG_S32 max );
UG_RESULT UG_SliderSetValue( UG_WINDOW* wnd, UG_U8 id, UG_S32 value );
UG_RESULT UG_SliderSetTickInterval( UG_WINDOW* wnd, UG_U8 id, UG_S32 interval );
UG_RESULT UG_SliderSetStyle( UG_WINDOW* wnd, UG_U8 id, UG_U8 style );
UG_RESULT UG_SliderSetHandleSize( UG_WINDOW* wnd, UG_U8 id, UG_U8 size );
UG_COLOR UG_SliderGetTrackColor( UG_WINDOW* wnd, UG_U8 id );
UG_COLOR UG_SliderGetHandleColor( UG_WINDOW* wnd, UG_U8 id );
UG_COLOR UG_SliderGetTickColor( UG_WINDOW* wnd, UG_U8 id );
UG_COLOR UG_SliderGetAlternateTrackColor( UG_WINDOW* wnd, UG_U8 id );
UG_COLOR UG_SliderGetAlternateHandleColor( UG_WINDOW* wnd, UG_U8 id );
UG_S32 UG_SliderGetMinValue( UG_WINDOW* wnd, UG_U8 id );
UG_S32 UG_SliderGetMaxValue( UG_WINDOW* wnd, UG_U8 id );
UG_S32 UG_SliderGetValue( UG_WINDOW* wnd, UG_U8 id );
UG_S32 UG_SliderGetTickInterval( UG_WINDOW* wnd, UG_U8 id );
UG_U8 UG_SliderGetStyle( UG_WINDOW* wnd, UG_U8 id );
UG_U8 UG_SliderGetHandleSize( UG_WINDOW* wnd, UG_U8 id );

#endif