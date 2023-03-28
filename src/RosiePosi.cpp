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
#include "filesystem.h"

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
  init_filesystem();
}

void loop(void)
{
  accmtr_loop();
  activity_loop();

  static system_tick_t s_last_print = 0; //TODO:delete. TEST: every X ms, print latest sample && file info
  if (millis() - s_last_print > 3000)
  {
    s_last_print = millis();
    accmtr_sample_t sample;
    uint32_t file_size = UINT32_MAX;
    if (activate_file(FILE_ACCMTR_SAMPLE, FILE_ACCESS_TYPE_READ_ONLY)
    && (get_active_file_size(&file_size))
    && (file_size >= sizeof(sample))
    && (read_active_file(-1*sizeof(sample), &sample, sizeof(sample))))
    {
      TERM_crt("File size is now %u, latest sample is:", file_size);
      print_sample(sample);
    }
    TERM_crt("Test took %dms", millis() - s_last_print); // ~3ms
  }
}

///////////////////////////////////////////////////////////////////////////////
// Private Function Definitions
///////////////////////////////////////////////////////////////////////////////
