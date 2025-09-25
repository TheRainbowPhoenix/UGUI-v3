/* -------------------------------------------------------------------------------- */
/* -- µGUI - Generic GUI module (C)Achim Döbler, 2015                            -- */
/* -------------------------------------------------------------------------------- */
// µGUI is a generic GUI module for embedded systems.
// This is a free software that is open for education, research and commercial
// developments under license policy of following terms.
//
//  Copyright (C) 2015, Achim Döbler, all rights reserved.
//  URL: http://www.embeddedlightning.com/
//
// * The µGUI module is a free software and there is NO WARRANTY.
// * No restriction on use. You can use, modify and redistribute it for
//   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
// * Redistributions of source code must retain the above copyright notice.
//
/* -------------------------------------------------------------------------------- */
/* -- MY SPECIAL THANKS GO TO                                                    -- */
/* -------------------------------------------------------------------------------- */
// Andrey Filimonov (-->https://github.com/Sermus)
// for giving valuable suggestions, reporting bugs and adding several new features.
// Andrey also put a lot of work in the implementaion of anti-aliased font support. 
//
// Mikhail Podkur (-->https://github.com/MikhailPodkur)
// for adding cyrillic 8x12 font, checkbox feature and RGB565 support.
//
// Gustavo Denardin
// for giving valuable suggestions regarding real-time os support.
//
// Samuel Kleiser
// for reporting bugs and giving examples how to improve µGUI.
/* -------------------------------------------------------------------------------- */
/* -- REVISION HISTORY                                                           -- */
/* -------------------------------------------------------------------------------- */
//  Dec 20, 2015  V0.31 Checkbox component with all funtions added.
//                      Cyrillic font 8x12 added.
//                      RGB565 color schema added.
//                      Windows components font could be getted from current GUI by default
//  Mar 18, 2015  V0.3  Driver support added.
//                      Window and object support added.
//                      Touch support added.
//                      Fixed some minor bugs.
//
//  Oct 20, 2014  V0.2  Function UG_DrawRoundFrame() added.
//                      Function UG_FillRoundFrame() added.
//                      Function UG_DrawArc() added.
//                      Fixed some minor bugs.
//
//  Oct 11, 2014  V0.1  First release.
/* -------------------------------------------------------------------------------- */
#include "ugui.h"

/* Static functions */
 void _UG_TextboxUpdate(UG_WINDOW* wnd, UG_OBJECT* obj);
 void _UG_ButtonUpdate(UG_WINDOW* wnd, UG_OBJECT* obj);
//  void _UG_CheckboxUpdate(UG_WINDOW* wnd, UG_OBJECT* obj);
 void _UG_ImageUpdate(UG_WINDOW* wnd, UG_OBJECT* obj);
 void _UG_PutChar( char chr, UG_S16 x, UG_S16 y, UG_COLOR fc, UG_COLOR bc, const UG_FONT* font);

 /* Pointer to the gui */
static UG_GUI* gui;

UG_S16 UG_Init( UG_GUI* g, void (*p)(UG_S16,UG_S16,UG_COLOR), UG_S16 x, UG_S16 y )
{
   UG_U8 i;

   g->pset = (void(*)(UG_S16,UG_S16,UG_COLOR))p;
   g->x_dim = x;
   g->y_dim = y;
   g->console.x_start = 4;
   g->console.y_start = 4;
   g->console.x_end = g->x_dim - g->console.x_start-1;
   g->console.y_end = g->y_dim - g->console.x_start-1;
   g->console.x_pos = g->console.x_end;
   g->console.y_pos = g->console.y_end;
   g->char_h_space = 1;
   g->char_v_space = 1;
   g->font.p = NULL;
   g->font.char_height = 0;
   g->font.char_width = 0;
   g->font.start_char = 0;
   g->font.end_char = 0;
   g->font.widths = NULL;
   g->desktop_color = C_DESKTOP_COLOR;
   g->fore_color = C_WHITE;
   g->back_color = C_BLACK;
   g->next_window = NULL;
   g->active_window = NULL;
   g->last_window = NULL;
   UG_ResetClipArea();

   /* Clear drivers */
   for(i=0;i<NUMBER_OF_DRIVERS;i++)
   {
      g->driver[i].driver = NULL;
      g->driver[i].state = 0;
   }

   gui = g;
   return 1;
}

UG_S16 UG_SelectGUI( UG_GUI* g )
{
   gui = g;
   return 1;
}

UG_GUI* UG_GetGUI( void )
{
   return gui;
}

void UG_FontSelect( const UG_FONT* font )
{
   gui->font = *font;
}

void UG_FillScreen( UG_COLOR c )
{
   UG_FillFrame(0,0,gui->x_dim-1,gui->y_dim-1,c);
}

void UG_FillFrame( UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c )
{
   UG_S16 n,m;

   if ( x2 < x1 )
     swap(x1,x2);
   if ( y2 < y1 )
     swap(y1,y2);

   /* Is hardware acceleration available? */
   if ( gui->driver[DRIVER_FILL_FRAME].state & DRIVER_ENABLED )
   {
      if( ((UG_RESULT(*)(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c))gui->driver[DRIVER_FILL_FRAME].driver)(x1,y1,x2,y2,c) == UG_RESULT_OK ) return;
   }

   for( m=y1; m<=y2; m++ )
   {
      for( n=x1; n<=x2; n++ )
      {
         gui->pset(n,m,c);
      }
   }
}

