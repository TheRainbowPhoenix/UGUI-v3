#include "ugui_oskeyboard.h"
#include <string.h>

/* -------------------------------------------------------------------------------- */
/* -- INTERNAL CONSTANTS                                                         -- */
/* -------------------------------------------------------------------------------- */
#define KB_ROWS 4
#define KB_COLS 11
#define KB_MAX_KEYS 44
#define KB_SPECIAL_KEYS 5

// Key layouts
static const char* kb_layout_lower[KB_ROWS][KB_COLS] = {
    {"1","2","3","4","5","6","7","8","9","0","<"},  // < = backspace
    {"q","w","e","r","t","y","u","i","o","p","@"},
    {"a","s","d","f","g","h","j","k","l",";",":"},
    {"^","z","x","c","v","b","n","m",",",".","?"}   // ^ = caps
};

static const char* kb_layout_upper[KB_ROWS][KB_COLS] = {
    {"!","\"","#","$","%","&","'","(",")","_","<"},
    {"Q","W","E","R","T","Y","U","I","O","P","/"},
    {"A","S","D","F","G","H","J","K","L","+","*"},
    {"^","Z","X","C","V","B","N","M","<",">","?"}
};

/* -------------------------------------------------------------------------------- */
/* -- INTERNAL STRUCTURES                                                        -- */
/* -------------------------------------------------------------------------------- */
typedef struct {
    UG_S16 x, y, w, h;
    UG_VKEY_TYPE type;
    char character;
    UG_U8 pressed;
} KB_KEY;

typedef struct {
    UG_GUI* gui;
    UG_KEYBOARD_CALLBACK callback;
    UG_KEYBOARD_CONFIG config;
    KB_KEY keys[KB_MAX_KEYS];
    UG_U8 num_keys;
    UG_U8 visible;
    UG_U8 caps_state;
    UG_S16 panel_x, panel_y, panel_w, panel_h;
    UG_U8 needs_redraw;
} KB_STATE;

/* -------------------------------------------------------------------------------- */
/* -- STATIC VARIABLES                                                           -- */
/* -------------------------------------------------------------------------------- */
static KB_STATE g_keyboard = {0};

/* -------------------------------------------------------------------------------- */
/* -- INTERNAL FUNCTIONS                                                         -- */
/* -------------------------------------------------------------------------------- */

