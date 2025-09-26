#include <appdef.hpp>
#include <sdk/calc/calc.hpp>
#include <sdk/os/input.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" {
#include "UGUI/ugui.h"
#include "UGUI/ugui_config.h"
#include "UGUI/ugui_messagebox.h"
}

APP_NAME("UGUI Test")
APP_DESCRIPTION("UGUI Test")
APP_AUTHOR("s3ansh33p & PC")
APP_VERSION("1.0.0")

#define INPUT_BUFFER_SIZE 50
#define PROMPT_BUFFER_SIZE 30 

#define MAX_OBJECTS 10

UG_GUI gui;

UG_WINDOW window_1;
UG_BUTTON button_1;
UG_BUTTON button_2;
UG_BUTTON button_3;
UG_TEXTBOX textbox_1;
UG_INPUT_FIELD input_field_1;
UG_SLIDER slider_1; // Horizontal slider
UG_SLIDER slider_2; // Vertical slider
UG_OBJECT obj_buff_wnd_1[MAX_OBJECTS];


// TODO: move to a special class
#define MAX_DIALOG_OBJECTS 3 

#define BTN_ID_DIALOG_YES 20
#define BTN_ID_DIALOG_NO 21
#define TXB_ID_DIALOG 20

#define MESSAGEBOX_OBJECTS 10

UG_WINDOW window_dialog;
UG_OBJECT obj_buff_wnd_dialog[MAX_DIALOG_OBJECTS];
UG_BUTTON button_yes;
UG_BUTTON button_no;
UG_TEXTBOX textbox_dialog;

volatile bool dialog_active = false;
volatile int dialog_result = 0; // 0 = no result, 1 = yes, 2 = no

// END-TODO

// wrapper for setPixel
void UGWrapper(UG_S16 x, UG_S16 y, UG_COLOR color) { setPixel(x, y, color); }


// *** NEW ***: Forward declaration for our main window
void window_1_callback(UG_MESSAGE *msg);

// *** NEW ***: Callback function for the dialog window
void dialog_callback(UG_MESSAGE *msg) {
  if (msg->type == MSG_TYPE_OBJECT) {
    if (msg->id == OBJ_TYPE_BUTTON) {
      if (msg->event == OBJ_EVENT_CLICKED || msg->event == OBJ_EVENT_PRESSED) {
        switch (msg->sub_id) {
        case BTN_ID_DIALOG_YES:
          dialog_result = 1; // 'Yes'
          dialog_active = false;
          break;
        case BTN_ID_DIALOG_NO:
          dialog_result = 2; // 'No'
          dialog_active = false;
          break;
        }
      }
    }
  }
}

// *** NEW ***: Helper function to create and show the message box
void ShowMessageBox(const char *message, const char *title) {
  if (dialog_active)
    return; // Don't show if one is already active

  // Set the text for the dialog
  UG_TextboxSetText(&window_dialog, TXB_ID_DIALOG, (char *)message);
  UG_WindowSetTitleText(&window_dialog, (char *)title);
  UG_ButtonSetText(&window_dialog, BTN_ID_DIALOG_YES, "Yes");
  UG_ButtonSetText(&window_dialog, BTN_ID_DIALOG_NO, "No");

  // Set state variables
  dialog_result = 0;
  dialog_active = true;

  // Show the dialog window (this makes it the active window)
  UG_WindowShow(&window_dialog);
  UG_Update();
  LCD_Refresh();
}

static char prompt_buffer[PROMPT_BUFFER_SIZE];

