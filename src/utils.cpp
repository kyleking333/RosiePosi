//------------------------------------------------------------------------------
// utils.cpp
// Kyle King
// 2023
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// C library includes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Platform includes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Application includes
//------------------------------------------------------------------------------
#include "utils.h"

//------------------------------------------------------------------------------
// Data types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Private variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Private function declarations
//------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// Public Function Definitions
///////////////////////////////////////////////////////////////////////////////

uint32_t ms_since(uint32_t _input_ms)
{
    return millis() - _input_ms;
}

///////////////////////////////////////////////////////////////////////////////
// Private Function Definitions
///////////////////////////////////////////////////////////////////////////////