static void _KB_CalculateLayout(void)
{
    UG_S16 screen_w = UG_GetXDim();
    UG_S16 screen_h = UG_GetYDim();
    
    // Calculate panel size
    if (g_keyboard.config.width == 0) {
        g_keyboard.panel_w = screen_w * 9 / 10; // 90% of screen width
    } else {
        g_keyboard.panel_w = g_keyboard.config.width;
    }
    
    if (g_keyboard.config.height == 0) {
        g_keyboard.panel_h = screen_h / 3; // 1/3 of screen height
    } else {
        g_keyboard.panel_h = g_keyboard.config.height;
    }
    
    // Calculate panel position
    if (g_keyboard.config.x_pos == -1) {
        g_keyboard.panel_x = (screen_w - g_keyboard.panel_w) / 2;
    } else {
        g_keyboard.panel_x = g_keyboard.config.x_pos;
    }
    
    if (g_keyboard.config.y_pos == -1) {
        g_keyboard.panel_y = screen_h - g_keyboard.panel_h;
    } else {
        g_keyboard.panel_y = g_keyboard.config.y_pos;
    }
    
    // Calculate key layout
    UG_S16 usable_w = g_keyboard.panel_w - (g_keyboard.config.key_spacing * 2);
    UG_S16 usable_h = g_keyboard.panel_h - (g_keyboard.config.key_spacing * 2);
    
    UG_S16 key_w = (usable_w - (KB_COLS - 1) * g_keyboard.config.key_spacing) / KB_COLS;
    UG_S16 key_h = (usable_h - (KB_ROWS - 1 + 1) * g_keyboard.config.key_spacing) / (KB_ROWS + 1); // +1 for special row
    
    g_keyboard.num_keys = 0;
    
    // Create regular keys
    for (int row = 0; row < KB_ROWS; row++) {
        for (int col = 0; col < KB_COLS; col++) {
            if (g_keyboard.num_keys >= KB_MAX_KEYS) break;
            
            KB_KEY* key = &g_keyboard.keys[g_keyboard.num_keys];
            
            key->x = g_keyboard.panel_x + g_keyboard.config.key_spacing + 
                     col * (key_w + g_keyboard.config.key_spacing);
            key->y = g_keyboard.panel_y + g_keyboard.config.key_spacing + 
                     row * (key_h + g_keyboard.config.key_spacing);
            key->w = key_w;
            key->h = key_h;
            key->pressed = 0;
            
            // Determine key type and character
            const char* ch = g_keyboard.caps_state ? 
                           kb_layout_upper[row][col] : kb_layout_lower[row][col];
            
            if (ch[0] == '<') {
                key->type = VKEY_BACKSPACE;
                key->character = 0;
            } else if (ch[0] == '^') {
                key->type = VKEY_CAPS;
                key->character = 0;
            } else if (ch[0] == '?') {
                key->type = VKEY_HIDE;
                key->character = 0;
            } else {
                key->type = VKEY_CHAR;
                key->character = ch[0];
            }
            
            g_keyboard.num_keys++;
        }
    }
    
    // Add special keys row (space, enter, etc.)
    UG_S16 special_y = g_keyboard.panel_y + g_keyboard.config.key_spacing + 
                       KB_ROWS * (key_h + g_keyboard.config.key_spacing);
    
    // Space bar (takes 6 columns)
    if (g_keyboard.num_keys < KB_MAX_KEYS) {
        KB_KEY* space = &g_keyboard.keys[g_keyboard.num_keys];
        space->x = g_keyboard.panel_x + g_keyboard.config.key_spacing;
        space->y = special_y;
        space->w = 6 * key_w + 5 * g_keyboard.config.key_spacing;
        space->h = key_h;
        space->type = VKEY_SPACE;
        space->character = ' ';
        space->pressed = 0;
        g_keyboard.num_keys++;
    }
    
    // Enter key (takes remaining columns)
    if (g_keyboard.num_keys < KB_MAX_KEYS) {
        KB_KEY* enter = &g_keyboard.keys[g_keyboard.num_keys];
        enter->x = g_keyboard.panel_x + g_keyboard.config.key_spacing + 
                   6 * key_w + 6 * g_keyboard.config.key_spacing;
        enter->y = special_y;
        enter->w = 5 * key_w + 4 * g_keyboard.config.key_spacing;
        enter->h = key_h;
        enter->type = VKEY_ENTER;
        enter->character = 0;
        enter->pressed = 0;
        g_keyboard.num_keys++;
    }
}