void window_1_callback(UG_MESSAGE *msg) {
  if (msg->type == MSG_TYPE_OBJECT) {
    switch (msg->id) {
    case OBJ_TYPE_INPUT_FIELD: {
      if (msg->event == EVENT_INPUTFIELD_CLICKED) {
        // Show keyboard when input field is clicked
        UG_OSKeyboard_Show();
        LCD_Refresh();
      }
      break;
    }

    case OBJ_TYPE_BUTTON: {
      if (msg->event == OBJ_EVENT_PRESSED) {
        switch (msg->sub_id) {
        case BTN_ID_0:
          // UG_MessageBoxShow("This is an information box.", "Info", MB_TYPE_INFO, NULL, 0);
          // fputs("Button 1 pressed\n", stdout);
          UG_PutString(10, 250, "Button 1 pressed");
          break;
        case BTN_ID_1:
          // UG_MessageBoxShow("Please enter your name:", "Prompt", MB_TYPE_PROMPT, prompt_buffer, PROMPT_BUFFER_SIZE);
          // fputs("Button 2 pressed\n", stdout);
          UG_PutString(10, 250, "Button 2 pressed");
          // UG_Window_SetFocus(wnd, input_obj);
          UG_OSKeyboard_Show();
          break;
        case BTN_ID_2:
          // fputs("Button 3 pressed\n", stdout);
          // UG_MessageBoxShow("Do you want to continue?", "Confirmation", MB_TYPE_CONFIRM, NULL, 0);
          UG_PutString(10, 250, "Button 3 pressed");
          ShowMessageBox("Do you want to continue?", "Confirmation");
          break;
        }
        LCD_Refresh();
      }
      break;
    }
    case OBJ_TYPE_SLIDER: {
      if (msg->event == SLD_EVENT_VALUE_CHANGED) {
        switch (msg->sub_id) {
        case SLD_ID_0: {
          // Get the current slider value
          UG_S32 value = UG_SliderGetValue((gui.active_window), SLD_ID_0);

          // Display the value (you might want to format this better)
          char value_str[32];
          snprintf(value_str, sizeof(value_str), "Slider 1: %ld", value);
          UG_PutString(10, 270, value_str);

          LCD_Refresh();
          break;
        }
        case SLD_ID_1: {
          UG_S32 value = UG_SliderGetValue((gui.active_window), SLD_ID_1);
          char value_str[32];
          snprintf(value_str, sizeof(value_str), "Slider 2: %ld", value);
          UG_PutString(10, 290, value_str);
          LCD_Refresh();
          break;
        }
        }
      }
      break;
    }
    }
  }
}

void keyboard_event_handler(UG_KEYBOARD_EVENT *event) {
  static char msg_buffer[64]; // Static buffer to hold formatted messages

  switch (event->type) {
  case VKEY_CHAR:
    snprintf(msg_buffer, sizeof(msg_buffer), "osk: %c",
             event->character);
    UG_TextboxSetText(gui.active_window, TXB_ID_0, msg_buffer);
    // Example: If you have a focused input field, append the character
    // UG_InputFieldAppendChar(focused_input_field, event->character);
    break;

  case VKEY_BACKSPACE:
    snprintf(msg_buffer, sizeof(msg_buffer), "Backspace");
    UG_TextboxSetText(gui.active_window, TXB_ID_0, msg_buffer);
    // Handle backspace - remove character from focused element
    // UG_InputFieldBackspace(focused_input_field);
    break;

  case VKEY_SPACE:
    snprintf(msg_buffer, sizeof(msg_buffer), "Space");
    UG_TextboxSetText(gui.active_window, TXB_ID_0, msg_buffer);
    // Handle space
    // UG_InputFieldAppendChar(focused_input_field, ' ');
    break;

  case VKEY_ENTER:
    snprintf(msg_buffer, sizeof(msg_buffer), "Enter");
    UG_TextboxSetText(gui.active_window, TXB_ID_0, msg_buffer);
    // Handle enter - could submit form, move to next field, etc.
    break;

  case VKEY_CAPS:
    snprintf(msg_buffer, sizeof(msg_buffer), "Caps: %s",
             event->caps_state ? "UPPER" : "lower");
    UG_TextboxSetText(gui.active_window, TXB_ID_0, msg_buffer);
    // Caps state is automatically handled by the keyboard
    break;

  case VKEY_HIDE:
    snprintf(msg_buffer, sizeof(msg_buffer), "Hide osk");
    UG_TextboxSetText(gui.active_window, TXB_ID_0, msg_buffer);
    UG_OSKeyboard_Hide();
    break;

  default:
    break;
  }

  // Force screen refresh after handling keyboard event
  UG_Update();
  LCD_Refresh();
}

