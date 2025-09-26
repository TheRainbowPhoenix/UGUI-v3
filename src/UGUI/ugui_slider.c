#include "ugui_slider.h"
#include "ugui.h"

static void _UG_SliderUpdate(UG_WINDOW* wnd, UG_OBJECT* obj);
static void _UG_SliderCalculateHandlePosition(UG_SLIDER* sld, UG_OBJECT* obj);
static UG_S32 _UG_SliderPositionToValue(UG_SLIDER* sld, UG_OBJECT* obj, UG_S16 pos);
static UG_S16 _UG_SliderValueToPosition(UG_SLIDER* sld, UG_OBJECT* obj, UG_S32 value);
static UG_U8 _UG_SliderPointInHandle(UG_SLIDER* sld, UG_S16 x, UG_S16 y);

const UG_COLOR pal_slider_track[] = {
    C_PAL_SLIDER_TRACK
};

const UG_COLOR pal_slider_handle[] = {
    C_PAL_SLIDER_HANDLE
};

/* -------------------------------------------------------------------------------- */
/* -- SLIDER FUNCTIONS                                                           -- */
/* -------------------------------------------------------------------------------- */
UG_RESULT UG_SliderCreate( UG_WINDOW* wnd, UG_SLIDER* sld, UG_U8 id, UG_S16 xs, UG_S16 ys, UG_S16 xe, UG_S16 ye )
{
   UG_OBJECT* obj;

   obj = _UG_GetFreeObject( wnd );
   if ( obj == NULL ) return UG_RESULT_FAIL;

   /* Initialize object-specific parameters */
   sld->min_value = 0;
   sld->max_value = 100;
   sld->current_value = 0;
   sld->tick_interval = 10;
   sld->state = SLD_STATE_RELEASED;
   sld->track_color = wnd->bc;
   sld->handle_color = wnd->fc;
   sld->tick_color = wnd->fc;
   sld->alt_track_color = wnd->bc;
   sld->alt_handle_color = wnd->fc;
   sld->style = SLD_STYLE_3D;
   sld->handle_size = SLD_DEFAULT_HANDLE_SIZE;
   
   /* Initialize handle areas */
   sld->handle_area.xs = 0;
   sld->handle_area.ys = 0;
   sld->handle_area.xe = 0;
   sld->handle_area.ye = 0;
   sld->prev_handle_area = sld->handle_area;
   
   /* Determine orientation based on dimensions */
   if ((xe - xs) > (ye - ys)) {
       sld->orientation = SLD_ORIENTATION_HORIZONTAL;
   } else {
       sld->orientation = SLD_ORIENTATION_VERTICAL;
       sld->style |= SLD_STYLE_VERTICAL;
   }

   /* Initialize standard object parameters */
   obj->update = _UG_SliderUpdate;
   obj->touch_state = OBJ_TOUCH_STATE_INIT;
   obj->type = OBJ_TYPE_SLIDER;
   obj->event = OBJ_EVENT_NONE;
   obj->a_rel.xs = xs;
   obj->a_rel.ys = ys;
   obj->a_rel.xe = xe;
   obj->a_rel.ye = ye;
   obj->a_abs.xs = -1;
   obj->a_abs.ys = -1;
   obj->a_abs.xe = -1;
   obj->a_abs.ye = -1;
   obj->id = id;
   obj->state |= OBJ_STATE_VISIBLE | OBJ_STATE_REDRAW | OBJ_STATE_VALID | OBJ_STATE_TOUCH_ENABLE;
   obj->data = (void*)sld;

   /* Calculate initial handle position */
   _UG_SliderCalculateHandlePosition(sld, obj);

   /* Update function: Do your thing! */
   obj->state &= ~OBJ_STATE_FREE;

   return UG_RESULT_OK;
}

UG_RESULT UG_SliderDelete( UG_WINDOW* wnd, UG_U8 id )
{
   return _UG_DeleteObject( wnd, OBJ_TYPE_SLIDER, id );
}