static void _KB_DrawKey(KB_KEY* key)
{
    UG_COLOR key_color = key->pressed ? 
        (g_keyboard.config.key_color ^ 0x7BEF) : g_keyboard.config.key_color; // Invert for pressed
    UG_U8 d = 1; // Border depth
    
    // Draw key background (similar to button)
    UG_FillFrame(key->x + d, key->y + d, key->x + key->w - 1 - d, key->y + key->h - 1 - d, key_color);
    
    // Draw key border (3D effect like buttons)
    if (g_keyboard.config.show_border) {
        // Simple raised/pressed effect
        UG_COLOR light = key->pressed ? g_keyboard.config.border_color : C_WHITE;
        UG_COLOR dark = key->pressed ? C_WHITE : g_keyboard.config.border_color;
        
        // Top and left (light)
        UG_DrawLine(key->x, key->y, key->x + key->w - 1, key->y, light);
        UG_DrawLine(key->x, key->y, key->x, key->y + key->h - 1, light);
        
        // Bottom and right (dark)
        UG_DrawLine(key->x + key->w - 1, key->y, key->x + key->w - 1, key->y + key->h - 1, dark);
        UG_DrawLine(key->x, key->y + key->h - 1, key->x + key->w - 1, key->y + key->h - 1, dark);
    }
    
    // Get text to display
    const char* text = "";
    switch (key->type) {
        case VKEY_CHAR:
            {
                static char ch_str[2] = {0, 0};
                ch_str[0] = key->character;
                text = ch_str;
            }
            break;
        case VKEY_BACKSPACE:
            text = "Back";
            break;
        case VKEY_SPACE:
            text = "Space";
            break;
        case VKEY_ENTER:
            text = "Enter";
            break;
        case VKEY_CAPS:
            text = g_keyboard.caps_state ? "CAPS" : "caps";
            break;
        case VKEY_HIDE:
            text = "Hide";
            break;
        default:
            break;
    }
    
    // Draw text using UGUI's text rendering (like buttons do)
    if (strlen(text) > 0) {
        UG_TEXT txt;
        txt.bc = key_color;
        txt.fc = g_keyboard.config.key_text_color;
        txt.a.xs = key->x + d;
        txt.a.ys = key->y + d;
        txt.a.xe = key->x + key->w - 1 - d;
        txt.a.ye = key->y + key->h - 1 - d;
        txt.align = ALIGN_CENTER;
        
        // Use appropriate font based on text length and key size
        if (strlen(text) > 3 && key->w < 50) {
            txt.font = &FONT_8X8; // Smaller font for longer text on small keys
        } else if (UG_GetGUI() && UG_GetGUI()->font.char_width > 0) {
            txt.font = &(UG_GetGUI()->font);
        } else {
            txt.font = &FONT_8X12; // Default fallback
        }
        
        txt.h_space = 2;
        txt.v_space = 2;
        txt.str = (char*)text;
        _UG_PutText(&txt);
    }
}

static void _KB_Draw(void)
{
    if (!g_keyboard.visible) return;
    
    // Draw panel background
    UG_FillFrame(g_keyboard.panel_x, g_keyboard.panel_y,
                 g_keyboard.panel_x + g_keyboard.panel_w - 1,
                 g_keyboard.panel_y + g_keyboard.panel_h - 1,
                 g_keyboard.config.bg_color);
    
    // Draw panel border
    if (g_keyboard.config.show_border) {
        UG_DrawFrame(g_keyboard.panel_x, g_keyboard.panel_y,
                     g_keyboard.panel_x + g_keyboard.panel_w - 1,
                     g_keyboard.panel_y + g_keyboard.panel_h - 1,
                     g_keyboard.config.border_color);
    }
    
    // Draw all keys
    for (int i = 0; i < g_keyboard.num_keys; i++) {
        _KB_DrawKey(&g_keyboard.keys[i]);
    }
    
    g_keyboard.needs_redraw = 0;
}

static KB_KEY* _KB_FindKeyAt(UG_S16 x, UG_S16 y)
{
    if (!g_keyboard.visible) return NULL;
    
    for (int i = 0; i < g_keyboard.num_keys; i++) {
        KB_KEY* key = &g_keyboard.keys[i];
        if (x >= key->x && x < key->x + key->w &&
            y >= key->y && y < key->y + key->h) {
            return key;
        }
    }
    return NULL;
}

static void _KB_SendKeyEvent(KB_KEY* key)
{
    if (!g_keyboard.callback) return;
    
    UG_KEYBOARD_EVENT event;
    event.type = key->type;
    event.character = key->character;
    event.caps_state = g_keyboard.caps_state;
    
    // Handle caps toggle
    if (key->type == VKEY_CAPS) {
        g_keyboard.caps_state = !g_keyboard.caps_state;
        _KB_CalculateLayout(); // Recalculate to update character keys
        g_keyboard.needs_redraw = 1;
    }
    
    g_keyboard.callback(&event);
}

/* -------------------------------------------------------------------------------- */
/* -- PUBLIC FUNCTIONS                                                           -- */
/* -------------------------------------------------------------------------------- */

