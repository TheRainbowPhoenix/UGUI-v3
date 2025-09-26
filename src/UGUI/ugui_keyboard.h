#ifndef __UGUI_KEYBOARD_H
#define __UGUI_KEYBOARD_H

#include "ugui.h"

/* -------------------------------------------------------------------------------- */
/* -- PROTOTYPES                                                                 -- */
/* -------------------------------------------------------------------------------- */

/**
 * @brief Initializes the virtual keyboard. Must be called once at startup.
 * @param g Pointer to the main UG_GUI structure.
 * @return UG_RESULT_OK or UG_RESULT_FAIL.
 */
UG_RESULT UG_KeyboardInit(UG_GUI* g);

/**
 * @brief Shows the virtual keyboard and targets an input object.
 * @param target_obj The UG_OBJECT (of type OBJ_TYPE_INPUT_FIELD) that will receive key presses.
 * @return UG_RESULT_OK or UG_RESULT_FAIL.
 */
UG_RESULT UG_KeyboardShow(UG_OBJECT* target_obj);

/**
 * @brief Hides the virtual keyboard.
 * @return UG_RESULT_OK or UG_RESULT_FAIL.
 */
UG_RESULT UG_KeyboardHide(void);

#endif // __UGUI_KEYBOARD_H