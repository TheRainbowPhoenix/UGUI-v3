#include <string.h>
#include "ugui_config.h"
#include "ugui_messagebox.h"

#include "ugui_button.h"
#include "ugui_inputfield.h"
#include "ugui_oskeyboard.h"
#include "ugui_textbox.h"
#include "ugui_window.h"
#include "ugui_fonts.h"

#define MB_MAX_OBJECTS 5
#define MB_TEXTBOX_ID OBJ_ID_0
#define MB_PRIMARY_BUTTON_ID OBJ_ID_1
#define MB_SECONDARY_BUTTON_ID OBJ_ID_2
#define MB_INPUT_ID OBJ_ID_3

#define MB_DIALOG_WIDTH 310
#define MB_DIALOG_HEIGHT 140
#define MB_TEXTBOX_XS 5
#define MB_TEXTBOX_YS 5
#define MB_TEXTBOX_XE (MB_DIALOG_WIDTH - 10)
#define MB_TEXTBOX_YE 50
#define MB_BUTTON_WIDTH 96
#define MB_BUTTON_HEIGHT 40
#define MB_BUTTON_PADDING 6
#define MB_BUTTON_X_PRIMARY MB_BUTTON_PADDING
#define MB_BUTTON_X_SECONDARY (MB_DIALOG_WIDTH - (MB_BUTTON_PADDING * 2) - MB_BUTTON_WIDTH)
#define MB_BUTTON_Y_CONFIRM 66
#define MB_BUTTON_Y_PROMPT (MB_DIALOG_HEIGHT - MB_BUTTON_HEIGHT - MB_BUTTON_PADDING)
#define MB_INPUTFIELD_XS MB_BUTTON_PADDING
#define MB_INPUTFIELD_YS (MB_TEXTBOX_YE + 8)
#define MB_INPUTFIELD_XE (MB_DIALOG_WIDTH - MB_BUTTON_PADDING)
#define MB_INPUTFIELD_YE (MB_BUTTON_Y_PROMPT - 8)
#define MB_PROMPT_BUFFER_LENGTH 64

static UG_GUI *mb_gui = NULL;
static UG_WINDOW mb_window;
static UG_OBJECT mb_objects[MB_MAX_OBJECTS];
static UG_BUTTON mb_primary_button;
static UG_BUTTON mb_secondary_button;
static UG_TEXTBOX mb_textbox;
static UG_INPUT_FIELD mb_input_field;
static char mb_prompt_buffer[MB_PROMPT_BUFFER_LENGTH];

static struct {
    UG_U8 active;
    UG_WINDOW *previous_window;
    UG_MESSAGEBOX_STYLE style;
    UG_MESSAGEBOX_STYLE last_style;
    UG_MESSAGEBOX_RESULT result;
    char *prompt_target;
    UG_U16 prompt_target_length;
} mb_state = {0};

static void mb_set_object_visibility(UG_WINDOW *wnd, UG_U8 type, UG_U8 id, UG_U8 visible)
{
    UG_OBJECT *obj = _UG_SearchObject(wnd, type, id);
    if (obj == NULL) {
        return;
    }

    if (visible) {
        obj->state |= (OBJ_STATE_VISIBLE | OBJ_STATE_UPDATE | OBJ_STATE_REDRAW);
    } else {
        obj->state &= ~OBJ_STATE_VISIBLE;
        obj->state |= (OBJ_STATE_UPDATE | OBJ_STATE_REDRAW);
    }
}

static void mb_set_button_area(UG_U8 id, UG_S16 xs, UG_S16 ys, UG_S16 xe, UG_S16 ye)
{
    UG_OBJECT *obj = _UG_SearchObject(&mb_window, OBJ_TYPE_BUTTON, id);
    if (obj == NULL) {
        return;
    }

    obj->a_rel.xs = xs;
    obj->a_rel.ys = ys;
    obj->a_rel.xe = xe;
    obj->a_rel.ye = ye;
    obj->state |= (OBJ_STATE_UPDATE | OBJ_STATE_REDRAW);
}

static void mb_focus_object(UG_U8 type, UG_U8 id)
{
    UG_OBJECT *obj = _UG_SearchObject(&mb_window, type, id);
    if (obj != NULL) {
        UG_Window_SetFocus(&mb_window, obj);
    }
}

