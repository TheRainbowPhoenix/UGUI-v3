#ifndef __UGUI_MESSAGEBOX_H
#define __UGUI_MESSAGEBOX_H

#include "ugui.h"
#include <stdbool.h>
#include "ugui_oskeyboard.h" 

/* -------------------------------------------------------------------------------- */
/* -- DEFINES & ENUMS                                                            -- */
/* -------------------------------------------------------------------------------- */

// Defines the type of message box to display
typedef enum {
    MB_TYPE_INFO,       // Single "OK" button
    MB_TYPE_CONFIRM,    // "Yes" and "No" buttons
    MB_TYPE_PROMPT      // Text input field with "OK" and "Cancel" buttons
} MB_TYPE;

// Defines the result returned by the message box
typedef enum {
    MB_RES_NONE,        // No result yet, or box not shown
    MB_RES_OK,
    MB_RES_YES,
    MB_RES_NO,
    MB_RES_CANCEL
} MB_RESULT;


/* -------------------------------------------------------------------------------- */
/* -- PUBLIC FUNCTIONS                                                           -- */
/* -------------------------------------------------------------------------------- */

/**
 * @brief Initializes the message box system. Must be called once after UG_Init.
 * @param gui Pointer to the main UG_GUI structure.
 * @return UG_RESULT_OK on success.
 */
UG_RESULT UG_MessageBoxInit(UG_GUI* gui);

/**
 * @brief Shows a message box. This function is non-blocking.
 * Use UG_MessageBoxIsActive() to check its state and UG_MessageBoxGetResult()
 * to retrieve the result once it's no longer active.
 *
 * @param message The main text content of the message box.
 * @param title The title of the message box window.
 * @param type The type of message box (Info, Confirm, or Prompt).
 * @param prompt_buffer Pointer to a character buffer for the Prompt type.
 * @param prompt_buffer_size The size of the prompt buffer.
 * @return UG_RESULT_OK if the box was shown, UG_RESULT_FAIL if one is already active.
 */
UG_RESULT UG_MessageBoxShow(const char* message, const char* title, MB_TYPE type, char* prompt_buffer, int prompt_buffer_size);

/**
 * @brief Checks if a message box is currently active and waiting for user input.
 * @return true if active, false otherwise.
 */
bool UG_MessageBoxIsActive(void);

/**
 * @brief Gets the result from the last message box interaction.
 * The result is cleared after being read once.
 * @return The user's selection (MB_RES_OK, MB_RES_YES, etc.).
 */
MB_RESULT UG_MessageBoxGetResult(void);

/**
 * @brief Processes keyboard events when the message box is active.
 * This should be called from your main keyboard event handler.
 * @param event The keyboard event from the UGUI OS Keyboard.
 */
void UG_MessageBox_KeyboardProc(UG_KEYBOARD_EVENT* event);

#endif // __UGUI_MESSAGEBOX_H