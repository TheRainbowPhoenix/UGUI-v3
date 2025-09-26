#include "ugui_messagebox.h"
#include <string.h>

/* -------------------------------------------------------------------------------- */
/* -- PRIVATE DEFINES & VARS                                                     -- */
/* -------------------------------------------------------------------------------- */
#define MAX_MB_OBJECTS 4 // Textbox, Button1, Button2, InputField

// IDs for internal objects
#define MB_BTN_ID_1    0 // Corresponds to OK / Yes
#define MB_BTN_ID_2    1 // Corresponds to No / Cancel
#define MB_TXB_ID      0
#define MB_INP_ID      0

// Global state for the message box module
static UG_GUI*     g_gui_ptr = NULL;
static UG_WINDOW   g_mb_window;
static UG_OBJECT   g_mb_objects[MAX_MB_OBJECTS];
static UG_BUTTON   g_mb_button1;
static UG_BUTTON   g_mb_button2;
static UG_TEXTBOX  g_mb_textbox;
static UG_INPUT_FIELD g_mb_input;

static volatile bool      g_is_active = false;
static volatile MB_RESULT g_result = MB_RES_NONE;
static MB_TYPE            g_current_type;

// Pointer to the external buffer for prompt mode
static char* g_prompt_buffer_ptr = NULL;


/* -------------------------------------------------------------------------------- */
/* -- PRIVATE FUNCTIONS                                                          -- */
/* -------------------------------------------------------------------------------- */

// Callback function for the message box window
static void _messagebox_callback(UG_MESSAGE* msg) {
    if (msg->type != MSG_TYPE_OBJECT || msg->id != OBJ_TYPE_BUTTON) {
        return;
    }

    if (msg->event == OBJ_EVENT_CLICKED || msg->event == OBJ_EVENT_PRESSED) {
        switch (msg->sub_id) {
            case MB_BTN_ID_1: // OK or Yes
                if (g_current_type == MB_TYPE_INFO)   g_result = MB_RES_OK;
                if (g_current_type == MB_TYPE_CONFIRM) g_result = MB_RES_YES;
                if (g_current_type == MB_TYPE_PROMPT)  g_result = MB_RES_OK;
                break;

            case MB_BTN_ID_2: // No or Cancel
                if (g_current_type == MB_TYPE_CONFIRM) g_result = MB_RES_NO;
                if (g_current_type == MB_TYPE_PROMPT)  g_result = MB_RES_CANCEL;
                break;
        }
        
        if (g_result != MB_RES_NONE) {
            g_is_active = false;
        }
    }
}

/* -------------------------------------------------------------------------------- */
/* -- PUBLIC API IMPLEMENTATION                                                  -- */
/* -------------------------------------------------------------------------------- */

UG_RESULT UG_MessageBoxInit(UG_GUI* gui) {
    if (!gui) return UG_RESULT_FAIL;
    g_gui_ptr = gui;

    // Create the window (but don't show it)
    UG_WindowCreate(&g_mb_window, g_mb_objects, MAX_MB_OBJECTS, _messagebox_callback);
    UG_WindowSetTitleTextFont(&g_mb_window, &FONT_SYSTEM_1);

    // Create all potential objects
    // 1. Textbox for the message
    UG_TextboxCreate(&g_mb_window, &g_mb_textbox, MB_TXB_ID, 5, 5, 235, 50);
    UG_TextboxSetFont(&g_mb_window, MB_TXB_ID, &FONT_8X12);
    UG_TextboxSetAlignment(&g_mb_window, MB_TXB_ID, ALIGN_CENTER);

    // 2. Button 1 (OK / Yes)
    UG_ButtonCreate(&g_mb_window, &g_mb_button1, MB_BTN_ID_1, 0, 0, 0, 0); // Pos will be set later
    UG_ButtonSetFont(&g_mb_window, MB_BTN_ID_1, &FONT_SYSTEM_1);

    // 3. Button 2 (No / Cancel)
    UG_ButtonCreate(&g_mb_window, &g_mb_button2, MB_BTN_ID_2, 0, 0, 0, 0); // Pos will be set later
    UG_ButtonSetFont(&g_mb_window, MB_BTN_ID_2, &FONT_SYSTEM_1);
    
    // 4. Input Field (for Prompt)
    UG_InputFieldCreate(&g_mb_window, &g_mb_input, MB_INP_ID, 10, 55, 230, 85, NULL, 0);

    return UG_RESULT_OK;
}