static void mb_close(UG_MESSAGEBOX_RESULT result)
{
    if (!mb_state.active) {
        return;
    }

    if (mb_state.style == UG_MESSAGEBOX_STYLE_PROMPT) {
        const char *text = UG_InputFieldGetText(&mb_window, MB_INPUT_ID);
        if (text == NULL) {
            text = "";
        }
        if (result == UG_MESSAGEBOX_RESULT_OK && mb_state.prompt_target != NULL && mb_state.prompt_target_length > 0) {
            if (mb_state.prompt_target_length == 1) {
                mb_state.prompt_target[0] = '\0';
            } else {
                size_t copy_len = mb_state.prompt_target_length - 1;
                if (copy_len > MB_PROMPT_BUFFER_LENGTH - 1) {
                    copy_len = MB_PROMPT_BUFFER_LENGTH - 1;
                }
                strncpy(mb_state.prompt_target, text, copy_len);
                mb_state.prompt_target[copy_len] = '\0';
            }
        }
        UG_OSKeyboard_Hide();
    }

    UG_WindowHide(&mb_window);
    if (mb_state.previous_window != NULL) {
        UG_WindowShow(mb_state.previous_window);
    }

    mb_state.active = 0;
    mb_state.previous_window = NULL;
    mb_state.result = result;
    mb_state.last_style = mb_state.style;
    mb_state.prompt_target = NULL;
    mb_state.prompt_target_length = 0;
}

static void mb_window_callback(UG_MESSAGE *msg)
{
    if (msg->type != MSG_TYPE_OBJECT) {
        return;
    }

    if (msg->id != OBJ_TYPE_BUTTON) {
        return;
    }

    if (msg->event != OBJ_EVENT_CLICKED && msg->event != OBJ_EVENT_PRESSED) {
        return;
    }

    switch (msg->sub_id) {
    case MB_PRIMARY_BUTTON_ID:
        if (mb_state.style == UG_MESSAGEBOX_STYLE_CONFIRM) {
            mb_close(UG_MESSAGEBOX_RESULT_YES);
        } else {
            mb_close(UG_MESSAGEBOX_RESULT_OK);
        }
        break;
    case MB_SECONDARY_BUTTON_ID:
        if (mb_state.style == UG_MESSAGEBOX_STYLE_CONFIRM) {
            mb_close(UG_MESSAGEBOX_RESULT_NO);
        } else {
            mb_close(UG_MESSAGEBOX_RESULT_CANCEL);
        }
        break;
    default:
        break;
    }
}

static void mb_prepare_window(const char *text, const char *title)
{
    if (title != NULL) {
        UG_WindowSetTitleText(&mb_window, (char *)title);
    }

    if (text != NULL) {
        UG_TextboxSetText(&mb_window, MB_TEXTBOX_ID, (char *)text);
    }

    UG_ButtonShow(&mb_window, MB_PRIMARY_BUTTON_ID);
    mb_set_object_visibility(&mb_window, OBJ_TYPE_INPUT_FIELD, MB_INPUT_ID, 0);
}

void UG_MessageBox_Init(UG_GUI *gui)
{
    if (gui == NULL) {
        return;
    }

    mb_gui = gui;
    mb_state.active = 0;
    mb_state.previous_window = NULL;
    mb_state.result = UG_MESSAGEBOX_RESULT_NONE;
    mb_state.last_style = UG_MESSAGEBOX_STYLE_INFO;

    UG_WindowCreate(&mb_window, mb_objects, MB_MAX_OBJECTS, mb_window_callback);

    UG_S16 dialog_x = (UG_GetXDim() - MB_DIALOG_WIDTH) / 2;
    UG_S16 dialog_y = (UG_GetYDim() - MB_DIALOG_HEIGHT) / 2;
    UG_WindowResize(&mb_window, dialog_x, dialog_y, dialog_x + MB_DIALOG_WIDTH, dialog_y + MB_DIALOG_HEIGHT);
    UG_WindowSetTitleTextFont(&mb_window, &FONT_SYSTEM_2);

    UG_TextboxCreate(&mb_window, &mb_textbox, MB_TEXTBOX_ID, MB_TEXTBOX_XS, MB_TEXTBOX_YS,
                     MB_TEXTBOX_XE, MB_TEXTBOX_YE);
    UG_TextboxSetFont(&mb_window, MB_TEXTBOX_ID, &FONT_SYSTEM_2);
    UG_TextboxSetAlignment(&mb_window, MB_TEXTBOX_ID, ALIGN_TOP_LEFT);

    UG_ButtonCreate(&mb_window, &mb_primary_button, MB_PRIMARY_BUTTON_ID,
                    MB_BUTTON_X_PRIMARY, MB_BUTTON_Y_CONFIRM,
                    MB_BUTTON_X_PRIMARY + MB_BUTTON_WIDTH, MB_BUTTON_Y_CONFIRM + MB_BUTTON_HEIGHT);
    UG_ButtonSetFont(&mb_window, MB_PRIMARY_BUTTON_ID, &FONT_SYSTEM_2);
    UG_ButtonSetText(&mb_window, MB_PRIMARY_BUTTON_ID, "OK");

    UG_ButtonCreate(&mb_window, &mb_secondary_button, MB_SECONDARY_BUTTON_ID,
                    MB_BUTTON_X_SECONDARY,
                    MB_BUTTON_Y_CONFIRM,
                    MB_BUTTON_X_SECONDARY + MB_BUTTON_WIDTH,
                    MB_BUTTON_Y_CONFIRM + MB_BUTTON_HEIGHT);
    UG_ButtonSetFont(&mb_window, MB_SECONDARY_BUTTON_ID, &FONT_SYSTEM_2);
    UG_ButtonSetText(&mb_window, MB_SECONDARY_BUTTON_ID, "Cancel");

    UG_InputFieldCreate(&mb_window, &mb_input_field, MB_INPUT_ID,
                        MB_INPUTFIELD_XS,
                        MB_INPUTFIELD_YS,
                        MB_INPUTFIELD_XE,
                        MB_INPUTFIELD_YE,
                        mb_prompt_buffer, sizeof(mb_prompt_buffer));
    mb_prompt_buffer[0] = '\0';
    UG_InputFieldSetText(&mb_window, MB_INPUT_ID, mb_prompt_buffer);
    mb_set_object_visibility(&mb_window, OBJ_TYPE_INPUT_FIELD, MB_INPUT_ID, 0);

    UG_WindowHide(&mb_window);
}