UG_RESULT UG_SliderShow( UG_WINDOW* wnd, UG_U8 id )
{
   UG_OBJECT* obj=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_SLIDER, id );
   if ( obj == NULL ) return UG_RESULT_FAIL;

   obj->state |= OBJ_STATE_VISIBLE;
   obj->state |= OBJ_STATE_UPDATE | OBJ_STATE_REDRAW;

   return UG_RESULT_OK;
}

UG_RESULT UG_SliderHide( UG_WINDOW* wnd, UG_U8 id )
{
   UG_OBJECT* obj=NULL;
   UG_SLIDER* sld=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_SLIDER, id );
   if ( obj == NULL ) return UG_RESULT_FAIL;

   sld = (UG_SLIDER*)(obj->data);

   sld->state = SLD_STATE_RELEASED;
   obj->touch_state = OBJ_TOUCH_STATE_INIT;
   obj->event = OBJ_EVENT_NONE;
   obj->state &= ~OBJ_STATE_VISIBLE;
   obj->state |= OBJ_STATE_UPDATE;

   return UG_RESULT_OK;
}

UG_RESULT UG_SliderSetTrackColor( UG_WINDOW* wnd, UG_U8 id, UG_COLOR color )
{
   UG_OBJECT* obj=NULL;
   UG_SLIDER* sld=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_SLIDER, id );
   if ( obj == NULL ) return UG_RESULT_FAIL;

   sld = (UG_SLIDER*)(obj->data);
   sld->track_color = color;
   obj->state |= OBJ_STATE_UPDATE | OBJ_STATE_REDRAW;

   return UG_RESULT_OK;
}

UG_RESULT UG_SliderSetHandleColor( UG_WINDOW* wnd, UG_U8 id, UG_COLOR color )
{
   UG_OBJECT* obj=NULL;
   UG_SLIDER* sld=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_SLIDER, id );
   if ( obj == NULL ) return UG_RESULT_FAIL;

   sld = (UG_SLIDER*)(obj->data);
   sld->handle_color = color;
   obj->state |= OBJ_STATE_UPDATE | OBJ_STATE_REDRAW;

   return UG_RESULT_OK;
}

UG_RESULT UG_SliderSetTickColor( UG_WINDOW* wnd, UG_U8 id, UG_COLOR color )
{
   UG_OBJECT* obj=NULL;
   UG_SLIDER* sld=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_SLIDER, id );
   if ( obj == NULL ) return UG_RESULT_FAIL;

   sld = (UG_SLIDER*)(obj->data);
   sld->tick_color = color;
   obj->state |= OBJ_STATE_UPDATE | OBJ_STATE_REDRAW;

   return UG_RESULT_OK;
}

UG_RESULT UG_SliderSetAlternateTrackColor( UG_WINDOW* wnd, UG_U8 id, UG_COLOR color )
{
   UG_OBJECT* obj=NULL;
   UG_SLIDER* sld=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_SLIDER, id );
   if ( obj == NULL ) return UG_RESULT_FAIL;

   sld = (UG_SLIDER*)(obj->data);
   sld->alt_track_color = color;
   obj->state |= OBJ_STATE_UPDATE | OBJ_STATE_REDRAW;

   return UG_RESULT_OK;
}

UG_RESULT UG_SliderSetAlternateHandleColor( UG_WINDOW* wnd, UG_U8 id, UG_COLOR color )
{
   UG_OBJECT* obj=NULL;
   UG_SLIDER* sld=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_SLIDER, id );
   if ( obj == NULL ) return UG_RESULT_FAIL;

   sld = (UG_SLIDER*)(obj->data);
   sld->alt_handle_color = color;
   obj->state |= OBJ_STATE_UPDATE | OBJ_STATE_REDRAW;

   return UG_RESULT_OK;
}