extern "C" int __attribute__((section(".bootstrap.text"))) main(void) {
  calcInit();

  struct InputEvent event;

  /* Initialize the GUI */

  UG_Init(&gui, UGWrapper, width, height);

  UG_OSKeyboard_Init(&gui, keyboard_event_handler);

  UG_FontSelect(&FONT_8X8);

  UG_KEYBOARD_CONFIG kb_config;
  UG_OSKeyboard_GetDefaultConfig(&kb_config);
  kb_config.bg_color = C_WHITE;
  kb_config.key_color = C_LIGHT_GRAY;
  kb_config.key_text_color = C_BLACK;
  kb_config.key_spacing = 3;
  UG_OSKeyboard_Configure(&kb_config);

  char g_input_buffer[INPUT_BUFFER_SIZE];

  // fill background
  UG_FillScreen(C_BLACK);

  UG_WindowCreate(&window_1, obj_buff_wnd_1, MAX_OBJECTS, window_1_callback);

  UG_WindowSetTitleText(&window_1, "Test Window Title");
  UG_WindowSetTitleTextFont(&window_1, &FONT_SYSTEM_2);

  UG_ButtonCreate(&window_1, &button_1, BTN_ID_0, 10, 10, 110, 60);
  UG_ButtonSetFont(&window_1, BTN_ID_0, &FONT_SYSTEM_1);
  UG_ButtonSetText(&window_1, BTN_ID_0,
                   "Sys1 is a long text"); // TODO: CLIP LONG TEXT

  UG_ButtonCreate(&window_1, &button_2, BTN_ID_1, 10, 80, 110, 130);
  UG_ButtonSetFont(&window_1, BTN_ID_1, &FONT_SYSTEM_2);
  UG_ButtonSetText(&window_1, BTN_ID_1, "Sys2");

  UG_ButtonCreate(&window_1, &button_3, BTN_ID_2, 10, 150, 110, 200);
  UG_ButtonSetFont(&window_1, BTN_ID_2, &FONT_SYSTEM_3);
  UG_ButtonSetText(&window_1, BTN_ID_2, "Sys3");

  // Create horizontal slider
  // UG_SliderCreate( &window_1, &slider_1, SLD_ID_0, 120, 210, 300, 240 );
  // UG_SliderSetRange( &window_1, SLD_ID_0, 0, 100 );           // Range 0-100
  // UG_SliderSetValue( &window_1, SLD_ID_0, 50 );               // Initial
  // value UG_SliderSetTickInterval( &window_1, SLD_ID_0, 10 );        // Tick
  // every 10 units UG_SliderSetStyle( &window_1, SLD_ID_0, SLD_STYLE_3D ); //
  // 3D style with ticks

  // // Create vertical slider
  // UG_SliderCreate( &window_1, &slider_2, SLD_ID_1, 320, 10, 350, 200 );
  // UG_SliderSetRange( &window_1, SLD_ID_1, -50, 50 );          // Range -50 to
  // +50 UG_SliderSetValue( &window_1, SLD_ID_1, 0 );                // Initial
  // value at center UG_SliderSetTickInterval( &window_1, SLD_ID_1, 25 ); //
  // Tick every 25 units UG_SliderSetStyle( &window_1, SLD_ID_1, SLD_STYLE_3D |
  // SLD_STYLE_VERTICAL );

  // // Optional: Customize slider colors
  // UG_SliderSetTrackColor( &window_1, SLD_ID_0, C_DARK_GRAY );
  // UG_SliderSetHandleColor( &window_1, SLD_ID_0, C_BLUE );
  // UG_SliderSetTickColor( &window_1, SLD_ID_0, C_WHITE );

  // UG_SliderSetTrackColor( &window_1, SLD_ID_1, C_DARK_GRAY );
  // UG_SliderSetHandleColor( &window_1, SLD_ID_1, C_RED );
  // UG_SliderSetTickColor( &window_1, SLD_ID_1, C_WHITE );

  UG_FontSelect(&FONT_SYSTEM_1);
  // UG_InputFieldCreate( &window_1, &input_field_1, INPUT_ID_0, 120, 110, 280,
  // 150, g_input_buffer, sizeof(g_input_buffer));
  // UG_InputFieldSetText(&window_1, INPUT_ID_0, "Input field");

  UG_TextboxCreate(&window_1, &textbox_1, TXB_ID_0, 120, 10, 310, 200);
  UG_TextboxSetFont(&window_1, TXB_ID_0, &FONT_8X12);
  UG_TextboxSetText(&window_1, TXB_ID_0, "This is a test textbox\nSecond line");
  UG_TextboxSetForeColor(&window_1, TXB_ID_0, C_BLACK);
  UG_TextboxSetAlignment(&window_1, TXB_ID_0, ALIGN_CENTER);

  // UG_FontSelect( &FONT_8X8 );

  UG_FontSelect(&FONT_SYSTEM_1);

  UG_InputFieldCreate(&window_1, &input_field_1, INPUT_ID_0, 120, 40, 280, 80,
                      g_input_buffer, sizeof(g_input_buffer));
  UG_InputFieldSetText(&window_1, INPUT_ID_0, "Click me!");

  // TODO: move to another file ?
   // Center the dialog window
  UG_S16 dialog_w = 310;
  UG_S16 dialog_h = 140;
  UG_S16 dialog_x = (width - dialog_w) / 2;
  UG_S16 dialog_y = (height - dialog_h) / 2;

  UG_WindowCreate(&window_dialog, obj_buff_wnd_dialog, MAX_DIALOG_OBJECTS, dialog_callback);
  UG_WindowResize(&window_dialog, dialog_x, dialog_y, dialog_x + dialog_w, dialog_y + dialog_h);
  UG_WindowSetTitleTextFont(&window_dialog, &FONT_SYSTEM_2);

  // Create the message textbox for the dialog
  UG_TextboxCreate(&window_dialog, &textbox_dialog, TXB_ID_DIALOG, 5, 5, dialog_w - 10, 50);
  UG_TextboxSetFont(&window_dialog, TXB_ID_DIALOG, &FONT_SYSTEM_2);
  UG_TextboxSetAlignment(&window_dialog, TXB_ID_DIALOG, ALIGN_TOP_LEFT);

  // Create "Yes" and "No" buttons
  UG_S16 btn_w = 96;
  UG_S16 btn_h = 40;
  UG_S16 btn_padding = 6;
  UG_S16 btn_y = 66;
  UG_S16 btn_x_yes = btn_padding; // (dialog_w / 2) - btn_w - btn_padding;
  UG_S16 btn_x_no = dialog_w - btn_padding*2 - btn_w; // (dialog_w / 2) + btn_padding;

  UG_ButtonCreate(&window_dialog, &button_yes, BTN_ID_DIALOG_YES, btn_x_yes, btn_y, btn_x_yes + btn_w, btn_y + btn_h);
  UG_ButtonSetText(&window_dialog, BTN_ID_DIALOG_YES, "Yes");
  UG_ButtonSetFont(&window_dialog, BTN_ID_DIALOG_YES, &FONT_SYSTEM_2);

  UG_ButtonCreate(&window_dialog, &button_no, BTN_ID_DIALOG_NO, btn_x_no, btn_y, btn_x_no + btn_w, btn_y + btn_h);
  UG_ButtonSetText(&window_dialog, BTN_ID_DIALOG_NO, "No");
  UG_ButtonSetFont(&window_dialog, BTN_ID_DIALOG_NO, &FONT_SYSTEM_2);

  UG_WindowHide(&window_dialog);
  // END-TODO

  UG_WindowShow(&window_1);

  UG_Update();

  LCD_Refresh();

  bool running = true;
  while (running) {
    GetInput(&event, 0xFFFFFFFF, 0x10); // polls

    if (!dialog_active && dialog_result != 0) {
        // 1. Hide the dialog window
        UG_WindowHide(&window_dialog);

        // 2. Re-show the main window to make it active again
        UG_WindowShow(&window_1);

        // 3. Process the result
        if (dialog_result == 1) { // Yes
            UG_TextboxSetText(&window_1, TXB_ID_0, "You clicked 'Yes'!");
        } else if (dialog_result == 2) { // No
            UG_TextboxSetText(&window_1, TXB_ID_0, "You clicked 'No'.");
        }

        // 4. Reset the result for the next time
        dialog_result = 0;
        
        UG_Update(); // Force a redraw of the main window
    }
    switch (event.type) {
    case EVENT_TOUCH:
      if (event.data.touch_single.direction == TOUCH_DOWN) {
        // First, let the keyboard handle the touch
        if (UG_OSKeyboard_ProcessTouch(event.data.touch_single.p1_x,
                                       event.data.touch_single.p1_y, 1)) {
          // Touch was handled by keyboard, don't process further
          break;
        }
        UG_TouchUpdate(event.data.touch_single.p1_x,
                       event.data.touch_single.p1_y, TOUCH_STATE_PRESSED);
      } else if (event.data.touch_single.direction == TOUCH_UP) {
        // Handle touch release
        if (UG_OSKeyboard_ProcessTouch(event.data.touch_single.p1_x,
                                       event.data.touch_single.p1_y, 0)) {
          // Touch release was handled by keyboard
          break;
        }
        UG_TouchUpdate(event.data.touch_single.p1_x,
                       event.data.touch_single.p1_y, TOUCH_STATE_RELEASED);
      }
      UG_Update();
      break;
    case EVENT_KEY:
      if (event.data.key.keyCode == KEYCODE_POWER_CLEAR) {
        running = false;
      } else if (event.data.key.direction == KEY_PRESSED) {
        // Handle hardware keyboard toggle
        if (event.data.key.keyCode == KEYCODE_KEYBOARD) {
          UG_OSKeyboard_Toggle();
          UG_Update();
          break;
        }

        // Hide keyboard on ESC/Back if visible
        if (UG_OSKeyboard_IsVisible() &&
            (event.data.key.keyCode == KEYCODE_EXE)) {
          UG_OSKeyboard_Hide();
          UG_Update();
          break;
        }
        UG_WINDOW *current_wnd = dialog_active ? &window_dialog : &window_1;
        if (current_wnd) {
          switch (event.data.key.keyCode) {
          case KEYCODE_UP:
          case KEYCODE_LEFT:
            UG_Window_FocusNext(current_wnd, -1);
            UG_Update();
            break;
          case KEYCODE_DOWN:
          case KEYCODE_RIGHT:
            UG_Window_FocusNext(current_wnd, 1);
            UG_Update();
            break;
          case KEYCODE_EXE:
            if (current_wnd->focused_obj) {
              if (current_wnd->focused_obj->type == OBJ_TYPE_INPUT_FIELD) {
                // Show keyboard for input fields
                UG_OSKeyboard_Show();
              } else if (current_wnd->focused_obj->type == OBJ_TYPE_SLIDER) {
                UG_SLIDER *sld = (UG_SLIDER *)(current_wnd->focused_obj->data);
                UG_S32 current_value = sld->current_value;
                UG_S32 step = ((sld->max_value - sld->min_value) / 10) %
                              sld->max_value; // 10% step
                if (step < 1)
                  step = 1;

                UG_SliderSetValue(&window_1, current_wnd->focused_obj->id,
                                  current_value + step);
              } else {
                // Handle other objects as before
                current_wnd->focused_obj->event = OBJ_EVENT_PRESSED;
                _UG_HandleEvents(current_wnd);
                current_wnd->focused_obj->event = OBJ_EVENT_RELEASED;
                _UG_HandleEvents(current_wnd);
                current_wnd->focused_obj->event = OBJ_EVENT_CLICKED;
                _UG_HandleEvents(current_wnd);
              }
              UG_Update();
            }
            break;
          case KEYCODE_KEYBOARD:
                if(!dialog_active) UG_OSKeyboard_Toggle();
            UG_Update();
            // TODO: update ?
            break;
          }
        }
      }
      break;
    }

    UG_OSKeyboard_Update();
    LCD_Refresh();
  }

  // while(true){
  // 	uint32_t key1, key2;	//First create variables
  // 	getKey(&key1, &key2);	//then read the keys
  // 	if(testKey(key1, key2, KEY_CLEAR)){ //Use testKey() to test if a
  // specific key is pressed 		break;
  // 	}
  // }
  calcEnd();
  exit(EXIT_SUCCESS);
}

// You can also create helper functions to show keyboard programmatically:
// void show_keyboard_for_input(UG_OBJECT* input_obj) {
//     // Set focus to the input field
//     UG_WINDOW* wnd = &window_1; // or find the correct window
//     UG_Window_SetFocus(wnd, input_obj);

//     // Show the keyboard
//     UG_OSKeyboard_Show();
// }
