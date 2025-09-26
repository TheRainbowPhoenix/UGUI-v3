#include "ugui_keyboard.h"
#include "ugui_inputfield.h" // Needed to call AppendChar and Backspace

/* -------------------------------------------------------------------------------- */
/* -- DEFINES AND CONSTANTS                                                      -- */
/* -------------------------------------------------------------------------------- */
#define KEYBOARD_ROWS 5
#define KEYBOARD_COLS 11
#define KEYBOARD_MAX_KEYS (4 * 11 + 4) // 4 full rows + 4 special keys on 5th row
#define KEY_ID_CAPS 44
#define KEY_ID_SPACE 45
#define KEY_ID_BACKSPACE 46
#define KEY_ID_EXE 47
#define KEY_ID_HIDE 48

// Keyboard state
#define KBD_STATE_LOWER 0
#define KBD_STATE_UPPER 1

/* -------------------------------------------------------------------------------- */
/* -- KEY LAYOUTS                                                                -- */
/* -------------------------------------------------------------------------------- */
// Using single char strings for easy use with UG_ButtonSetText
const char* g_keymap_lower[KEYBOARD_ROWS][KEYBOARD_COLS] = {
    {"1","2","3","4","5","6","7","8","9","0","-"},
    {"q","w","e","r","t","y","u","i","o","p","@"},
    {"a","s","d","f","g","h","j","k","l",";",":"},
    {"","z","x","c","v","b","n","m",",",".",""}
};

const char* g_keymap_upper[KEYBOARD_ROWS][KEYBOARD_COLS] = {
    {"!","\"","#","$","%","&","'","(",")","^","="},
    {"Q","W","E","R","T","Y","U","I","O","P","/"},
    {"A","S","D","F","G","H","J","K","L","+","*"},
    {"","Z","X","C","V","B","N","M","<",">",""}
};

/* -------------------------------------------------------------------------------- */
/* -- STATIC VARIABLES                                                           -- */
/* -------------------------------------------------------------------------------- */
static UG_GUI* g_gui;
static UG_WINDOW g_keyboard_window;
static UG_OBJECT g_keyboard_objects[KEYBOARD_MAX_KEYS];
static UG_BUTTON g_keyboard_buttons[KEYBOARD_MAX_KEYS];
static UG_OBJECT* g_keyboard_target = NULL;
static UG_U8 g_keyboard_state = KBD_STATE_LOWER;

/* -------------------------------------------------------------------------------- */
/* -- PRIVATE FUNCTIONS                                                          -- */
/* -------------------------------------------------------------------------------- */

/**
 * @brief Updates the text on all keys when caps/shift state changes.
 */
static void _Keyboard_UpdateKeys(void)
{
    const char* (*current_map)[KEYBOARD_COLS] = (g_keyboard_state == KBD_STATE_LOWER) ? g_keymap_lower : g_keymap_upper;

    int key_index = 0;
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 11; ++c) {
            // Skip the first and last spot on row 4 (for caps key)
            if (r == 3 && (c == 0 || c == 10)) continue;
            UG_ButtonSetText(&g_keyboard_window, key_index, (char*)current_map[r][c]);
            key_index++;
        }
    }
}

/**
 * @brief The callback function for the keyboard window.
 * @param msg The message from a pressed key.
 */
static void _Keyboard_Callback(UG_MESSAGE* msg)
{
    if (msg->type != MSG_TYPE_OBJECT || msg->id != OBJ_TYPE_BUTTON || msg->event != OBJ_EVENT_CLICKED) {
        return;
    }

    if (g_keyboard_target == NULL) { // No target, do nothing
        UG_KeyboardHide();
        return;
    }

    switch(msg->sub_id)
    {
        case KEY_ID_CAPS:
            g_keyboard_state = (g_keyboard_state == KBD_STATE_LOWER) ? KBD_STATE_UPPER : KBD_STATE_LOWER;
            _Keyboard_UpdateKeys();
            break;
        case KEY_ID_HIDE:
        case KEY_ID_EXE:
            UG_KeyboardHide();
            break;
        case KEY_ID_BACKSPACE:
            UG_InputFieldBackspace(g_keyboard_target);
            break;
        case KEY_ID_SPACE:
            UG_InputFieldAppendChar(g_keyboard_target, ' ');
            break;
        default: // It's a character key
            {
                // Find the button object from the message source to get its text
                UG_OBJECT* btn_obj = (UG_OBJECT*)msg->src;
                UG_BUTTON* btn_data = (UG_BUTTON*)btn_obj->data;
                if (btn_data && btn_data->str && btn_data->str[0] != '\0') {
                    UG_InputFieldAppendChar(g_keyboard_target, btn_data->str[0]);
                }
            }
            break;
    }
}

/* -------------------------------------------------------------------------------- */
/* -- PUBLIC FUNCTIONS                                                           -- */
/* -------------------------------------------------------------------------------- */