UG_RESULT UG_SliderSetRange( UG_WINDOW* wnd, UG_U8 id, UG_S32 min, UG_S32 max )
{
   UG_OBJECT* obj=NULL;
   UG_SLIDER* sld=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_SLIDER, id );
   if ( obj == NULL ) return UG_RESULT_FAIL;

   sld = (UG_SLIDER*)(obj->data);
   if (min >= max) return UG_RESULT_FAIL;
   
   sld->min_value = min;
   sld->max_value = max;
   
   /* Clamp current value to new range */
   if (sld->current_value < min) sld->current_value = min;
   if (sld->current_value > max) sld->current_value = max;
   
   _UG_SliderCalculateHandlePosition(sld, obj);
   obj->state |= OBJ_STATE_UPDATE | OBJ_STATE_REDRAW;

   return UG_RESULT_OK;
}

UG_RESULT UG_SliderSetValue( UG_WINDOW* wnd, UG_U8 id, UG_S32 value )
{
   UG_OBJECT* obj=NULL;
   UG_SLIDER* sld=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_SLIDER, id );
   if ( obj == NULL ) return UG_RESULT_FAIL;

   sld = (UG_SLIDER*)(obj->data);
   
   /* Clamp value to range */
   if (value < sld->min_value) value = sld->min_value;
   if (value > sld->max_value) value = sld->max_value;
   
   sld->current_value = value;
   _UG_SliderCalculateHandlePosition(sld, obj);
   obj->state |= OBJ_STATE_UPDATE | OBJ_STATE_REDRAW;

   return UG_RESULT_OK;
}

UG_RESULT UG_SliderSetTickInterval( UG_WINDOW* wnd, UG_U8 id, UG_S32 interval )
{
   UG_OBJECT* obj=NULL;
   UG_SLIDER* sld=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_SLIDER, id );
   if ( obj == NULL ) return UG_RESULT_FAIL;

   sld = (UG_SLIDER*)(obj->data);
   sld->tick_interval = interval;
   obj->state |= OBJ_STATE_UPDATE | OBJ_STATE_REDRAW;

   return UG_RESULT_OK;
}

UG_RESULT UG_SliderSetStyle( UG_WINDOW* wnd, UG_U8 id, UG_U8 style )
{
   UG_OBJECT* obj=NULL;
   UG_SLIDER* sld=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_SLIDER, id );
   if ( obj == NULL ) return UG_RESULT_FAIL;

   sld = (UG_SLIDER*)(obj->data);
   sld->style = style;
   
   /* Update orientation based on style */
   if (style & SLD_STYLE_VERTICAL) {
       sld->orientation = SLD_ORIENTATION_VERTICAL;
   } else {
       sld->orientation = SLD_ORIENTATION_HORIZONTAL;
   }
   
   _UG_SliderCalculateHandlePosition(sld, obj);
   obj->state |= OBJ_STATE_UPDATE | OBJ_STATE_REDRAW;

   return UG_RESULT_OK;
}

UG_RESULT UG_SliderSetHandleSize( UG_WINDOW* wnd, UG_U8 id, UG_U8 size )
{
   UG_OBJECT* obj=NULL;
   UG_SLIDER* sld=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_SLIDER, id );
   if ( obj == NULL ) return UG_RESULT_FAIL;

   sld = (UG_SLIDER*)(obj->data);
   sld->handle_size = size;
   _UG_SliderCalculateHandlePosition(sld, obj);
   obj->state |= OBJ_STATE_UPDATE | OBJ_STATE_REDRAW;

   return UG_RESULT_OK;
}

/* Getter functions */
UG_COLOR UG_SliderGetTrackColor( UG_WINDOW* wnd, UG_U8 id )
{
   UG_OBJECT* obj=NULL;
   UG_SLIDER* sld=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_SLIDER, id );
   if ( obj != NULL )
   {
      sld = (UG_SLIDER*)(obj->data);
      return sld->track_color;
   }
   return C_BLACK;
}