void UG_FillRoundFrame( UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_S16 r, UG_COLOR c )
{
   UG_S16  x,y,xd;

   if ( x2 < x1 )
   {
      x = x2;
      x2 = x1;
      x1 = x;
   }
   if ( y2 < y1 )
   {
      y = y2;
      y2 = y1;
      y1 = y;
   }

   if ( r<=0 ) return;

   xd = 3 - (r << 1);
   x = 0;
   y = r;

   UG_FillFrame(x1 + r, y1, x2 - r, y2, c);

   while ( x <= y )
   {
     if( y > 0 )
     {
        UG_DrawLine(x2 + x - r, y1 - y + r, x2+ x - r, y + y2 - r, c);
        UG_DrawLine(x1 - x + r, y1 - y + r, x1- x + r, y + y2 - r, c);
     }
     if( x > 0 )
     {
        UG_DrawLine(x1 - y + r, y1 - x + r, x1 - y + r, x + y2 - r, c);
        UG_DrawLine(x2 + y - r, y1 - x + r, x2 + y - r, x + y2 - r, c);
     }
     if ( xd < 0 )
     {
        xd += (x << 2) + 6;
     }
     else
     {
        xd += ((x - y) << 2) + 10;
        y--;
     }
     x++;
   }
}

void UG_DrawMesh( UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_U16 spacing, UG_COLOR c )
{
   UG_S16 p;

   if ( x2 < x1 )
     swap(x1,x2);
   if ( y2 < y1 )
     swap(y1,y2);

   for( p=y1; p<y2; p+=spacing )
   {
     UG_DrawLine(x1, p, x2, p, c);
   }
   UG_DrawLine(x1, y2, x2, y2, c);

   for( p=x1; p<x2; p+=spacing )
   {
     UG_DrawLine(p, y1, p, y2, c);
   }
   UG_DrawLine(x2, y1, x2, y2, c);
}

void UG_DrawFrame( UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c )
{
   UG_DrawLine(x1,y1,x2,y1,c);
   UG_DrawLine(x1,y2,x2,y2,c);
   UG_DrawLine(x1,y1,x1,y2,c);
   UG_DrawLine(x2,y1,x2,y2,c);
}

void UG_DrawRoundFrame( UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_S16 r, UG_COLOR c )
{
   if(r == 0)
   {
      UG_DrawFrame(x1, y1, x2, y2, c);
      return;
   }

   if ( x2 < x1 )
     swap(x1,x2);
   if ( y2 < y1 )
     swap(y1,y2);

   if ( r > x2 ) return;
   if ( r > y2 ) return;

   UG_DrawLine(x1+r, y1, x2-r, y1, c);
   UG_DrawLine(x1+r, y2, x2-r, y2, c);
   UG_DrawLine(x1, y1+r, x1, y2-r, c);
   UG_DrawLine(x2, y1+r, x2, y2-r, c);
   UG_DrawArc(x1+r, y1+r, r, 0x0C, c);
   UG_DrawArc(x2-r, y1+r, r, 0x03, c);
   UG_DrawArc(x1+r, y2-r, r, 0x30, c);
   UG_DrawArc(x2-r, y2-r, r, 0xC0, c);
}

void UG_DrawPixel( UG_S16 x0, UG_S16 y0, UG_COLOR c )
{
   gui->pset(x0,y0,c);
}

void UG_DrawCircle( UG_S16 x0, UG_S16 y0, UG_S16 r, UG_COLOR c )
{
   UG_S16 x,y,xd,yd,e;

   if ( x0<0 ) return;
   if ( y0<0 ) return;
   if ( r<=0 ) return;

   xd = 1 - (r << 1);
   yd = 0;
   e = 0;
   x = r;
   y = 0;

   while ( x >= y )
   {
      gui->pset(x0 - x, y0 + y, c);
      gui->pset(x0 - x, y0 - y, c);
      gui->pset(x0 + x, y0 + y, c);
      gui->pset(x0 + x, y0 - y, c);
      gui->pset(x0 - y, y0 + x, c);
      gui->pset(x0 - y, y0 - x, c);
      gui->pset(x0 + y, y0 + x, c);
      gui->pset(x0 + y, y0 - x, c);

      y++;
      e += yd;
      yd += 2;
      if ( ((e << 1) + xd) > 0 )
      {
         x--;
         e += xd;
         xd += 2;
      }
   }
}

void UG_FillCircle( UG_S16 x0, UG_S16 y0, UG_S16 r, UG_COLOR c )
{
   UG_S16  x,y,xd;

   if ( x0<0 ) return;
   if ( y0<0 ) return;
   if ( r<=0 ) return;

   xd = 3 - (r << 1);
   x = 0;
   y = r;

   while ( x <= y )
   {
     if( y > 0 )
     {
        UG_DrawLine(x0 - x, y0 - y,x0 - x, y0 + y, c);
        UG_DrawLine(x0 + x, y0 - y,x0 + x, y0 + y, c);
     }
     if( x > 0 )
     {
        UG_DrawLine(x0 - y, y0 - x,x0 - y, y0 + x, c);
        UG_DrawLine(x0 + y, y0 - x,x0 + y, y0 + x, c);
     }
     if ( xd < 0 )
     {
        xd += (x << 2) + 6;
     }
     else
     {
        xd += ((x - y) << 2) + 10;
        y--;
     }
     x++;
   }
   UG_DrawCircle(x0, y0, r,c);
}