UG_RESULT UG_MessageBoxShow(const char* message, const char* title, MB_TYPE type, char* prompt_buffer, int prompt_buffer_size) {
    if (g_is_active) return UG_RESULT_FAIL;

    g_is_active = true;
    g_result = MB_RES_NONE;
    g_current_type = type;

    // --- Configure Window & Text ---
    UG_S16 win_w = 260;
    UG_S16 win_h = (type == MB_TYPE_PROMPT) ? 150 : 120;
    UG_S16 win_x = (g_gui_ptr->x_dim - win_w) / 2;
    UG_S16 win_y = (g_gui_ptr->y_dim - win_h) / 2;

    UG_WindowResize(&g_mb_window, win_x, win_y, win_x + win_w, win_y + win_h);
    UG_WindowSetTitleText(&g_mb_window, (char*)title);
    UG_TextboxSetText(&g_mb_window, MB_TXB_ID, (char*)message);

    // --- Configure Objects based on Type ---
    UG_S16 btn_w = 80;
    UG_S16 btn_h = 35;
    UG_S16 btn_y = win_h - btn_h - 15;

    switch (type) {
        case MB_TYPE_INFO:
            // Single "OK" button, centered
            UG_ButtonResize(&g_mb_window, MB_BTN_ID_1, (win_w - btn_w) / 2, btn_y, (win_w + btn_w) / 2, btn_y + btn_h);
            UG_ButtonSetText(&g_mb_window, MB_BTN_ID_1, "OK");
            UG_ButtonShow(&g_mb_window, MB_BTN_ID_1);
            UG_ButtonHide(&g_mb_window, MB_BTN_ID_2);
            UG_InputFieldHide(&g_mb_window, MB_INP_ID);
            break;

        case MB_TYPE_CONFIRM:
            // Two buttons: Yes / No
            UG_ButtonResize(&g_mb_window, MB_BTN_ID_1, (win_w / 2) - btn_w - 10, btn_y, (win_w / 2) - 10, btn_y + btn_h);
            UG_ButtonSetText(&g_mb_window, MB_BTN_ID_1, "Yes");
            UG_ButtonResize(&g_mb_window, MB_BTN_ID_2, (win_w / 2) + 10, btn_y, (win_w / 2) + 10 + btn_w, btn_y + btn_h);
            UG_ButtonSetText(&g_mb_window, MB_BTN_ID_2, "No");
            UG_ButtonShow(&g_mb_window, MB_BTN_ID_1);
            UG_ButtonShow(&g_mb_window, MB_BTN_ID_2);
            UG_InputFieldHide(&g_mb_window, MB_INP_ID);
            break;

        case MB_TYPE_PROMPT:
            // Two buttons: OK / Cancel + Input Field
            UG_ButtonResize(&g_mb_window, MB_BTN_ID_1, (win_w / 2) - btn_w - 10, btn_y, (win_w / 2) - 10, btn_y + btn_h);
            UG_ButtonSetText(&g_mb_window, MB_BTN_ID_1, "OK");
            UG_ButtonResize(&g_mb_window, MB_BTN_ID_2, (win_w / 2) + 10, btn_y, (win_w / 2) + 10 + btn_w, btn_y + btn_h);
            UG_ButtonSetText(&g_mb_window, MB_BTN_ID_2, "Cancel");
            UG_ButtonShow(&g_mb_window, MB_BTN_ID_1);
            UG_ButtonShow(&g_mb_window, MB_BTN_ID_2);

            g_prompt_buffer_ptr = prompt_buffer;
            if (g_prompt_buffer_ptr) g_prompt_buffer_ptr[0] = '\0';

            UG_InputFieldAssignBuffer(&g_mb_window, MB_INP_ID, prompt_buffer, prompt_buffer_size);
            UG_InputFieldShow(&g_mb_window, MB_INP_ID);
            break;
    }

    UG_WindowShow(&g_mb_window);
    return UG_RESULT_OK;
}

bool UG_MessageBoxIsActive(void) {
    return g_is_active;
}

MB_RESULT UG_MessageBoxGetResult(void) {
    if (g_result != MB_RES_NONE) {
        MB_RESULT res = g_result;
        g_result = MB_RES_NONE; // Clear result after reading
        return res;
    }
    return MB_RES_NONE;
}

void UG_MessageBox_KeyboardProc(UG_KEYBOARD_EVENT* event) {
    UG_OBJECT* focused_obj = g_mb_window.focused_obj;
    if (!focused_obj || focused_obj->type != OBJ_TYPE_INPUT_FIELD) {
        // If no input field is focused, just hide the keyboard on enter/hide
        if (event->type == VKEY_ENTER || event->type == VKEY_HIDE) {
            UG_OSKeyboard_Hide();
        }
        return;
    }

    switch (event->type) {
        case VKEY_CHAR:      UG_InputFieldAppendChar(focused_obj, event->character); break;
        case VKEY_BACKSPACE: UG_InputFieldBackspace(focused_obj); break;
        case VKEY_SPACE:     UG_InputFieldAppendChar(focused_obj, ' '); break;
        case VKEY_ENTER:     // Simulate OK click on Enter
            g_result = MB_RES_OK;
            g_is_active = false;
            UG_OSKeyboard_Hide();
            break;
        case VKEY_HIDE:
            UG_OSKeyboard_Hide();
            break;
        default: break;
    }
}