#ifndef _STANDALONE_FONT_HPP_
#define _STANDALONE_FONT_HPP_

#include "../ugui.h"

#ifdef UGUI_USE_SYSTEM_FONT

#include <stdint.h> // For standard integer types like uint16_t
// extern const UG_FONT FONT_PEG_SYSTEM;

// --- Basic Type Definitions (to remove dependency on pegtypes.hpp) ---
typedef uint16_t WORD;
typedef uint8_t  UCHAR;
typedef char     TCHAR;

// --- Core Data Structures (from PEG) ---
typedef struct PegFont_t {
  UCHAR uType;
  UCHAR uAscent;
  UCHAR uDescent;
  UCHAR uHeight;
  WORD  wBytesPerLine;
  WORD  wFirstChar;
  WORD  wLastChar;
  WORD *pOffsets;
  void *pNext;
  UCHAR *pData;
} PegFont;

// extern const PegFont* pSystemFont;

extern const UG_FONT FONT_SYSTEM_1;
extern const UG_FONT FONT_SYSTEM_2;
extern const UG_FONT FONT_SYSTEM_3;

// const UG_FONT FONT_PEG_SYSTEM = {
//     .p = (unsigned char *)FONT_ADDRESS_1,
//     .font_type = FONT_TYPE_PEG,
//     .char_width = 0,
//     .char_height = 16, // pSystemFont->uHeight,
//     .start_char = 0,
//     .end_char = 127,
//     .widths = 0                 // Widths are calculated dynamically.
// };

typedef struct PegPoint_t {
  int x;
  int y;
} PegPoint;

// =======================================================================
//                          PUBLIC API
// =======================================================================

/**
* @brief Gets the pixel width of a single PEG character.
*/
int peg_get_char_width(TCHAR ch, const PegFont* pFont) ;

/**
* @brief Fills a buffer with the pixel data for one row of a PEG character.
* @param ch The character to get data for.
* @param row The row number (0 to font_height - 1).
* @param buffer A pointer to a buffer to be filled with 1s (solid) and 0s (transparent).
*               This buffer must be at least as large as the character's width.
*/
void peg_get_char_scanline(TCHAR ch, int row, uint8_t* buffer, const PegFont* pFont);

#endif

#endif // _STANDALONE_FONT_HPP_