void UG_DrawArc( UG_S16 x0, UG_S16 y0, UG_S16 r, UG_U8 s, UG_COLOR c )
{
   UG_S16 x,y,xd,yd,e;

   if ( x0<0 ) return;
   if ( y0<0 ) return;
   if ( r<=0 ) return;

   xd = 1 - (r << 1);
   yd = 0;
   e = 0;
   x = r;
   y = 0;

   while ( x >= y )
   {
      // Q1
      if ( s & 0x01 ) gui->pset(x0 + x, y0 - y, c);
      if ( s & 0x02 ) gui->pset(x0 + y, y0 - x, c);

      // Q2
      if ( s & 0x04 ) gui->pset(x0 - y, y0 - x, c);
      if ( s & 0x08 ) gui->pset(x0 - x, y0 - y, c);

      // Q3
      if ( s & 0x10 ) gui->pset(x0 - x, y0 + y, c);
      if ( s & 0x20 ) gui->pset(x0 - y, y0 + x, c);

      // Q4
      if ( s & 0x40 ) gui->pset(x0 + y, y0 + x, c);
      if ( s & 0x80 ) gui->pset(x0 + x, y0 + y, c);

      y++;
      e += yd;
      yd += 2;
      if ( ((e << 1) + xd) > 0 )
      {
         x--;
         e += xd;
         xd += 2;
      }
   }
}

void UG_DrawLine( UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c )
{
   UG_S16 n, dx, dy, sgndx, sgndy, dxabs, dyabs, x, y, drawx, drawy;

   /* Is hardware acceleration available? */
   if ( gui->driver[DRIVER_DRAW_LINE].state & DRIVER_ENABLED )
   {
      if( ((UG_RESULT(*)(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c))gui->driver[DRIVER_DRAW_LINE].driver)(x1,y1,x2,y2,c) == UG_RESULT_OK ) return;
   }

   dx = x2 - x1;
   dy = y2 - y1;
   dxabs = (dx>0)?dx:-dx;
   dyabs = (dy>0)?dy:-dy;
   sgndx = (dx>0)?1:-1;
   sgndy = (dy>0)?1:-1;
   x = dyabs >> 1;
   y = dxabs >> 1;
   drawx = x1;
   drawy = y1;

   gui->pset(drawx, drawy,c);

   if( dxabs >= dyabs )
   {
      for( n=0; n<dxabs; n++ )
      {
         y += dyabs;
         if( y >= dxabs )
         {
            y -= dxabs;
            drawy += sgndy;
         }
         drawx += sgndx;
         gui->pset(drawx, drawy,c);
      }
   }
   else
   {
      for( n=0; n<dyabs; n++ )
      {
         x += dxabs;
         if( x >= dyabs )
         {
            x -= dyabs;
            drawx += sgndx;
         }
         drawy += sgndy;
         gui->pset(drawx, drawy,c);
      }
   }  
}

/* Draw a triangle */
void UG_DrawTriangle( UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_S16 x3, UG_S16 y3, UG_COLOR c ){
  UG_DrawLine(x1, y1, x2, y2, c);
  UG_DrawLine(x2, y2, x3, y3, c);
  UG_DrawLine(x3, y3, x1, y1, c);
}

/* Fill a triangle */
void UG_FillTriangle( UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_S16 x3, UG_S16 y3, UG_COLOR c ){

  UG_S16 a, b, y, last;

  /* Sort coordinates by Y order (y3 >= y2 >= y1) */
  if (y1 > y2) {
    swap(y1, y2); swap(x1, x2);
  }
  if (y2 > y3) {
    swap(y3, y2); swap(x3, x2);
  }
  if (y1 > y2) {
    swap(y1, y2); swap(x1, x2);
  }

  /* Handle awkward all-on-same-line case as its own thing */
  if (y1 == y3) {
    a = b = x1;
    if (x2 < a) {
      a = x2;
    } else if (x2 > b) {
      b = x2;
    }
    if (x3 < a) {
      a = x3;
    } else if (x3 > b) {
      b = x3;
    }
    UG_DrawLine(a, y1, b + 1, y1, c);
    return;
  }

  UG_S16
  dx01 = x2 - x1,
  dy01 = y2 - y1,
  dx02 = x3 - x1,
  dy02 = y3 - y1,
  dx12 = x3 - x2,
  dy12 = y3 - y2,
  sa   = 0,
  sb   = 0;

  /* For upper part of triangle, find scanline crossings for segments
   * 0-1 and 0-2.  If y2=y3 (flat-bottomed triangle), the scanline y2
   * is included here (and second loop will be skipped, avoiding a /0
   * error there), otherwise scanline y2 is skipped here and handled
   * in the second loop...which also avoids a /0 error here if y1=y2
   * (flat-topped triangle).
   */
  if (y2 == y3) {
    last = y2;   /* Include y2 scanline */
  } else {
    last = y2 - 1; /* Skip it */
  }

  for (y = y1; y <= last; y++) {
    a   = x1 + sa / dy01;
    b   = x1 + sb / dy02;
    sa += dx01;
    sb += dx02;
    /* longhand:
       a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
       b = x1 + (x3 - x1) * (y - y1) / (y3 - y1);
       */
    if (a > b) {
      swap(a, b);
    }
    UG_DrawLine(a, y, b + 1, y, c);
  }

  /* For lower part of triangle, find scanline crossings for segments
   * 0-2 and 1-2.  This loop is skipped if y2=y3.
   */
  sa = dx12 * (y - y2);
  sb = dx02 * (y - y1);
  for (; y <= y3; y++) {
    a   = x2 + sa / dy12;
    b   = x1 + sb / dy02;
    sa += dx12;
    sb += dx02;
    /* longhand:
       a = x2 + (x3 - x2) * (y - y2) / (y3 - y2);
       b = x1 + (x3 - x1) * (y - y1) / (y3 - y1);
       */
    if (a > b) {
      swap(a, b);
    }
    UG_DrawLine(a, y, b + 1, y, c);
  }
}


