#include "ugui_inputfield.h"
#include <string.h>

/* -------------------------------------------------------------------------------- */
/* -- PRIVATE FUNCTIONS                                                          -- */
/* -------------------------------------------------------------------------------- */

/**
 * @brief The update function for the input field.
 * @param wnd The parent window.
 * @param obj The object to update.
 */
static void _UG_InputFieldUpdate(UG_WINDOW* wnd, UG_OBJECT* obj)
{
   UG_INPUT_FIELD* inf = (UG_INPUT_FIELD*)obj->data;
   UG_TEXT txt;
   UG_COLOR current_bc;
   if (obj->touch_state & OBJ_TOUCH_STATE_CLICK_ON_OBJECT)
   {
      obj->event = EVENT_INPUTFIELD_CLICKED;
      // Clear the click flag to consume the event.
      obj->touch_state &= ~OBJ_TOUCH_STATE_CLICK_ON_OBJECT;
   }

   // --- Update & Redraw Section ---
   if (obj->state & OBJ_STATE_UPDATE)
   {
      if (obj->state & OBJ_STATE_VISIBLE)
      {
         // Update absolute coordinates
         obj->a_abs.xs = obj->a_rel.xs + wnd->xs;
         obj->a_abs.ys = obj->a_rel.ys + wnd->ys;
         obj->a_abs.xe = obj->a_rel.xe + wnd->xs;
         obj->a_abs.ye = obj->a_rel.ye + wnd->ys;

         // Check if this object has focus
         current_bc = (obj->state & OBJ_STATE_FOCUSED) ? inf->focused_bc : inf->bc;

         // Draw background and border
         UG_FillFrame(obj->a_abs.xs, obj->a_abs.ys, obj->a_abs.xe, obj->a_abs.ye, current_bc);
         UG_DrawFrame(obj->a_abs.xs, obj->a_abs.ys, obj->a_abs.xe, obj->a_abs.ye, C_BLACK);

         // Prepare and draw the text
         txt.a.xs = obj->a_abs.xs + 2;
         txt.a.ys = obj->a_abs.ys + 2;
         txt.a.xe = obj->a_abs.xe - 2;
         txt.a.ye = obj->a_abs.ye - 2;
         txt.str = inf->text;
         txt.font = inf->font;
         txt.fc = inf->fc;
         txt.bc = current_bc;
         txt.align = inf->align;
         txt.h_space = 2;
         txt.v_space = 2;

         _UG_PutText(&txt);
      }
      obj->state &= ~OBJ_STATE_UPDATE;
   }
}


/* -------------------------------------------------------------------------------- */
/* -- PUBLIC FUNCTIONS                                                           -- */
/* -------------------------------------------------------------------------------- */

UG_RESULT UG_InputFieldCreate(UG_WINDOW* wnd, UG_INPUT_FIELD* inf, UG_U8 id, UG_S16 xs, UG_S16 ys, UG_S16 xe, UG_S16 ye, char* buffer, UG_U16 buffer_len)
{
   UG_OBJECT* obj;

   obj = _UG_GetFreeObject(wnd);
   if (obj == NULL) return UG_RESULT_FAIL;

   // Initialize object-specific data
   inf->text = buffer;
   inf->max_len = buffer_len;
   inf->text[0] = '\0'; // Ensure buffer is empty
   inf->text_len = 0;
   if (UG_GetGUI() != NULL) inf->font = &(UG_GetGUI()->font);
   else inf->font = NULL;
   inf->style = 0;
   inf->fc = C_BLACK;
   inf->bc = C_WHITE;
   inf->focused_bc = C_PALE_TURQUOISE; // A light blue to indicate focus
   inf->align = ALIGN_CENTER_LEFT;

   // Initialize standard object parameters
   obj->update = _UG_InputFieldUpdate;
   obj->touch_state = OBJ_TOUCH_STATE_INIT;
   obj->type = OBJ_TYPE_INPUT_FIELD;
   obj->event = OBJ_EVENT_NONE;
   obj->a_rel.xs = xs;
   obj->a_rel.ys = ys;
   obj->a_rel.xe = xe;
   obj->a_rel.ye = ye;
   obj->id = id;
   obj->state = OBJ_STATE_VISIBLE | OBJ_STATE_REDRAW | OBJ_STATE_VALID | OBJ_STATE_TOUCH_ENABLE;
   obj->data = (void*)inf;
   obj->state &= ~OBJ_STATE_FREE;

   return UG_RESULT_OK;
}

UG_RESULT UG_InputFieldAppendChar(UG_OBJECT* obj, char c)
{
   if (!obj || obj->type != OBJ_TYPE_INPUT_FIELD) return UG_RESULT_FAIL;

   UG_INPUT_FIELD* inf = (UG_INPUT_FIELD*)obj->data;

   if (inf->text_len < (inf->max_len - 1))
   {
      inf->text[inf->text_len] = c;
      inf->text_len++;
      inf->text[inf->text_len] = '\0';
      obj->state |= OBJ_STATE_UPDATE | OBJ_STATE_REDRAW;
      return UG_RESULT_OK;
   }
   return UG_RESULT_FAIL;
}

UG_RESULT UG_InputFieldBackspace(UG_OBJECT* obj)
{
   if (!obj || obj->type != OBJ_TYPE_INPUT_FIELD) return UG_RESULT_FAIL;

   UG_INPUT_FIELD* inf = (UG_INPUT_FIELD*)obj->data;

   if (inf->text_len > 0)
   {
      inf->text_len--;
      inf->text[inf->text_len] = '\0';
      obj->state |= OBJ_STATE_UPDATE | OBJ_STATE_REDRAW;
      return UG_RESULT_OK;
   }
   return UG_RESULT_FAIL;
}

UG_RESULT UG_InputFieldSetText(UG_WINDOW* wnd, UG_U8 id, char* str)
{
   UG_OBJECT* obj = _UG_SearchObject(wnd, OBJ_TYPE_INPUT_FIELD, id);
   if (obj == NULL) return UG_RESULT_FAIL;

   UG_INPUT_FIELD* inf = (UG_INPUT_FIELD*)obj->data;
   strncpy(inf->text, str, inf->max_len - 1);
   inf->text[inf->max_len - 1] = '\0'; // Ensure null termination
   inf->text_len = strlen(inf->text);
   obj->state |= OBJ_STATE_UPDATE | OBJ_STATE_REDRAW;
   return UG_RESULT_OK;
}

char* UG_InputFieldGetText(UG_WINDOW* wnd, UG_U8 id)
{
   UG_OBJECT* obj = _UG_SearchObject(wnd, OBJ_TYPE_INPUT_FIELD, id);
   if (obj == NULL) return NULL;
   
   UG_INPUT_FIELD* inf = (UG_INPUT_FIELD*)obj->data;
   return inf->text;
}