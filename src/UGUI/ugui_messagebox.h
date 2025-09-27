#ifndef __UGUI_MESSAGEBOX_H
#define __UGUI_MESSAGEBOX_H

#include "ugui.h"
#include "ugui_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    UG_MESSAGEBOX_STYLE_INFO = 0,
    UG_MESSAGEBOX_STYLE_CONFIRM,
    UG_MESSAGEBOX_STYLE_PROMPT
} UG_MESSAGEBOX_STYLE;

typedef enum {
    UG_MESSAGEBOX_RESULT_NONE = 0,
    UG_MESSAGEBOX_RESULT_OK,
    UG_MESSAGEBOX_RESULT_CANCEL,
    UG_MESSAGEBOX_RESULT_YES,
    UG_MESSAGEBOX_RESULT_NO
} UG_MESSAGEBOX_RESULT;

void UG_MessageBox_Init(UG_GUI *gui);
UG_U8 UG_MessageBox_IsActive(void);
UG_WINDOW *UG_MessageBox_GetWindow(void);
UG_MESSAGEBOX_RESULT UG_MessageBox_GetResult(void);
UG_MESSAGEBOX_STYLE UG_MessageBox_GetLastStyle(void);
UG_MESSAGEBOX_STYLE UG_MessageBox_GetActiveStyle(void);
const char *UG_MessageBox_GetPromptText(void);

void UG_MessageBox_ShowInfo(const char *text, const char *title);
void UG_MessageBox_ShowConfirm(const char *text, const char *title);
void UG_MessageBox_ShowPrompt(const char *text, const char *title, char *buffer, UG_U16 buffer_len);

void UG_MessageBox_SubmitPrimary(void);
void UG_MessageBox_SubmitSecondary(void);

#ifdef __cplusplus
}
#endif

#endif