void UG_PutString( UG_S16 x, UG_S16 y, char* str )
{
   UG_S16 xp,yp;
   UG_U8 cw;
   char chr;

   xp=x;
   yp=y;

   while ( *str != 0 )
   {
      chr = *str++;
	  if (chr < gui->font.start_char || chr > gui->font.end_char) continue;
      if ( chr == '\n' )
      {
         xp = gui->x_dim;
         continue;
      }
      if (gui->font.font_type == FONT_TYPE_PEG) {
         cw = peg_get_char_width(chr, (void*)gui->font.p);
      } else {
	      cw = gui->font.widths ? gui->font.widths[chr - gui->font.start_char] : gui->font.char_width;
      }

      if ( xp + cw > gui->x_dim - 1 )
      {
         xp = x;
         yp += gui->font.char_height+gui->char_v_space;
      }

      UG_PutChar(chr, xp, yp, gui->fore_color, gui->back_color);

      xp += cw + gui->char_h_space;
   }
}

void UG_PutChar( char chr, UG_S16 x, UG_S16 y, UG_COLOR fc, UG_COLOR bc )
{
	_UG_PutChar(chr,x,y,fc,bc,&gui->font);
}

void UG_ConsolePutString( char* str )
{
   char chr;
   UG_U8 cw;

   while ( *str != 0 )
   {
      chr = *str;
      if ( chr == '\n' )
      {
         gui->console.x_pos = gui->x_dim;
         str++;
         continue;
      }
      
      cw = gui->font.widths ? gui->font.widths[chr - gui->font.start_char] : gui->font.char_width;
      gui->console.x_pos += cw+gui->char_h_space;

      if ( gui->console.x_pos+cw > gui->console.x_end )
      {
         gui->console.x_pos = gui->console.x_start;
         gui->console.y_pos += gui->font.char_height+gui->char_v_space;
      }
      if ( gui->console.y_pos+gui->font.char_height > gui->console.y_end )
      {
         gui->console.x_pos = gui->console.x_start;
         gui->console.y_pos = gui->console.y_start;
         UG_FillFrame(gui->console.x_start,gui->console.y_start,gui->console.x_end,gui->console.y_end,gui->console.back_color);
      }

      UG_PutChar(chr, gui->console.x_pos, gui->console.y_pos, gui->console.fore_color, gui->console.back_color);
      str++;
   }
}

void UG_ConsoleSetArea( UG_S16 xs, UG_S16 ys, UG_S16 xe, UG_S16 ye )
{
   gui->console.x_start = xs;
   gui->console.y_start = ys;
   gui->console.x_end = xe;
   gui->console.y_end = ye;
}

void UG_ConsoleSetForecolor( UG_COLOR c )
{
   gui->console.fore_color = c;
}

void UG_ConsoleSetBackcolor( UG_COLOR c )
{
   gui->console.back_color = c;
}

void UG_SetForecolor( UG_COLOR c )
{
   gui->fore_color = c;
}

void UG_SetBackcolor( UG_COLOR c )
{
   gui->back_color = c;
}

UG_S16 UG_GetXDim( void )
{
   return gui->x_dim;
}

UG_S16 UG_GetYDim( void )
{
   return gui->y_dim;
}

void UG_FontSetHSpace( UG_U16 s )
{
   gui->char_h_space = s;
}

void UG_FontSetVSpace( UG_U16 s )
{
   gui->char_v_space = s;
}

void UG_GetClipArea(UG_AREA* a)
{
   a->xs = gui->clip.xs;
   a->ys = gui->clip.ys;
   a->xe = gui->clip.xe;
   a->ye = gui->clip.ye;
}

void UG_SetClipArea(UG_AREA* a)
{
   gui->clip.xs = a->xs;
   gui->clip.ys = a->ys;
   gui->clip.xe = a->xe;
   gui->clip.ye = a->ye;
}