UG_COLOR UG_SliderGetHandleColor( UG_WINDOW* wnd, UG_U8 id )
{
   UG_OBJECT* obj=NULL;
   UG_SLIDER* sld=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_SLIDER, id );
   if ( obj != NULL )
   {
      sld = (UG_SLIDER*)(obj->data);
      return sld->handle_color;
   }
   return C_BLACK;
}

UG_COLOR UG_SliderGetTickColor( UG_WINDOW* wnd, UG_U8 id )
{
   UG_OBJECT* obj=NULL;
   UG_SLIDER* sld=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_SLIDER, id );
   if ( obj != NULL )
   {
      sld = (UG_SLIDER*)(obj->data);
      return sld->tick_color;
   }
   return C_BLACK;
}

UG_COLOR UG_SliderGetAlternateTrackColor( UG_WINDOW* wnd, UG_U8 id )
{
   UG_OBJECT* obj=NULL;
   UG_SLIDER* sld=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_SLIDER, id );
   if ( obj != NULL )
   {
      sld = (UG_SLIDER*)(obj->data);
      return sld->alt_track_color;
   }
   return C_BLACK;
}

UG_COLOR UG_SliderGetAlternateHandleColor( UG_WINDOW* wnd, UG_U8 id )
{
   UG_OBJECT* obj=NULL;
   UG_SLIDER* sld=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_SLIDER, id );
   if ( obj != NULL )
   {
      sld = (UG_SLIDER*)(obj->data);
      return sld->alt_handle_color;
   }
   return C_BLACK;
}

UG_S32 UG_SliderGetMinValue( UG_WINDOW* wnd, UG_U8 id )
{
   UG_OBJECT* obj=NULL;
   UG_SLIDER* sld=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_SLIDER, id );
   if ( obj != NULL )
   {
      sld = (UG_SLIDER*)(obj->data);
      return sld->min_value;
   }
   return 0;
}

UG_S32 UG_SliderGetMaxValue( UG_WINDOW* wnd, UG_U8 id )
{
   UG_OBJECT* obj=NULL;
   UG_SLIDER* sld=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_SLIDER, id );
   if ( obj != NULL )
   {
      sld = (UG_SLIDER*)(obj->data);
      return sld->max_value;
   }
   return 0;
}

UG_S32 UG_SliderGetValue( UG_WINDOW* wnd, UG_U8 id )
{
   UG_OBJECT* obj=NULL;
   UG_SLIDER* sld=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_SLIDER, id );
   if ( obj != NULL )
   {
      sld = (UG_SLIDER*)(obj->data);
      return sld->current_value;
   }
   return 0;
}

UG_S32 UG_SliderGetTickInterval( UG_WINDOW* wnd, UG_U8 id )
{
   UG_OBJECT* obj=NULL;
   UG_SLIDER* sld=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_SLIDER, id );
   if ( obj != NULL )
   {
      sld = (UG_SLIDER*)(obj->data);
      return sld->tick_interval;
   }
   return 0;
}

UG_U8 UG_SliderGetStyle( UG_WINDOW* wnd, UG_U8 id )
{
   UG_OBJECT* obj=NULL;
   UG_SLIDER* sld=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_SLIDER, id );
   if ( obj != NULL )
   {
      sld = (UG_SLIDER*)(obj->data);
      return sld->style;
   }
   return 0;
}

UG_U8 UG_SliderGetHandleSize( UG_WINDOW* wnd, UG_U8 id )
{
   UG_OBJECT* obj=NULL;
   UG_SLIDER* sld=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_SLIDER, id );
   if ( obj != NULL )
   {
      sld = (UG_SLIDER*)(obj->data);
      return sld->handle_size;
   }
   return 0;
}

/* -------------------------------------------------------------------------------- */
/* -- INTERNAL SLIDER FUNCTIONS                                                  -- */
/* -------------------------------------------------------------------------------- */