UG_U8 UG_MessageBox_IsActive(void)
{
    return mb_state.active;
}

UG_WINDOW *UG_MessageBox_GetWindow(void)
{
    return &mb_window;
}

UG_MESSAGEBOX_RESULT UG_MessageBox_GetResult(void)
{
    UG_MESSAGEBOX_RESULT result = mb_state.result;
    mb_state.result = UG_MESSAGEBOX_RESULT_NONE;
    return result;
}

UG_MESSAGEBOX_STYLE UG_MessageBox_GetLastStyle(void)
{
    return mb_state.last_style;
}

UG_MESSAGEBOX_STYLE UG_MessageBox_GetActiveStyle(void)
{
    if (mb_state.active) {
        return mb_state.style;
    }
    return mb_state.last_style;
}

const char *UG_MessageBox_GetPromptText(void)
{
    return UG_InputFieldGetText(&mb_window, MB_INPUT_ID);
}

void UG_MessageBox_ShowInfo(const char *text, const char *title)
{
    if (mb_gui == NULL || mb_state.active) {
        return;
    }

    mb_state.style = UG_MESSAGEBOX_STYLE_INFO;
    mb_state.result = UG_MESSAGEBOX_RESULT_NONE;
    mb_state.previous_window = mb_gui->active_window;
    mb_state.active = 1;

    mb_prepare_window(text, title);

    UG_ButtonSetText(&mb_window, MB_PRIMARY_BUTTON_ID, "OK");

    UG_ButtonHide(&mb_window, MB_SECONDARY_BUTTON_ID);

    UG_S16 center_x = (MB_DIALOG_WIDTH - MB_BUTTON_WIDTH) / 2;
    mb_set_button_area(MB_PRIMARY_BUTTON_ID,
                       center_x,
                       MB_BUTTON_Y_CONFIRM,
                       center_x + MB_BUTTON_WIDTH,
                       MB_BUTTON_Y_CONFIRM + MB_BUTTON_HEIGHT);
    mb_focus_object(OBJ_TYPE_BUTTON, MB_PRIMARY_BUTTON_ID);

    UG_WindowShow(&mb_window);
}

void UG_MessageBox_ShowConfirm(const char *text, const char *title)
{
    if (mb_gui == NULL || mb_state.active) {
        return;
    }

    mb_state.style = UG_MESSAGEBOX_STYLE_CONFIRM;
    mb_state.result = UG_MESSAGEBOX_RESULT_NONE;
    mb_state.previous_window = mb_gui->active_window;
    mb_state.active = 1;

    mb_prepare_window(text, title);

    UG_ButtonSetText(&mb_window, MB_PRIMARY_BUTTON_ID, "Yes");
    UG_ButtonSetText(&mb_window, MB_SECONDARY_BUTTON_ID, "No");
    UG_ButtonShow(&mb_window, MB_SECONDARY_BUTTON_ID);

    mb_set_button_area(MB_PRIMARY_BUTTON_ID,
                       MB_BUTTON_X_PRIMARY,
                       MB_BUTTON_Y_CONFIRM,
                       MB_BUTTON_X_PRIMARY + MB_BUTTON_WIDTH,
                       MB_BUTTON_Y_CONFIRM + MB_BUTTON_HEIGHT);
    mb_set_button_area(MB_SECONDARY_BUTTON_ID,
                       MB_BUTTON_X_SECONDARY,
                       MB_BUTTON_Y_CONFIRM,
                       MB_BUTTON_X_SECONDARY + MB_BUTTON_WIDTH,
                       MB_BUTTON_Y_CONFIRM + MB_BUTTON_HEIGHT);

    mb_focus_object(OBJ_TYPE_BUTTON, MB_PRIMARY_BUTTON_ID);

    UG_WindowShow(&mb_window);
}

