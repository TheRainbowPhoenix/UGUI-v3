#ifndef __UGUI_FONTS_DATA_H
#define __UGUI_FONTS_DATA_H

/* -------------------------------------------------------------------------------- */
/* -- µGUI FONTS                                                                 -- */
/* -- Source: http://www.mikrocontroller.net/user/show/benedikt                  -- */
/* -------------------------------------------------------------------------------- */
#include "ugui.h"
/*
  Old fonts converted to new structure. They use Code Page 850 encoding. UTF²-8 can be disabled if only these fonts are being used.
  https://en.wikipedia.org/wiki/Code_page_850
*/

#ifdef UGUI_USE_FONT_4X6
   extern const UG_FONT FONT_4X6;
#endif
#ifdef UGUI_USE_FONT_5X8
   extern const UG_FONT FONT_5X8;
#endif
#ifdef UGUI_USE_FONT_5X12
   extern const UG_FONT FONT_5X12;
#endif
#ifdef UGUI_USE_FONT_6X8
   extern const UG_FONT FONT_6X8;
#endif
#ifdef UGUI_USE_FONT_6X10
   extern const UG_FONT FONT_6X10;
#endif
#ifdef UGUI_USE_FONT_7X12
   extern const UG_FONT FONT_7X12;
#endif
#ifdef UGUI_USE_FONT_8X8
    extern const UG_FONT FONT_8X8; 
#endif
#ifdef UGUI_USE_FONT_8X12
    extern const UG_FONT FONT_8X12; 
#endif
#ifdef UGUI_USE_FONT_8X12_CYRILLIC
   extern const UG_FONT FONT_8X12;
#endif
#ifdef UGUI_USE_FONT_8X14
   extern const UG_FONT FONT_8X14;
#endif
#ifdef UGUI_USE_FONT_10X16
   extern const UG_FONT FONT_10X16;
#endif
#ifdef UGUI_USE_FONT_12X16
   extern const UG_FONT FONT_12X16;
#endif
#ifdef UGUI_USE_FONT_12X20
   extern const UG_FONT FONT_12X20;
#endif
#ifdef UGUI_USE_FONT_16X26
   extern const UG_FONT FONT_16X26;
#endif

#ifdef UGUI_USE_SYSTEM_FONT
    #include "./Fonts/system_font.h"
    extern const UG_FONT FONT_SYSTEM_1;
    extern const UG_FONT FONT_SYSTEM_2;
    extern const UG_FONT FONT_SYSTEM_3;

    extern int peg_get_char_width(char ch, const PegFont* pFont);
    extern void peg_draw_char(char ch, int x, int y, uint16_t fc, uint16_t bc, const PegFont* pFont);
#endif





#endif // __UGUI_FONTS_DATA_H
