//------------------------------------------------------------------------------
// activity_monitor.cpp
// Kyle King
// 2023
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// C library includes
//------------------------------------------------------------------------------
#include <math.h>

//------------------------------------------------------------------------------
// Platform includes
//------------------------------------------------------------------------------
#include <Particle.h>

//------------------------------------------------------------------------------
// Application includes
//------------------------------------------------------------------------------
#include "activity_monitor.h"
#include "accmtr_hal.h"
#include "debug.h"

//------------------------------------------------------------------------------
// Data types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Private variables
//------------------------------------------------------------------------------
accmtr_sample_t m_latest_samples[50];

//------------------------------------------------------------------------------
// Private function declarations
//------------------------------------------------------------------------------
void print_sample(accmtr_sample_t sample);

///////////////////////////////////////////////////////////////////////////////
// Public Function Definitions
///////////////////////////////////////////////////////////////////////////////

void activity_loop(void)
{
  accmtr_sample_t sample;
  if (get_next_sample(&sample)) // should burst 25 samples every .5 seconds
  {
    // TODO: actually 
    print_sample(sample);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Private Function Definitions
///////////////////////////////////////////////////////////////////////////////

void print_sample(accmtr_sample_t _sample)
{
  typedef enum
  {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    FORWARD,
    BACKWARD,
    NONE
  } dir_t;
  
  char * dir_name[]
  {
    (char*)"UP",
    (char*)"DOWN",
    (char*)"LEFT",
    (char*)"RIGHT",
    (char*)"FORWARD",
    (char*)"BACKWARD",
    (char*)"NONE"
  };

  dir_t main_dir;
  double x = ((double)_sample.x) / ACCMTR_1G;
  double y = ((double)_sample.y) / ACCMTR_1G;
  double z = ((double)_sample.z) / ACCMTR_1G;

  if (abs(x) > abs(y) && abs(x) > abs(z)) // x is main
  {
    main_dir = x > 0 ? LEFT: RIGHT;
  }
  else if (abs(y) > abs(z)) // y is main
  {
    main_dir = y > 0 ? FORWARD: BACKWARD;
  }
  else // z is main
  {
    main_dir = z > 0 ? UP : DOWN;
  }
  
  char buf[100]; // arbitrary length
  sprintf(buf, "%s", dir_name[main_dir]); 
  double mag = sqrt(x*x+y*y+z*z);
  TERM_crt("%s mag=%lf, x=%lf y=%lf z=%lf", buf, mag, x, y, z);
}