UG_RESULT UG_OSKeyboard_Init(UG_GUI* gui, UG_KEYBOARD_CALLBACK callback)
{
    if (!gui || !callback) return UG_RESULT_FAIL;
    
    memset(&g_keyboard, 0, sizeof(g_keyboard));
    
    g_keyboard.gui = gui;
    g_keyboard.callback = callback;
    
    // Set default configuration
    UG_OSKeyboard_GetDefaultConfig(&g_keyboard.config);
    
    return UG_RESULT_OK;
}

UG_RESULT UG_OSKeyboard_Configure(UG_KEYBOARD_CONFIG* config)
{
    if (!config) return UG_RESULT_FAIL;
    
    g_keyboard.config = *config;
    
    if (g_keyboard.visible) {
        _KB_CalculateLayout();
        g_keyboard.needs_redraw = 1;
    }
    
    return UG_RESULT_OK;
}

UG_RESULT UG_OSKeyboard_Show(void)
{
    g_keyboard.visible = 1;
    _KB_CalculateLayout();
    g_keyboard.needs_redraw = 1;
    return UG_RESULT_OK;
}

UG_RESULT UG_OSKeyboard_Hide(void)
{
    if (g_keyboard.visible) {
        // Clear keyboard area first
        UG_FillFrame(g_keyboard.panel_x, g_keyboard.panel_y,
                     g_keyboard.panel_x + g_keyboard.panel_w - 1,
                     g_keyboard.panel_y + g_keyboard.panel_h - 1,
                     UG_GetGUI()->desktop_color);
        
        g_keyboard.visible = 0;
        
        // Force full redraw of active window to restore content behind keyboard
        UG_WINDOW* active_wnd = UG_GetGUI()->active_window;
        if (active_wnd && (active_wnd->state & WND_STATE_VISIBLE)) {
            active_wnd->state |= WND_STATE_UPDATE;
            
            // Force all objects in the window to redraw
            for (UG_U8 i = 0; i < active_wnd->objcnt; i++) {
                UG_OBJECT* obj = &active_wnd->objlst[i];
                if ((obj->state & OBJ_STATE_VALID) && (obj->state & OBJ_STATE_VISIBLE)) {
                    obj->state |= OBJ_STATE_UPDATE | OBJ_STATE_REDRAW;
                }
            }
        }
    }
    
    return UG_RESULT_OK;
}

UG_RESULT UG_OSKeyboard_Toggle(void)
{
    if (g_keyboard.visible) {
        return UG_OSKeyboard_Hide();
    } else {
        return UG_OSKeyboard_Show();
    }
}

UG_U8 UG_OSKeyboard_IsVisible(void)
{
    return g_keyboard.visible;
}

UG_U8 UG_OSKeyboard_ProcessTouch(UG_S16 x, UG_S16 y, UG_U8 pressed)
{
    if (!g_keyboard.visible) return 0;
    
    KB_KEY* key = _KB_FindKeyAt(x, y);
    if (!key) return 0;
    
    if (pressed && !key->pressed) {
        key->pressed = 1;
        g_keyboard.needs_redraw = 1;
    } else if (!pressed && key->pressed) {
        key->pressed = 0;
        g_keyboard.needs_redraw = 1;
        _KB_SendKeyEvent(key);
    }
    
    return 1; // Event was handled
}

void UG_OSKeyboard_Update(void)
{
    if (g_keyboard.visible && g_keyboard.needs_redraw) {
        _KB_Draw();
    }
}

void UG_OSKeyboard_GetDefaultConfig(UG_KEYBOARD_CONFIG* config)
{
    if (!config) return;
    
    config->width = 0;              // Auto-size
    config->height = 0;             // Auto-size
    config->x_pos = -1;             // Center
    config->y_pos = -1;             // Bottom
    config->bg_color = C_WHITE_94;
    config->key_color = C_LIGHT_GRAY;
    config->key_text_color = C_BLACK;
    config->border_color = C_GRAY;
    config->key_spacing = 2;
    config->show_border = 1;
}