static void _UG_SliderCalculateHandlePosition(UG_SLIDER* sld, UG_OBJECT* obj)
{
   UG_S16 track_length;
   UG_S16 handle_pos;
   UG_S16 handle_half_size = sld->handle_size / 2;
   
   /* Save previous handle position for clearing */
   sld->prev_handle_area = sld->handle_area;
   
   if (sld->orientation == SLD_ORIENTATION_HORIZONTAL) 
   {
      track_length = obj->a_abs.xe - obj->a_abs.xs - sld->handle_size;
      if (sld->max_value > sld->min_value) {
         handle_pos = obj->a_abs.xs + handle_half_size + 
                     (track_length * (sld->current_value - sld->min_value)) / 
                     (sld->max_value - sld->min_value);
      } else {
         handle_pos = obj->a_abs.xs + handle_half_size;
      }
      
      sld->handle_area.xs = handle_pos - handle_half_size;
      sld->handle_area.xe = handle_pos + handle_half_size;
      sld->handle_area.ys = obj->a_abs.ys;
      sld->handle_area.ye = obj->a_abs.ye;
   } 
   else 
   {
      track_length = obj->a_abs.ye - obj->a_abs.ys - sld->handle_size;
      if (sld->max_value > sld->min_value) {
         handle_pos = obj->a_abs.ye - handle_half_size - 
                     (track_length * (sld->current_value - sld->min_value)) / 
                     (sld->max_value - sld->min_value);
      } else {
         handle_pos = obj->a_abs.ye - handle_half_size;
      }
      
      sld->handle_area.ys = handle_pos - handle_half_size;
      sld->handle_area.ye = handle_pos + handle_half_size;
      sld->handle_area.xs = obj->a_abs.xs;
      sld->handle_area.xe = obj->a_abs.xe;
   }
}

static UG_S32 _UG_SliderPositionToValue(UG_SLIDER* sld, UG_OBJECT* obj, UG_S16 pos)
{
   UG_S32 value;
   UG_S16 track_length;
   UG_S16 handle_half_size = sld->handle_size / 2;
   
   if (sld->orientation == SLD_ORIENTATION_HORIZONTAL) 
   {
      track_length = obj->a_abs.xe - obj->a_abs.xs - sld->handle_size;
      pos -= (obj->a_abs.xs + handle_half_size);
      if (pos < 0) pos = 0;
      if (pos > track_length) pos = track_length;
      
      if (track_length > 0) {
         value = sld->min_value + ((sld->max_value - sld->min_value) * pos) / track_length;
      } else {
         value = sld->min_value;
      }
   }
   
   /* Snap to tick marks if enabled */
   if ((sld->style & SLD_STYLE_SNAP_TO_TICKS) && (sld->tick_interval > 0)) {
      UG_S32 tick_value = ((value - sld->min_value) / sld->tick_interval) * sld->tick_interval + sld->min_value;
      UG_S32 next_tick = tick_value + sld->tick_interval;
      
      if ((value - tick_value) > (next_tick - value) && next_tick <= sld->max_value) {
         value = next_tick;
      } else {
         value = tick_value;
      }
   }
   
   return value;
}

static UG_S16 _UG_SliderValueToPosition(UG_SLIDER* sld, UG_OBJECT* obj, UG_S32 value)
{
   UG_S16 pos;
   UG_S16 track_length;
   UG_S16 handle_half_size = sld->handle_size / 2;
   
   if (sld->orientation == SLD_ORIENTATION_HORIZONTAL) 
   {
      track_length = obj->a_abs.xe - obj->a_abs.xs - sld->handle_size;
      if (sld->max_value > sld->min_value) {
         pos = obj->a_abs.xs + handle_half_size + 
               (track_length * (value - sld->min_value)) / (sld->max_value - sld->min_value);
      } else {
         pos = obj->a_abs.xs + handle_half_size;
      }
   } 
   else 
   {
      track_length = obj->a_abs.ye - obj->a_abs.ys - sld->handle_size;
      if (sld->max_value > sld->min_value) {
         pos = obj->a_abs.ye - handle_half_size - 
               (track_length * (value - sld->min_value)) / (sld->max_value - sld->min_value);
      } else {
         pos = obj->a_abs.ye - handle_half_size;
      }
   }
   
   return pos;
}