void UG_MessageBox_ShowPrompt(const char *text, const char *title, char *buffer, UG_U16 buffer_len)
{
    if (mb_gui == NULL || mb_state.active) {
        return;
    }

    mb_state.style = UG_MESSAGEBOX_STYLE_PROMPT;
    mb_state.result = UG_MESSAGEBOX_RESULT_NONE;
    mb_state.previous_window = mb_gui->active_window;
    mb_state.active = 1;
    mb_state.prompt_target = buffer;
    mb_state.prompt_target_length = buffer_len;

    mb_prepare_window(text, title);

    UG_ButtonSetText(&mb_window, MB_PRIMARY_BUTTON_ID, "OK");
    UG_ButtonSetText(&mb_window, MB_SECONDARY_BUTTON_ID, "Cancel");
    UG_ButtonShow(&mb_window, MB_SECONDARY_BUTTON_ID);

    mb_set_button_area(MB_PRIMARY_BUTTON_ID,
                       MB_BUTTON_X_PRIMARY,
                       MB_BUTTON_Y_PROMPT,
                       MB_BUTTON_X_PRIMARY + MB_BUTTON_WIDTH,
                       MB_BUTTON_Y_PROMPT + MB_BUTTON_HEIGHT);
    mb_set_button_area(MB_SECONDARY_BUTTON_ID,
                       MB_BUTTON_X_SECONDARY,
                       MB_BUTTON_Y_PROMPT,
                       MB_BUTTON_X_SECONDARY + MB_BUTTON_WIDTH,
                       MB_BUTTON_Y_PROMPT + MB_BUTTON_HEIGHT);

    if (buffer != NULL && buffer_len > 1) {
        size_t copy_len = buffer_len - 1;
        if (copy_len > MB_PROMPT_BUFFER_LENGTH - 1) {
            copy_len = MB_PROMPT_BUFFER_LENGTH - 1;
        }
        strncpy(mb_prompt_buffer, buffer, copy_len);
        mb_prompt_buffer[copy_len] = '\0';
        UG_InputFieldSetText(&mb_window, MB_INPUT_ID, mb_prompt_buffer);
    } else {
        mb_prompt_buffer[0] = '\0';
        UG_InputFieldSetText(&mb_window, MB_INPUT_ID, mb_prompt_buffer);
    }

    mb_set_object_visibility(&mb_window, OBJ_TYPE_INPUT_FIELD, MB_INPUT_ID, 1);
    mb_focus_object(OBJ_TYPE_INPUT_FIELD, MB_INPUT_ID);

    UG_WindowShow(&mb_window);
    UG_OSKeyboard_Show();
}

void UG_MessageBox_SubmitPrimary(void)
{
    if (!mb_state.active) {
        return;
    }

    switch (mb_state.style) {
    case UG_MESSAGEBOX_STYLE_CONFIRM:
        mb_close(UG_MESSAGEBOX_RESULT_YES);
        break;
    case UG_MESSAGEBOX_STYLE_PROMPT:
        mb_close(UG_MESSAGEBOX_RESULT_OK);
        break;
    case UG_MESSAGEBOX_STYLE_INFO:
    default:
        mb_close(UG_MESSAGEBOX_RESULT_OK);
        break;
    }
}

void UG_MessageBox_SubmitSecondary(void)
{
    if (!mb_state.active) {
        return;
    }

    switch (mb_state.style) {
    case UG_MESSAGEBOX_STYLE_CONFIRM:
        mb_close(UG_MESSAGEBOX_RESULT_NO);
        break;
    case UG_MESSAGEBOX_STYLE_PROMPT:
        mb_close(UG_MESSAGEBOX_RESULT_CANCEL);
        break;
    case UG_MESSAGEBOX_STYLE_INFO:
    default:
        mb_close(UG_MESSAGEBOX_RESULT_CANCEL);
        break;
    }
}