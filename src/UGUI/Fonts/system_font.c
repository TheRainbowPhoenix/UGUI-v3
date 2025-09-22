#include "../ugui.h"

#include "system_font.h"

#ifdef UGUI_USE_SYSTEM_FONT

// --- Internal Helper Function ---

#define FONT_ADDRESS_1 0x8c1a70cc
#define FONT_ADDRESS_2 0x8c1a718c
#define FONT_ADDRESS_3 0x8c1a71a4

const PegFont* pSystemFont1 = (const PegFont*)FONT_ADDRESS_1;
const PegFont* pSystemFont2 = (const PegFont*)FONT_ADDRESS_2;
const PegFont* pSystemFont3 = (const PegFont*)FONT_ADDRESS_3;

const UG_FONT FONT_SYSTEM_1 = {
    .p = (unsigned char*)FONT_ADDRESS_1,
    .font_type = FONT_TYPE_PEG,
    .char_width = 0,
    .char_height = 16, // Should be ream from pSystemFont1->uHeight
    .start_char = 0,
    .end_char = 127, 
    .widths = 0
};

const UG_FONT FONT_SYSTEM_2 = {
    .p = (unsigned char*)FONT_ADDRESS_2,
    .font_type = FONT_TYPE_PEG,
    .char_width = 0,
    .char_height = 16, // Should be read from pSystemFont2->uHeight
    .start_char = 0,
    .end_char = 127,
    .widths = 0
};

const UG_FONT FONT_SYSTEM_3 = {
    .p = (unsigned char*)FONT_ADDRESS_3,
    .font_type = FONT_TYPE_PEG,
    .char_width = 0,
    .char_height = 16, // Should be read from pSystemFont3->uHeight
    .start_char = 0,
    .end_char = 127,
    .widths = 0
};

/**
 * @brief Gets the pixel width of a single character.
 * @param ch The character to measure.
 * @param pFont The font to use.
 * @return The width of the character in pixels.
 */
static int _getCharPixelWidth(TCHAR ch, const PegFont* pFont) {
    if (!pFont || ch < pFont->wFirstChar || ch >= pFont->wLastChar) {
        return 0;
    }
    int charIndex = ch - pFont->wFirstChar;
    // The width is the difference between the next char's offset and this char's offset.
    return pFont->pOffsets[charIndex + 1] - pFont->pOffsets[charIndex];
}

/**
 * @brief Draws a single 1bpp character from the pitched font atlas.
 * @param framebuffer The destination buffer.
 * @param buffer_width The pitch of the destination buffer.
 * @param pos The top-left position to draw the character.
 * @param ch The character to draw.
 * @param color The color to use.
 * @param pFont The font to use.
 */
static void _drawChar(uint16_t* framebuffer, int buffer_width, 
                      PegPoint pos, TCHAR ch, 
                      uint16_t color, const PegFont* pFont)
{
    int width = _getCharPixelWidth(ch, pFont);
    if (width <= 0) {
        return;
    }

    int startByteOffset = pFont->pOffsets[ch - pFont->wFirstChar];
    
    for (int y = 0; y < pFont->uHeight; y++) {
        for (int x = 0; x < width; x++) {
            // Calculate the absolute bit position in the source font data
            int bitPos = (startByteOffset * 8) + (y * pFont->wBytesPerLine * 8) + x;
            int byteIndex = bitPos / 8;
            int bitInByte = 7 - (bitPos % 8);

            // If the pixel is set in the font data...
            if ((pFont->pData[byteIndex] >> bitInByte) & 1) {
                // ...draw it on the screen.
                int screenX = pos.x + x;
                int screenY = pos.y + y;

                // Basic screen bounds check
                if (screenX >= 0 && screenX < buffer_width && screenY >= 0) {
                     *(framebuffer + (screenY * buffer_width) + screenX) = color;
                }
            }
        }
    }
}


// --- Public API Implementation ---

int peg_get_char_width(TCHAR ch, const PegFont* pFont) {
    if (!pFont || ch < pFont->wFirstChar || ch >= pFont->wLastChar) {
        return 0;
    }
    int charIndex = ch - pFont->wFirstChar;
    return pFont->pOffsets[charIndex + 1] - pFont->pOffsets[charIndex];
}

void peg_get_char_scanline(TCHAR ch, int row, uint8_t* buffer, const PegFont* pFont) {
    int width = peg_get_char_width(ch, pFont);
    if (width <= 0) return;

    int startBitOffset = pFont->pOffsets[ch - pFont->wFirstChar];
    int startBitOfScanline = startBitOffset + (row * pFont->wBytesPerLine * 8);

    for (int x = 0; x < width; x++) {
        int currentBit = startBitOfScanline + x;
        int byteIndex = currentBit / 8;
        int bitInByte = 7 - (currentBit % 8);

        if ((pFont->pData[byteIndex] >> bitInByte) & 1) {
            buffer[x] = 1; // Solid
        } else {
            buffer[x] = 0; // Transparent
        }
    }
}
#endif