static UG_U8 _UG_SliderPointInHandle(UG_SLIDER* sld, UG_S16 x, UG_S16 y)
{
   return (x >= sld->handle_area.xs && x <= sld->handle_area.xe &&
           y >= sld->handle_area.ys && y <= sld->handle_area.ye);
}

void _UG_SliderUpdate(UG_WINDOW* wnd, UG_OBJECT* obj)
{
   UG_SLIDER* sld;
   UG_AREA a;
   UG_S32 old_value;
   UG_S32 new_value;
   UG_S16 touch_x, touch_y;

   /* Get object-specific data */
   sld = (UG_SLIDER*)(obj->data);

   /* -------------------------------------------------- */
   /* Object touch section                               */
   /* -------------------------------------------------- */
   if ( (obj->touch_state & OBJ_TOUCH_STATE_CHANGED) )
   {
      /* Get touch coordinates */
      touch_x = UG_GetGUI()->touch.xp;
      touch_y = UG_GetGUI()->touch.yp;
      
      old_value = sld->current_value;
      
      /* Handle click on slider */
      if ( obj->touch_state & OBJ_TOUCH_STATE_CLICK_ON_OBJECT )
      {
         if (_UG_SliderPointInHandle(sld, touch_x, touch_y)) {
            sld->state |= SLD_STATE_PRESSED | SLD_STATE_DRAGGING;
         } else {
            /* Click on track - jump to position */
            if (sld->orientation == SLD_ORIENTATION_HORIZONTAL) {
               new_value = _UG_SliderPositionToValue(sld, obj, touch_x);
            } else {
               new_value = _UG_SliderPositionToValue(sld, obj, touch_y);
            }
            
            if (new_value != old_value) {
               sld->current_value = new_value;
               _UG_SliderCalculateHandlePosition(sld, obj);
               obj->event = SLD_EVENT_VALUE_CHANGED;
               obj->state |= OBJ_STATE_UPDATE;
            }
         }
      }
      
      /* Is the handle pressed down and being dragged? */
      if ( (obj->touch_state & OBJ_TOUCH_STATE_PRESSED_ON_OBJECT) && 
           (sld->state & SLD_STATE_DRAGGING) )
      {
         if (sld->orientation == SLD_ORIENTATION_HORIZONTAL) {
            new_value = _UG_SliderPositionToValue(sld, obj, touch_x);
         } else {
            new_value = _UG_SliderPositionToValue(sld, obj, touch_y);
         }
         
         if (new_value != old_value) {
            sld->current_value = new_value;
            _UG_SliderCalculateHandlePosition(sld, obj);
            obj->event = SLD_EVENT_VALUE_CHANGED;
            obj->state |= OBJ_STATE_UPDATE;
         }
         obj->event = OBJ_EVENT_PRESSED;
      }
      /* Can we release the handle? */
      else if ( sld->state & (SLD_STATE_PRESSED | SLD_STATE_DRAGGING) )
      {
         sld->state = SLD_STATE_RELEASED;
         obj->state |= OBJ_STATE_UPDATE;
         obj->event = OBJ_EVENT_RELEASED;
      }
      obj->touch_state &= ~OBJ_TOUCH_STATE_CHANGED;
   }

   /* -------------------------------------------------- */
   /* Object update section                              */
   /* -------------------------------------------------- */
   if ( obj->state & OBJ_STATE_UPDATE )
   {
      if ( obj->state & OBJ_STATE_VISIBLE )
      {
         /* Calculate absolute coordinates */
         if ( obj->state & OBJ_STATE_REDRAW )
         {
            UG_WindowGetArea(wnd,&a);
            obj->a_abs.xs = obj->a_rel.xs + a.xs;
            obj->a_abs.ys = obj->a_rel.ys + a.ys;
            obj->a_abs.xe = obj->a_rel.xe + a.xs;
            obj->a_abs.ye = obj->a_rel.ye + a.ys;
            if ( obj->a_abs.ye > wnd->ye ) return;
            if ( obj->a_abs.xe > wnd->xe ) return;
            
            _UG_SliderCalculateHandlePosition(sld, obj);
            
#ifdef UGUI_USE_PRERENDER_EVENT
            _UG_SendObjectPrerenderEvent(wnd, obj);
#endif

            /* Draw slider track */
            if ( !(sld->style & SLD_STYLE_NO_TRACK) )
            {
               UG_COLOR track_color = sld->track_color;
               if ( (sld->style & SLD_STYLE_USE_ALTERNATE_COLORS) )
               {
                  track_color = sld->alt_track_color;
               }
               
               if (sld->orientation == SLD_ORIENTATION_HORIZONTAL) {
                  UG_S16 track_y_center = (obj->a_abs.ys + obj->a_abs.ye) / 2;
                  UG_S16 track_height = 4;
                  UG_FillFrame(obj->a_abs.xs + 2, track_y_center - track_height/2, 
                              obj->a_abs.xe - 2, track_y_center + track_height/2, track_color);
               } else {
                  UG_S16 track_x_center = (obj->a_abs.xs + obj->a_abs.xe) / 2;
                  UG_S16 track_width = 4;
                  UG_FillFrame(track_x_center - track_width/2, obj->a_abs.ys + 2,
                              track_x_center + track_width/2, obj->a_abs.ye - 2, track_color);
               }
            }

            /* Draw tick marks */
            if ( !(sld->style & SLD_STYLE_NO_TICKS) && (sld->tick_interval > 0) )
            {
               UG_S32 tick_value;
               UG_S16 tick_pos;
               
               for (tick_value = sld->min_value; tick_value <= sld->max_value; tick_value += sld->tick_interval)
               {
                  if (sld->orientation == SLD_ORIENTATION_HORIZONTAL) {
                     tick_pos = _UG_SliderValueToPosition(sld, obj, tick_value);
                     UG_DrawLine(tick_pos, obj->a_abs.ye + 2, tick_pos, obj->a_abs.ye + 6, sld->tick_color);
                  } else {
                     tick_pos = _UG_SliderValueToPosition(sld, obj, tick_value);
                     UG_DrawLine(obj->a_abs.xe + 2, tick_pos, obj->a_abs.xe + 6, tick_pos, sld->tick_color);
                  }
               }
            }

            obj->state &= ~OBJ_STATE_REDRAW;
#ifdef UGUI_USE_POSTRENDER_EVENT
            _UG_SendObjectPostrenderEvent(wnd, obj);
#endif
         }
         else
         {
            /* Not a full redraw - just update handle position */
            _UG_SliderCalculateHandlePosition(sld, obj);
            
            /* Clear previous handle position by redrawing track area */
            if (sld->prev_handle_area.xs != sld->handle_area.xs || 
                sld->prev_handle_area.ys != sld->handle_area.ys)
            {
               /* Clear previous handle area with background/track */
               UG_COLOR clear_color = sld->track_color;
               if (sld->style & SLD_STYLE_USE_ALTERNATE_COLORS) {
                  clear_color = sld->alt_track_color;
               }
               
               /* Clear the previous handle area */
               UG_FillFrame(sld->prev_handle_area.xs, sld->prev_handle_area.ys, 
                           sld->prev_handle_area.xe, sld->prev_handle_area.ye, clear_color);
               
               /* Redraw track in the cleared area */
               if (!(sld->style & SLD_STYLE_NO_TRACK)) {
                  if (sld->orientation == SLD_ORIENTATION_HORIZONTAL) {
                     UG_S16 track_y_center = (obj->a_abs.ys + obj->a_abs.ye) / 2;
                     UG_S16 track_height = 4;
                     UG_S16 track_xs = (sld->prev_handle_area.xs > obj->a_abs.xs + 2) ? sld->prev_handle_area.xs : obj->a_abs.xs + 2;
                     UG_S16 track_xe = (sld->prev_handle_area.xe < obj->a_abs.xe - 2) ? sld->prev_handle_area.xe : obj->a_abs.xe - 2;
                     if (track_xs <= track_xe) {
                        UG_FillFrame(track_xs, track_y_center - track_height/2, 
                                    track_xe, track_y_center + track_height/2, clear_color);
                     }
                  } else {
                     UG_S16 track_x_center = (obj->a_abs.xs + obj->a_abs.xe) / 2;
                     UG_S16 track_width = 4;
                     UG_S16 track_ys = (sld->prev_handle_area.ys > obj->a_abs.ys + 2) ? sld->prev_handle_area.ys : obj->a_abs.ys + 2;
                     UG_S16 track_ye = (sld->prev_handle_area.ye < obj->a_abs.ye - 2) ? sld->prev_handle_area.ye : obj->a_abs.ye - 2;
                     if (track_ys <= track_ye) {
                        UG_FillFrame(track_x_center - track_width/2, track_ys,
                                    track_x_center + track_width/2, track_ye, clear_color);
                     }
                  }
               }
               
               /* Redraw any tick marks that might be in the cleared area */
               if (!(sld->style & SLD_STYLE_NO_TICKS) && (sld->tick_interval > 0)) {
                  UG_S32 tick_value;
                  UG_S16 tick_pos;
                  
                  for (tick_value = sld->min_value; tick_value <= sld->max_value; tick_value += sld->tick_interval) {
                     tick_pos = _UG_SliderValueToPosition(sld, obj, tick_value);
                     
                     if (sld->orientation == SLD_ORIENTATION_HORIZONTAL) {
                        if (tick_pos >= sld->prev_handle_area.xs && tick_pos <= sld->prev_handle_area.xe) {
                           UG_DrawLine(tick_pos, obj->a_abs.ye + 2, tick_pos, obj->a_abs.ye + 6, sld->tick_color);
                        }
                     } else {
                        if (tick_pos >= sld->prev_handle_area.ys && tick_pos <= sld->prev_handle_area.ye) {
                           UG_DrawLine(obj->a_abs.xe + 2, tick_pos, obj->a_abs.xe + 6, tick_pos, sld->tick_color);
                        }
                     }
                  }
               }
            }
         }

         /* Draw slider handle */
         UG_COLOR handle_color = sld->handle_color;
         if ( (sld->style & SLD_STYLE_USE_ALTERNATE_COLORS) )
         {
            handle_color = sld->alt_handle_color;
         }

         /* Fill handle */
         UG_FillFrame(sld->handle_area.xs, sld->handle_area.ys, 
                     sld->handle_area.xe, sld->handle_area.ye, handle_color);

         /* Draw handle frame */
         if ( sld->style & SLD_STYLE_3D )
         {  /* 3D */
            _UG_DrawObjectFrame(sld->handle_area.xs, sld->handle_area.ys,
                               sld->handle_area.xe, sld->handle_area.ye, 
                               (sld->state & SLD_STATE_PRESSED) ? 
                               (UG_COLOR*)pal_slider_handle : (UG_COLOR*)pal_slider_handle);
         }
         else
         {  /* 2D */
             UG_DrawFrame(sld->handle_area.xs, sld->handle_area.ys,
                         sld->handle_area.xe, sld->handle_area.ye, handle_color);
         }
      }
      else
      {
          /* Hidden - fill with background color */
          UG_FillFrame(obj->a_abs.xs, obj->a_abs.ys, obj->a_abs.xe, obj->a_abs.ye, wnd->bc);
      }

      if (obj->state & OBJ_STATE_FOCUSED)
      {
         UG_DrawFrame(obj->a_abs.xs + 1, obj->a_abs.ys + 1, obj->a_abs.xe - 1, obj->a_abs.ye - 1, C_BLACK);
      }
      obj->state &= ~OBJ_STATE_UPDATE;
   }
}