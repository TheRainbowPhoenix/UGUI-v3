#ifndef __UGUI_INPUTFIELD_H
#define __UGUI_INPUTFIELD_H

#include "ugui.h"

/* -------------------------------------------------------------------------------- */
/* -- INPUT FIELD OBJECT                                                         -- */
/* -------------------------------------------------------------------------------- */

/* Object type */
#define OBJ_TYPE_INPUT_FIELD                          6 // A new, unused object type ID

/* Default input field IDs */
#define INPUT_ID_0                                      OBJ_ID_0
#define INPUT_ID_1                                      OBJ_ID_1
#define INPUT_ID_2                                      OBJ_ID_2
#define INPUT_ID_3                                      OBJ_ID_3
#define INPUT_ID_4                                      OBJ_ID_4
#define INPUT_ID_5                                      OBJ_ID_5
#define INPUT_ID_6                                      OBJ_ID_6
#define INPUT_ID_7                                      OBJ_ID_7
#define INPUT_ID_8                                      OBJ_ID_8
#define INPUT_ID_9                                      OBJ_ID_9
#define INPUT_ID_10                                     OBJ_ID_10
#define INPUT_ID_11                                     OBJ_ID_11
#define INPUT_ID_12                                     OBJ_ID_12
#define INPUT_ID_13                                     OBJ_ID_13
#define INPUT_ID_14                                     OBJ_ID_14
#define INPUT_ID_15                                     OBJ_ID_15
#define INPUT_ID_16                                     OBJ_ID_16
#define INPUT_ID_17                                     OBJ_ID_17
#define INPUT_ID_18                                     OBJ_ID_18
#define INPUT_ID_19                                     OBJ_ID_19

/* Custom event for this object */
#define EVENT_INPUTFIELD_CLICKED                      OBJ_EVENT_CLICKED// A custom event ID

/* Input Field structure */
typedef struct
{
   UG_U8 state;
   char* text;
   UG_U16 max_len;
   UG_U16 text_len;
   const UG_FONT* font;
   UG_U8 style; // Reserved for future use (e.g., border styles)
   UG_COLOR fc;
   UG_COLOR bc;
   UG_COLOR focused_bc; // Background color when it's the keyboard's target
   UG_U8 align;
} UG_INPUT_FIELD;


/* -------------------------------------------------------------------------------- */
/* -- PROTOTYPES                                                                 -- */
/* -------------------------------------------------------------------------------- */

/**
 * @brief Create an input field object.
 * @param wnd The window to which the object will be added.
 * @param inf Pointer to the UG_INPUT_FIELD object.
 * @param id The object's ID.
 * @param xs, ys, xe, ye The object's coordinates.
 * @param buffer The character buffer to store the text.
 * @param buffer_len The maximum size of the character buffer.
 * @return UG_RESULT_OK or UG_RESULT_FAIL.
 */
UG_RESULT UG_InputFieldCreate(UG_WINDOW* wnd, UG_INPUT_FIELD* inf, UG_U8 id, UG_S16 xs, UG_S16 ys, UG_S16 xe, UG_S16 ye, char* buffer, UG_U16 buffer_len);

/**
 * @brief Appends a character to the input field's text.
 * @param obj Pointer to the input field's UG_OBJECT.
 * @param c The character to append.
 * @return UG_RESULT_OK or UG_RESULT_FAIL.
 */
UG_RESULT UG_InputFieldAppendChar(UG_OBJECT* obj, char c);

/**
 * @brief Removes the last character from the input field's text (backspace).
 * @param obj Pointer to the input field's UG_OBJECT.
 * @return UG_RESULT_OK or UG_RESULT_FAIL.
 */
UG_RESULT UG_InputFieldBackspace(UG_OBJECT* obj);

/**
 * @brief Sets the text of the input field.
 * @param wnd The window containing the input field.
 * @param id The ID of the input field.
 * @param str The new text string.
 * @return UG_RESULT_OK or UG_RESULT_FAIL.
 */
UG_RESULT UG_InputFieldSetText(UG_WINDOW* wnd, UG_U8 id, char* str);

/**
 * @brief Gets the text from the input field.
 * @param wnd The window containing the input field.
 * @param id The ID of the input field.
 * @return A pointer to the text string.
 */
char* UG_InputFieldGetText(UG_WINDOW* wnd, UG_U8 id);

#endif // __UGUI_INPUTFIELD_H