// Add a public function to reset the clipping area to the full screen.
void UG_ResetClipArea(void)
{
   gui->clip.xs = 0;
   gui->clip.ys = 0;
   gui->clip.xe = gui->x_dim - 1;
   gui->clip.ye = gui->y_dim - 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/* -------------------------------------------------------------------------------- */
/* -- INTERNAL FUNCTIONS                                                         -- */
/* -------------------------------------------------------------------------------- */
void _UG_PutChar( char chr, UG_S16 x, UG_S16 y, UG_COLOR fc, UG_COLOR bc, const UG_FONT* font)
{
   UG_U16 i,j,k,xo,yo,c,bn,actual_char_width;
   UG_U8 b,bt;
   UG_U32 index;
   UG_COLOR color;
   void(*push_pixel)(UG_COLOR);

   bt = (UG_U8)chr;

   switch ( bt )
   {
      case 0xF6: bt = 0x94; break; // �
      case 0xD6: bt = 0x99; break; // �
      case 0xFC: bt = 0x81; break; // �
      case 0xDC: bt = 0x9A; break; // �
      case 0xE4: bt = 0x84; break; // �
      case 0xC4: bt = 0x8E; break; // �
      case 0xB5: bt = 0xE6; break; // �
      case 0xB0: bt = 0xF8; break; // �
   }

   if (font->font_type == FONT_TYPE_PEG) {
      actual_char_width = peg_get_char_width(bt, (void*)font->p);
   } else {
      if (bt < font->start_char || bt > font->end_char) return;
      yo = y;
      bn = font->char_width;
      if ( !bn ) return;
      bn >>= 3;
      if ( font->char_width % 8 ) bn++;
      actual_char_width = (font->widths ? font->widths[bt - font->start_char] : font->char_width);
   }

   if (actual_char_width == 0) return;

   /* Is hardware acceleration available? */
   if ( gui->driver[DRIVER_FILL_AREA].state & DRIVER_ENABLED )
   {
	   //(void(*)(UG_COLOR))
      push_pixel = ((void*(*)(UG_S16, UG_S16, UG_S16, UG_S16))gui->driver[DRIVER_FILL_AREA].driver)(x,y,x+actual_char_width-1,y+font->char_height-1);
	   
      if (font->font_type == FONT_TYPE_1BPP)
      {
         index = (bt - font->start_char)* font->char_height * bn;
         for( j=0;j<font->char_height;j++ )
         {
            c=actual_char_width;
            for( i=0;i<bn;i++ )
            {
            b = font->p[index++];
            for( k=0;(k<8) && c;k++ )
            {
               if( b & 0x01 )
               {
                  push_pixel(fc);
               }
               else
               {
                  push_pixel(bc);
               }
               b >>= 1;
               c--;
            }
            }
         }
      }
      else if (font->font_type == FONT_TYPE_8BPP)
      {
         index = (bt - font->start_char)* font->char_height * font->char_width;
         for( j=0;j<font->char_height;j++ )
         {
            for( i=0;i<actual_char_width;i++ )
            {
               b = font->p[index++];
               color = (((fc & 0xFF) * b + (bc & 0xFF) * (256 - b)) >> 8) & 0xFF |//Blue component
                     (((fc & 0xFF00) * b + (bc & 0xFF00) * (256 - b)) >> 8)  & 0xFF00|//Green component
                     (((fc & 0xFF0000) * b + (bc & 0xFF0000) * (256 - b)) >> 8) & 0xFF0000; //Red component
               push_pixel(color);
            }
            index += font->char_width - actual_char_width;
         }
      }
      else if (font->font_type == FONT_TYPE_PEG)
      {
         const void* pPegFont = (void*)font->p;
         uint8_t scanline_buffer[actual_char_width]; // VLA to hold one row of pixel data
         for (j = 0; j < font->char_height; j++) {
            // Ask our C++ engine to fill the buffer with this row's data
            peg_get_char_scanline(bt, j, scanline_buffer, pPegFont);
            for (i = 0; i < actual_char_width; i++) {
               if (scanline_buffer[i]) {
                     push_pixel(fc);
               } else {
                     push_pixel(bc);
               }
            }
         }
      }
   }
   else
   {
	   /*Not accelerated output*/
      if (font->font_type == FONT_TYPE_1BPP)
      {
         index = (bt - font->start_char)* font->char_height * bn;
         for( j=0;j<font->char_height;j++ )
         {
            xo = x;
            c=actual_char_width;
            for( i=0;i<bn;i++ )
            {
               b = font->p[index++];
               for( k=0;(k<8) && c;k++ )
               {
               if( b & 0x01 )
               {
                  gui->pset(xo,yo,fc);
               }
               else
               {
                  gui->pset(xo,yo,bc);
               }
               b >>= 1;
               xo++;
               c--;
               }
            }
            yo++;
         }
      }
      else if (font->font_type == FONT_TYPE_8BPP)
      {
         index = (bt - font->start_char)* font->char_height * font->char_width;
         for( j=0;j<font->char_height;j++ )
         {
            xo = x;
            for( i=0;i<actual_char_width;i++ )
            {
               b = font->p[index++];
               color = (((fc & 0xFF) * b + (bc & 0xFF) * (256 - b)) >> 8) & 0xFF |//Blue component
                        (((fc & 0xFF00) * b + (bc & 0xFF00) * (256 - b)) >> 8)  & 0xFF00|//Green component
                        (((fc & 0xFF0000) * b + (bc & 0xFF0000) * (256 - b)) >> 8) & 0xFF0000; //Red component
               gui->pset(xo,yo,color);
               xo++;
            }
            index += font->char_width - actual_char_width;
            yo++;
         }
      }
      else if (font->font_type == FONT_TYPE_PEG)
      {
         const void* pPegFont = (void*)font->p;
         uint8_t scanline_buffer[actual_char_width]; // VLA for the pixel row
         yo = y;
         for (j = 0; j < font->char_height; j++) {
            peg_get_char_scanline(bt, j, scanline_buffer, pPegFont);
            xo = x;
            for (i = 0; i < actual_char_width; i++) {
               if (
                  1 
                  // TODO: Fix the clip checks 
                  // && xo >= gui->clip.xs && xo <= gui->clip.xe
                  // && (yo + j) >= gui->clip.ys && (yo + j) <= gui->clip.ye
               )
               {
                  if (scanline_buffer[i]) {
                     gui->pset(xo, yo, fc);
                  } else {
                     gui->pset(xo, yo, bc);
                  }
                  xo++;
               }
            }
            yo++;
         }
      }
   }
}

void _UG_PutText(UG_TEXT* txt)
{
   UG_U16 sl,rc,wl;
   UG_S16 xp,yp;
   UG_S16 xs=txt->a.xs;
   UG_S16 ys=txt->a.ys;
   UG_S16 xe=txt->a.xe;
   UG_S16 ye=txt->a.ye;
   UG_U8  align=txt->align;
   UG_S16 char_width=txt->font->char_width;
   UG_S16 char_height=txt->font->char_height;
   UG_S16 char_h_space=txt->h_space;
   UG_S16 char_v_space=txt->v_space;

   char chr;

   char* str = txt->str;
   char* c = str;

   if ( txt->font->p == NULL ) return;
   if ( str == NULL ) return;
   if ( (ye - ys) < txt->font->char_height ) return;

   UG_AREA old_clip;
   UG_GetClipArea(&old_clip);
   UG_SetClipArea(&txt->a);

   rc=1;
   c=str;
   while ( *c != 0 )
   {
      if ( *c == '\n' ) rc++;
      c++;
   }

   yp = 0;
   if ( align & (ALIGN_V_CENTER | ALIGN_V_BOTTOM) )
   {
      yp = ye - ys + 1;
      yp -= char_height*rc;
      yp -= char_v_space*(rc-1);
      if ( yp < 0 ) {
         UG_SetClipArea(&old_clip);
         return;
         // yp = 0
      }
   }
   if ( align & ALIGN_V_CENTER ) yp >>= 1;
   yp += ys;

   while( 1 )
   {
      sl=0;
      c=str;
      wl = 0;
      while( (*c != 0) && (*c != '\n') )
      {
         if (*c < txt->font->start_char || *c > txt->font->end_char) { c++; continue; }
         sl++;
         if (txt->font->font_type == FONT_TYPE_PEG) {
            // Use our C++ helper to get the true variable width
            wl += peg_get_char_width(*c, (void*)txt->font->p); // + char_h_space
         } else {
            // Original UGUI logic
            wl += (txt->font->widths ? txt->font->widths[*c - txt->font->start_char] : char_width) + char_h_space;
         }
         c++;
      }
      wl -= char_h_space;

      xp = xe - xs + 1;
      xp -= wl;
      // if ( xp < 0 ) return;

      if ( align & ALIGN_H_LEFT ) xp = 0;
      else if ( align & ALIGN_H_CENTER ) xp >>= 1;
      xp += xs;

      while( (*str != '\n') )
      {
         chr = *str++;
         if ( chr == 0 ) 
         {
            UG_SetClipArea(&old_clip);
            return;
         }
         if (xp >= txt->a.xs && xp <= txt->a.xe) {
            _UG_PutChar(chr,xp,yp,txt->fc,txt->bc,txt->font);
         }

         if (txt->font->font_type == FONT_TYPE_PEG) {
            xp += peg_get_char_width(chr, (void*)txt->font->p); //  + char_h_space
         } else {
            xp += (txt->font->widths ? txt->font->widths[chr - txt->font->start_char] : char_width) + char_h_space;
         }
      }
      str++;
      yp += char_height + char_v_space;
   }
   UG_SetClipArea(&old_clip);
}

UG_OBJECT* _UG_GetFreeObject( UG_WINDOW* wnd )
{
   UG_U8 i;
   UG_OBJECT* obj=(UG_OBJECT*)wnd->objlst;

   for(i=0;i<wnd->objcnt;i++)
   {
      obj = (UG_OBJECT*)(&wnd->objlst[i]);
      if ( (obj->state & OBJ_STATE_FREE) && (obj->state & OBJ_STATE_VALID) )
      {
         /* Free object found! */
         return obj;
      }
   }
   return NULL;
}

UG_OBJECT* _UG_SearchObject( UG_WINDOW* wnd, UG_U8 type, UG_U8 id )
{
   UG_U8 i;
   UG_OBJECT* obj=(UG_OBJECT*)wnd->objlst;

   for(i=0;i<wnd->objcnt;i++)
   {
      obj = (UG_OBJECT*)(&wnd->objlst[i]);
      if ( !(obj->state & OBJ_STATE_FREE) && (obj->state & OBJ_STATE_VALID) )
      {
         if ( (obj->type == type) && (obj->id == id) )
         {
            /* Requested object found! */
            return obj;
         }
      }
   }
   return NULL;
}

UG_RESULT _UG_DeleteObject( UG_WINDOW* wnd, UG_U8 type, UG_U8 id )
{
   UG_OBJECT* obj=NULL;

   obj = _UG_SearchObject( wnd, type, id );

   /* Object found? */
   if ( obj != NULL )
   {
      /* We dont't want to delete a visible or busy object! */
      if ( (obj->state & OBJ_STATE_VISIBLE) || (obj->state & OBJ_STATE_UPDATE) ) return UG_RESULT_FAIL;
      obj->state = OBJ_STATE_INIT;
      obj->data = NULL;
      obj->event = 0;
      obj->id = 0;
      obj->touch_state = 0;
      obj->type = 0;
      obj->update = NULL;
      return UG_RESULT_OK;
   }
   return UG_RESULT_FAIL;
}

void _UG_ProcessTouchData( UG_WINDOW* wnd )
{
   UG_S16 xp,yp;
   UG_U16 i,objcnt;
   UG_OBJECT* obj;
   UG_U8 objstate;
   UG_U8 objtouch;
   UG_U8 tchstate;

   xp = gui->touch.xp;
   yp = gui->touch.yp;
   tchstate = gui->touch.state;

   objcnt = wnd->objcnt;
   for(i=0; i<objcnt; i++)
   {
      obj = (UG_OBJECT*)&wnd->objlst[i];
      objstate = obj->state;
      objtouch = obj->touch_state;
      if ( !(objstate & OBJ_STATE_FREE) && (objstate & OBJ_STATE_VALID) && (objstate & OBJ_STATE_VISIBLE) && !(objstate & OBJ_STATE_REDRAW))
      {
         /* Process touch data */
         if ( (tchstate) && xp != -1 )
         {
            if ( !(objtouch & OBJ_TOUCH_STATE_IS_PRESSED) )
            {
               objtouch |= OBJ_TOUCH_STATE_PRESSED_OUTSIDE_OBJECT | OBJ_TOUCH_STATE_CHANGED;
               objtouch &= ~(OBJ_TOUCH_STATE_RELEASED_ON_OBJECT | OBJ_TOUCH_STATE_RELEASED_OUTSIDE_OBJECT | OBJ_TOUCH_STATE_CLICK_ON_OBJECT);
            }
            objtouch &= ~OBJ_TOUCH_STATE_IS_PRESSED_ON_OBJECT;
            if ( xp >= obj->a_abs.xs )
            {
               if ( xp <= obj->a_abs.xe )
               {
                  if ( yp >= obj->a_abs.ys )
                  {
                     if ( yp <= obj->a_abs.ye )
                     {
                        objtouch |= OBJ_TOUCH_STATE_IS_PRESSED_ON_OBJECT;
                        if ( !(objtouch & OBJ_TOUCH_STATE_IS_PRESSED) )
                        {
                           objtouch &= ~OBJ_TOUCH_STATE_PRESSED_OUTSIDE_OBJECT;
                           objtouch |= OBJ_TOUCH_STATE_PRESSED_ON_OBJECT;
                        }
                     }
                  }
               }
            }
            objtouch |= OBJ_TOUCH_STATE_IS_PRESSED;
         }
         else if ( objtouch & OBJ_TOUCH_STATE_IS_PRESSED )
         {
            if ( objtouch & OBJ_TOUCH_STATE_IS_PRESSED_ON_OBJECT )
            {
               if ( objtouch & OBJ_TOUCH_STATE_PRESSED_ON_OBJECT ) objtouch |= OBJ_TOUCH_STATE_CLICK_ON_OBJECT;
               objtouch |= OBJ_TOUCH_STATE_RELEASED_ON_OBJECT;
            }
            else
            {
               objtouch |= OBJ_TOUCH_STATE_RELEASED_OUTSIDE_OBJECT;
            }
            if ( objtouch & OBJ_TOUCH_STATE_IS_PRESSED )
            {
               objtouch |= OBJ_TOUCH_STATE_CHANGED;
            }
            objtouch &= ~(OBJ_TOUCH_STATE_PRESSED_OUTSIDE_OBJECT | OBJ_TOUCH_STATE_PRESSED_ON_OBJECT | OBJ_TOUCH_STATE_IS_PRESSED);
         }
      }
      obj->touch_state = objtouch;
   }
}

static void _UG_UpdateObjects( UG_WINDOW* wnd )
{
   UG_U16 i,objcnt;
   UG_OBJECT* obj;
   UG_U8 objstate;
   UG_U8 objtouch;

   /* Check each object, if it needs to be updated? */
   objcnt = wnd->objcnt;
   for(i=0; i<objcnt; i++)
   {
      obj = (UG_OBJECT*)&wnd->objlst[i];
      objstate = obj->state;
      objtouch = obj->touch_state;
      if ( !(objstate & OBJ_STATE_FREE) && (objstate & OBJ_STATE_VALID) )
      {
         if ( objstate & OBJ_STATE_UPDATE )
         {
            obj->update(wnd,obj);
         }
         if ( (objstate & OBJ_STATE_VISIBLE) && (objstate & OBJ_STATE_TOUCH_ENABLE) )
         {
            if ( (objtouch & (OBJ_TOUCH_STATE_CHANGED | OBJ_TOUCH_STATE_IS_PRESSED)) )
            {
               obj->update(wnd,obj);
            }
         }
      }
   }
}

/*
 * Using only bitshifts would save some instruction cycles
 * but this way the full color space remains (pure 0xFFFFFF)
 */
UG_U32 _UG_ConvertRGB565ToRGB888(UG_U16 c)
{
   UG_U32 r,g,b;

   r = (c&0xF800)<<8;
   r += (r+7)>>5;
   r &= 0xFF0000;

   g = (c&0x7E0)<<5;
   g += (g+3)>>6;
   g &= 0x00FF00;

   b = (c&0x1F)<<3;
   b += (b+7)>>5;
   b &= 0x0000FF;

   return (r | g | b);
}

static void _UG_HandleEvents( UG_WINDOW* wnd )
{
   UG_U16 i,objcnt;
   UG_OBJECT* obj;
   UG_U8 objstate;
   static UG_MESSAGE msg;
   msg.src = NULL;

   /* Handle window-related events */
   //ToDo


   /* Handle object-related events */
   msg.type = MSG_TYPE_OBJECT;
   objcnt = wnd->objcnt;
   for(i=0; i<objcnt; i++)
   {
      obj = (UG_OBJECT*)&wnd->objlst[i];
      objstate = obj->state;
      if ( !(objstate & OBJ_STATE_FREE) && (objstate & OBJ_STATE_VALID) )
      {
         if ( obj->event != OBJ_EVENT_NONE )
         {
            msg.src = &obj;
            msg.id = obj->type;
            msg.sub_id = obj->id;
            msg.event = obj->event;

            wnd->cb( &msg );

            obj->event = OBJ_EVENT_NONE;
         }
      }
   }
}

void _UG_DrawObjectFrame( UG_S16 xs, UG_S16 ys, UG_S16 xe, UG_S16 ye, UG_COLOR* p )
{
   // Frame 0
   UG_DrawLine(xs, ys  , xe-1, ys  , *p++);
   UG_DrawLine(xs, ys+1, xs  , ye-1, *p++);
   UG_DrawLine(xs, ye  , xe  , ye  , *p++);
   UG_DrawLine(xe, ys  , xe  , ye-1, *p++);
   // Frame 1
   UG_DrawLine(xs+1, ys+1, xe-2, ys+1, *p++);
   UG_DrawLine(xs+1, ys+2, xs+1, ye-2, *p++);
   UG_DrawLine(xs+1, ye-1, xe-1, ye-1, *p++);
   UG_DrawLine(xe-1, ys+1, xe-1, ye-2, *p++);
   // Frame 2
   UG_DrawLine(xs+2, ys+2, xe-3, ys+2, *p++);
   UG_DrawLine(xs+2, ys+3, xs+2, ye-3, *p++);
   UG_DrawLine(xs+2, ye-2, xe-2, ye-2, *p++);
   UG_DrawLine(xe-2, ys+2, xe-2, ye-3, *p);
}

#ifdef UGUI_USE_PRERENDER_EVENT
void _UG_SendObjectPrerenderEvent(UG_WINDOW *wnd,UG_OBJECT *obj)
{
	UG_MESSAGE msg;
	msg.event = OBJ_EVENT_PRERENDER;
	msg.type = MSG_TYPE_OBJECT;
	msg.id = obj->type;
	msg.sub_id = obj->id;
	msg.src = obj;

	wnd->cb(&msg);
}
#endif

#ifdef UGUI_USE_POSTRENDER_EVENT
void _UG_SendObjectPostrenderEvent(UG_WINDOW *wnd,UG_OBJECT *obj)
{
	UG_MESSAGE msg;
	msg.event = OBJ_EVENT_POSTRENDER;
	msg.type = MSG_TYPE_OBJECT;
	msg.id = obj->type;
	msg.sub_id = obj->id;
	msg.src = obj;

	wnd->cb(&msg);
}
#endif

/* -------------------------------------------------------------------------------- */
/* -- DRIVER FUNCTIONS                                                           -- */
/* -------------------------------------------------------------------------------- */
void UG_DriverRegister( UG_U8 type, void* driver )
{
   if ( type >= NUMBER_OF_DRIVERS ) return;

   gui->driver[type].driver = driver;
   gui->driver[type].state = DRIVER_REGISTERED | DRIVER_ENABLED;
}

void UG_DriverEnable( UG_U8 type )
{
   if ( type >= NUMBER_OF_DRIVERS ) return;
   if ( gui->driver[type].state & DRIVER_REGISTERED )
   {
      gui->driver[type].state |= DRIVER_ENABLED;
   }
}

void UG_DriverDisable( UG_U8 type )
{
   if ( type >= NUMBER_OF_DRIVERS ) return;
   if ( gui->driver[type].state & DRIVER_REGISTERED )
   {
      gui->driver[type].state &= ~DRIVER_ENABLED;
   }
}

/* -------------------------------------------------------------------------------- */
/* -- MISCELLANEOUS FUNCTIONS                                                    -- */
/* -------------------------------------------------------------------------------- */
void UG_Update( void )
{
   UG_WINDOW* wnd;

   /* Is somebody waiting for this update? */
   if ( gui->state & UG_STATUS_WAIT_FOR_UPDATE ) gui->state &= ~UG_STATUS_WAIT_FOR_UPDATE;

   /* Keep track of the windows */
   if ( gui->next_window != gui->active_window )
   {
      if ( gui->next_window != NULL )
      {
         gui->last_window = gui->active_window;
         gui->active_window = gui->next_window;

         /* Do we need to draw an inactive title? */
         if ((gui->last_window != NULL) && (gui->last_window->style & WND_STYLE_SHOW_TITLE) && (gui->last_window->state & WND_STATE_VISIBLE) )
         {
            /* Do both windows differ in size */
            if ( (gui->last_window->xs != gui->active_window->xs) || (gui->last_window->xe != gui->active_window->xe) || (gui->last_window->ys != gui->active_window->ys) || (gui->last_window->ye != gui->active_window->ye) )
            {
               /* Redraw title of the last window */
               _UG_WindowDrawTitle( gui->last_window );
            }
         }
         gui->active_window->state &= ~WND_STATE_REDRAW_TITLE;
         gui->active_window->state |= WND_STATE_UPDATE | WND_STATE_VISIBLE;
      }
   }

   /* Is there an active window */
   if ( gui->active_window != NULL )
   {
      wnd = gui->active_window;

      /* Does the window need to be updated? */
      if ( wnd->state & WND_STATE_UPDATE )
      {
         /* Do it! */
         _UG_WindowUpdate( wnd );
      }

      /* Is the window visible? */
      if ( wnd->state & WND_STATE_VISIBLE )
      {
         _UG_ProcessTouchData( wnd );
         _UG_UpdateObjects( wnd );
         _UG_HandleEvents( wnd );
      }
   }
}

void UG_WaitForUpdate( void )
{
   gui->state |= UG_STATUS_WAIT_FOR_UPDATE;
   #ifdef USE_MULTITASKING    
   while ( (volatile UG_U8)gui->state & UG_STATUS_WAIT_FOR_UPDATE ){};
   #endif    
   #ifndef USE_MULTITASKING    
   while ( (UG_U8)gui->state & UG_STATUS_WAIT_FOR_UPDATE ){};
   #endif    
}

void UG_DrawBMP( UG_S16 xp, UG_S16 yp, UG_BMP* bmp )
{
   UG_S16 x,y,xs;
   UG_U8 r,g,b;
   UG_U16* p;
   UG_U16 tmp;
   UG_COLOR c;

   if ( bmp->p == NULL ) return;

   /* Only support 16 BPP so far */
   if ( bmp->bpp == BMP_BPP_16 )
   {
      p = (UG_U16*)bmp->p;
   }
   else
   {
      return;
   }

   xs = xp;
   for(y=0;y<bmp->height;y++)
   {
      xp = xs;
      for(x=0;x<bmp->width;x++)
      {
         tmp = *p++;
         /* Convert RGB565 to RGB888 */
         r = (tmp>>11)&0x1F;
         r<<=3;
         g = (tmp>>5)&0x3F;
         g<<=2;
         b = (tmp)&0x1F;
         b<<=3;
         c = ((UG_COLOR)r<<16) | ((UG_COLOR)g<<8) | (UG_COLOR)b;
         UG_DrawPixel( xp++ , yp , c );
      }
      yp++;
   }
}

void UG_TouchUpdate( UG_S16 xp, UG_S16 yp, UG_U8 state )
{
   gui->touch.xp = xp;
   gui->touch.yp = yp;
   gui->touch.state = state;
}