UG_RESULT UG_KeyboardInit(UG_GUI* g)
{
    g_gui = g;
    if (UG_WindowCreate(&g_keyboard_window, g_keyboard_objects, KEYBOARD_MAX_KEYS, _Keyboard_Callback) != UG_RESULT_OK) {
        return UG_RESULT_FAIL;
    }
    
    // Configure window: no title, specific color
    UG_WindowSetStyle(&g_keyboard_window, WND_STYLE_2D | WND_STYLE_HIDE_TITLE);
    UG_WindowSetBackColor(&g_keyboard_window, C_WHITE_94);

    // Calculate key dimensions based on screen width
    UG_S16 screen_w = UG_GetXDim();
    UG_S16 screen_h = UG_GetYDim();
    UG_S16 key_w = (screen_w - (KEYBOARD_COLS + 1) * 2) / KEYBOARD_COLS;
    UG_S16 key_h = key_w;
    UG_S16 v_spacing = 2;
    UG_S16 h_spacing = 2;
    
    int key_index = 0;
    
    // Create character keys (Rows 0-3)
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 11; c++) {
            // Skip placeholder spots on the 4th row
            if (r == 3 && (c == 0 || c == 10)) continue;
            
            UG_S16 xs = h_spacing + c * (key_w + h_spacing);
            UG_S16 ys = v_spacing + r * (key_h + v_spacing);

            UG_ButtonCreate(&g_keyboard_window, &g_keyboard_buttons[key_index], key_index, xs, ys, xs + key_w, ys + key_h);
            UG_ButtonSetFont(&g_keyboard_window, key_index, &FONT_8X12);
            key_index++;
        }
    }

    // Create CAPS key (4th row, 1st position)
    UG_S16 xs_caps = h_spacing;
    UG_S16 ys_caps = v_spacing + 3 * (key_h + v_spacing);
    UG_ButtonCreate(&g_keyboard_window, &g_keyboard_buttons[KEY_ID_CAPS], KEY_ID_CAPS, xs_caps, ys_caps, xs_caps + key_w, ys_caps + key_h);
    UG_ButtonSetText(&g_keyboard_window, KEY_ID_CAPS, "CAPS");
    UG_ButtonSetFont(&g_keyboard_window, KEY_ID_CAPS, &FONT_8X8); // Use smaller font if needed

    // Create special keys (5th row)
    UG_S16 y_spec = v_spacing + 4 * (key_h + v_spacing);
    UG_S16 x_curr = h_spacing;

    // Space bar (5 columns wide)
    UG_S16 w_space = 5 * key_w + 4 * h_spacing;
    UG_ButtonCreate(&g_keyboard_window, &g_keyboard_buttons[KEY_ID_SPACE], KEY_ID_SPACE, x_curr, y_spec, x_curr + w_space, y_spec + key_h);
    UG_ButtonSetText(&g_keyboard_window, KEY_ID_SPACE, "Space");
    x_curr += w_space + h_spacing;

    // Backspace (2 columns wide)
    UG_S16 w_2col = 2 * key_w + 1 * h_spacing;
    UG_ButtonCreate(&g_keyboard_window, &g_keyboard_buttons[KEY_ID_BACKSPACE], KEY_ID_BACKSPACE, x_curr, y_spec, x_curr + w_2col, y_spec + key_h);
    UG_ButtonSetText(&g_keyboard_window, KEY_ID_BACKSPACE, "<--");
    x_curr += w_2col + h_spacing;
    
    // EXE (2 columns wide)
    UG_ButtonCreate(&g_keyboard_window, &g_keyboard_buttons[KEY_ID_EXE], KEY_ID_EXE, x_curr, y_spec, x_curr + w_2col, y_spec + key_h);
    UG_ButtonSetText(&g_keyboard_window, KEY_ID_EXE, "EXE");
    x_curr += w_2col + h_spacing;
    
    // Hide (2 columns wide)
    UG_ButtonCreate(&g_keyboard_window, &g_keyboard_buttons[KEY_ID_HIDE], KEY_ID_HIDE, x_curr, y_spec, x_curr + w_2col, y_spec + key_h);
    UG_ButtonSetText(&g_keyboard_window, KEY_ID_HIDE, "HIDE");

    _Keyboard_UpdateKeys(); // Set initial key text

    return UG_RESULT_OK;
}

UG_RESULT UG_KeyboardShow(UG_OBJECT* target_obj)
{
    // if (target_obj == NULL || target_obj->type != OBJ_TYPE_INPUT_FIELD) {
    //     return UG_RESULT_FAIL;
    // }

    // Unfocus the old target if there was one
    if (g_keyboard_target) {
        g_keyboard_target->state &= ~OBJ_STATE_FOCUSED;
        g_keyboard_target->state |= OBJ_STATE_UPDATE | OBJ_STATE_REDRAW;
    }
    
    g_keyboard_target = target_obj;
    g_keyboard_target->state |= OBJ_STATE_FOCUSED; // Mark the new target as focused
    g_keyboard_target->state |= OBJ_STATE_UPDATE | OBJ_STATE_REDRAW;

    // Calculate dimensions
    UG_S16 screen_w = UG_GetXDim();
    UG_S16 screen_h = UG_GetYDim();
    UG_S16 key_w = (screen_w - (KEYBOARD_COLS + 1) * 2) / KEYBOARD_COLS;
    UG_S16 key_h = key_w;
    UG_S16 v_spacing = 2;

    UG_S16 keyboard_h = 5 * (key_h + v_spacing) + v_spacing;
    UG_S16 keyboard_y = screen_h - keyboard_h;

    UG_WindowResize(&g_keyboard_window, 0, keyboard_y, screen_w - 1, screen_h - 1);
    UG_WindowShow(&g_keyboard_window);

    return UG_RESULT_OK;
}

UG_RESULT UG_KeyboardHide(void)
{
    if (g_keyboard_target) {
        g_keyboard_target->state &= ~OBJ_STATE_FOCUSED; // Unfocus the target
        g_keyboard_target->state |= OBJ_STATE_UPDATE | OBJ_STATE_REDRAW;
        g_keyboard_target = NULL;
    }
    UG_WindowHide(&g_keyboard_window);
    return UG_RESULT_OK;
}