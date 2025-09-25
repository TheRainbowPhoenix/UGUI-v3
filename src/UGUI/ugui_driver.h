#ifndef __UGUI_DRIVER_H
#define __UGUI_DRIVER_H

#include "ugui.h"

/* -------------------------------------------------------------------------------- */
/* -- µGUI DRIVER                                                                -- */
/* -------------------------------------------------------------------------------- */
typedef struct
{
  void* driver;
  UG_U8 state;
} UG_DRIVER;

#define DRIVER_REGISTERED                             (1<<0)
#define DRIVER_ENABLED                                (1<<1)

/* Supported drivers */
#define NUMBER_OF_DRIVERS                             3
#define DRIVER_DRAW_LINE                              0
#define DRIVER_FILL_FRAME                             1
#define DRIVER_FILL_AREA                              2

/* Driver functions */
void UG_DriverRegister( UG_U8 type, void* driver );
void UG_DriverEnable( UG_U8 type );
void UG_DriverDisable( UG_U8 type );


#endif