#include <sdk/calc/calc.hpp>
#include <sdk/os/input.hpp>
#include <sdk/os/lcd.hpp>
#include <stdio.h>
#include <stdlib.h>

extern "C" {
  #include "../UGUI/ugui_config.h"
  #include "../UGUI/ugui.h"
}

static  void l16_pset(UG_S16 x, UG_S16 y, UG_COLOR c) {
    setPixel(x, y, c);
}

bool l16_setup(void (**pset_fn)(UG_S16, UG_S16, UG_COLOR)) {
    fillScreen(0xFFFF);
    // calcInit();
    
    // Pass our pixel-setting function back to the caller.
    *pset_fn = l16_pset;
    
    return true; // Setup is always successful in this simple case.
}

void l16_process(void) {
    static bool is_touch_down = false;
    struct InputEvent event;

    // Poll for a single input event.
    GetInput(&event, 0xFFFFFFFF, 0x0); // Use 0 timeout for non-blocking poll

    // Process touch events for UGUI.
    if (event.type == EVENT_TOUCH) {
        if (event.data.touch_single.direction == TOUCH_DOWN) {
            is_touch_down = true;
            UG_TouchUpdate(event.data.touch_single.p1_x, event.data.touch_single.p1_y, TOUCH_STATE_PRESSED);
        } else if (event.data.touch_single.direction == TOUCH_UP) {
            is_touch_down = false;
            // When touch is released, UGUI expects -1, -1 coordinates
            UG_TouchUpdate(-1, -1, TOUCH_STATE_RELEASED);
        }
    } 
    // Handle touch movement (dragging)
    // else if (event.type == EVENT_MOVE && is_touch_down) {
    //      UG_TouchUpdate(event.data.touch_single.p1_x, event.data.touch_single.p1_y, TOUCH_STATE_PRESSED);
    // }
    
    // Process keyboard events (e.g., for exiting the app).
    if (event.type == EVENT_KEY && event.data.key.keyCode == KEYCODE_POWER_CLEAR) {
        calcEnd();
        exit(0);
    }
    
    LCD_Refresh();
}

void l16_flush(void) {
  LCD_Refresh();
}