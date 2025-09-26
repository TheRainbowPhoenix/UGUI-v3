/*
 * ugui_floating_keyboard.h - Floating Virtual Keyboard for UGUI
 * 
 * This keyboard system creates a floating panel that sends keyboard events
 * rather than directly manipulating input fields. It can be triggered by
 * hardware keys or programmatically.
 */

#ifndef __UGUI_FLOATING_KEYBOARD_H
#define __UGUI_FLOATING_KEYBOARD_H

#include "ugui.h"

/* -------------------------------------------------------------------------------- */
/* -- KEYBOARD EVENT TYPES                                                       -- */
/* -------------------------------------------------------------------------------- */
typedef enum {
    VKEY_CHAR,          // Regular character key
    VKEY_BACKSPACE,     // Backspace
    VKEY_ENTER,         // Enter/EXE
    VKEY_SPACE,         // Space bar
    VKEY_CAPS,          // Caps lock toggle
    VKEY_HIDE,          // Hide keyboard
    VKEY_TAB,           // Tab (optional)
    VKEY_ESC            // Escape (optional)
} UG_VKEY_TYPE;

typedef struct {
    UG_VKEY_TYPE type;
    char character;     // Only valid for VKEY_CHAR
    UG_U8 caps_state;   // Current caps state (0=lower, 1=upper)
} UG_KEYBOARD_EVENT;

/* -------------------------------------------------------------------------------- */
/* -- KEYBOARD CONFIGURATION                                                     -- */
/* -------------------------------------------------------------------------------- */
typedef struct {
    UG_S16 width;           // Keyboard panel width (0 = auto)
    UG_S16 height;          // Keyboard panel height (0 = auto)
    UG_S16 x_pos;           // X position (-1 = center)
    UG_S16 y_pos;           // Y position (-1 = bottom)
    UG_COLOR bg_color;      // Background color
    UG_COLOR key_color;     // Key background color
    UG_COLOR key_text_color; // Key text color
    UG_COLOR border_color;  // Border color
    UG_U8 key_spacing;      // Spacing between keys
    UG_U8 show_border;      // Show border around keyboard
} UG_KEYBOARD_CONFIG;

/* -------------------------------------------------------------------------------- */
/* -- CALLBACK FUNCTION TYPE                                                     -- */
/* -------------------------------------------------------------------------------- */
typedef void (*UG_KEYBOARD_CALLBACK)(UG_KEYBOARD_EVENT* event);

/* -------------------------------------------------------------------------------- */
/* -- PUBLIC FUNCTIONS                                                           -- */
/* -------------------------------------------------------------------------------- */

/**
 * @brief Initialize the floating keyboard system
 * @param gui Pointer to main GUI structure
 * @param callback Function to call when keys are pressed
 * @return UG_RESULT_OK on success, UG_RESULT_FAIL on error
 */
UG_RESULT UG_OSKeyboard_Init(UG_GUI* gui, UG_KEYBOARD_CALLBACK callback);

/**
 * @brief Configure keyboard appearance and behavior
 * @param config Pointer to configuration structure
 * @return UG_RESULT_OK on success, UG_RESULT_FAIL on error
 */
UG_RESULT UG_OSKeyboard_Configure(UG_KEYBOARD_CONFIG* config);

/**
 * @brief Show the floating keyboard
 * @return UG_RESULT_OK on success, UG_RESULT_FAIL on error
 */
UG_RESULT UG_OSKeyboard_Show(void);

/**
 * @brief Hide the floating keyboard
 * @return UG_RESULT_OK on success, UG_RESULT_FAIL on error
 */
UG_RESULT UG_OSKeyboard_Hide(void);

/**
 * @brief Toggle keyboard visibility
 * @return UG_RESULT_OK on success, UG_RESULT_FAIL on error
 */
UG_RESULT UG_OSKeyboard_Toggle(void);

/**
 * @brief Check if keyboard is currently visible
 * @return 1 if visible, 0 if hidden
 */
UG_U8 UG_OSKeyboard_IsVisible(void);

/**
 * @brief Process touch/click events for the keyboard
 * @param x Touch X coordinate
 * @param y Touch Y coordinate
 * @param pressed 1 for press, 0 for release
 * @return 1 if event was handled by keyboard, 0 if not
 */
UG_U8 UG_OSKeyboard_ProcessTouch(UG_S16 x, UG_S16 y, UG_U8 pressed);

/**
 * @brief Update and draw the keyboard (call this in your main update loop)
 */
void UG_OSKeyboard_Update(void);

/**
 * @brief Get default keyboard configuration
 * @param config Pointer to config structure to fill
 */
void UG_OSKeyboard_GetDefaultConfig(UG_KEYBOARD_CONFIG* config);

#endif // __UGUI_FLOATING_KEYBOARD_H

