//------------------------------------------------------------------------------
// Project RosiePosi
// RosiePosi.cpp
// Kyle King
// 2023
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// C library includes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Platform includes
//------------------------------------------------------------------------------
#include <Particle.h>

//------------------------------------------------------------------------------
// Application includes
//------------------------------------------------------------------------------
#include "debug.h"
#include "gps_hal.h"
#include "accmtr_hal.h"
#include "activity_monitor.h"

//------------------------------------------------------------------------------
// Data types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Private variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Private function declarations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Configuration
//------------------------------------------------------------------------------
SYSTEM_MODE(SEMI_AUTOMATIC);
SYSTEM_THREAD(ENABLED);

// Turn on application logging only
SerialLogHandler logHandler(LOG_LEVEL_NONE, {
  // Logging level for all non-application messages
  {"app", LOG_LEVEL_ALL } // Logging Level for application messages
  });

///////////////////////////////////////////////////////////////////////////////
// Public Function Definitions
///////////////////////////////////////////////////////////////////////////////

void setup(void)
{
  TERM_dbg("Setting up...");
  delay(1000);

  accmtr_init();
}

void loop(void)
{
  accmtr_loop();
  activity_loop();
}

///////////////////////////////////////////////////////////////////////////////
// Private Function Definitions
///////////////////////////////////////////////////////////////////////////////