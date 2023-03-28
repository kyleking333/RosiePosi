//------------------------------------------------------------------------------
// debug.cpp
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

//------------------------------------------------------------------------------
// Data types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Private variables
//------------------------------------------------------------------------------
//For now keep static variable for log level
static dbg_verbosity_lvl_t m_verbose_level = VRB_TRACE;
static bool m_uart_busy = false;

//------------------------------------------------------------------------------
// Private function declarations
//------------------------------------------------------------------------------
bool get_dbg_verbose_level(dbg_verbosity_lvl_t* verbose_lvl);

///////////////////////////////////////////////////////////////////////////////
// Public Function Definitions
///////////////////////////////////////////////////////////////////////////////

void TERM_init(bool _override_to_debug)
{
    dbg_verbosity_lvl_t dbg_verbose_val = VRB_OFF;
    bool status;
    if (_override_to_debug)
    {
        m_verbose_level = VRB_DEBUG;
    }
    else
    {
        status = get_dbg_verbose_level(&dbg_verbose_val);
        if(status)
        {
            m_verbose_level = (dbg_verbosity_lvl_t)dbg_verbose_val;
        }
    }
    
    TERM_trc("verbose level from NVM=%d", dbg_verbose_val);
}

void TERM_DebugOut(const dbg_verbosity_lvl_t _lvl, String _fmt, ... )
{    
    va_list ap;
    char buf[MAX_DEBUG_MSG_LEN];

    if ( ( _lvl <= m_verbose_level ) && !m_uart_busy )
    {
        va_start (ap, _fmt);
        (void)vsnprintf(buf, sizeof(buf), _fmt.c_str(), ap);
        buf[sizeof(buf) - 1] = 0;
        va_end (ap);
        Log.info(buf); 
    }
}

void TERM_DebugOut_UART_Busy(const bool _busy )
{
    #define BLOCK_LOGS_WHILE_ACK
    #if defined(BLOCK_LOGS_WHILE_ACK)
    m_uart_busy = _busy;
    #endif
}

bool TERM_is_verbose_as(dbg_verbosity_lvl_t _min_level)
{
    return m_verbose_level >= _min_level;
}

bool set_dbg_verbose_level(dbg_verbosity_lvl_t _verbose_lvl)
{
    bool success = true;
    m_verbose_level = _verbose_lvl;
    return success;
}

void TERM_dump(const dbg_verbosity_lvl_t _lvl, void * _data, uint16_t _size, String _msg, uint8_t _num_bytes_per_line, uint8_t _num_bytes_before_space)
{
    #define NUM_SPACES_PER_LINE (_num_bytes_per_line / _num_bytes_before_space - 1 + (_num_bytes_per_line % _num_bytes_before_space > 0))
    uint8_t temp_string[_num_bytes_per_line * 2 + NUM_SPACES_PER_LINE + 1];
    uint8_t tmp_i=0;
    uint16_t i;
    
    if (_msg.length())
    {
        TERM_DebugOut(_lvl, "%s", _msg.c_str());
    }

    for(i=0; i<_size / _num_bytes_per_line * _num_bytes_per_line + (_size % _num_bytes_per_line ? _num_bytes_per_line : 0); i += _num_bytes_per_line)
    {
        tmp_i = 0;
        for (uint8_t j=0; j<_num_bytes_per_line; j++)
        {
            if (i + j < _size)
            {
                uint8_t nybble = ((uint8_t*)_data)[i + j] / 16;
                if (nybble < 10)
                {
                    temp_string[tmp_i++] = nybble + '0';
                }
                else
                {
                    temp_string[tmp_i++] = nybble - 10 + 'A';
                }
                
                nybble = ((uint8_t*)_data)[i + j] % 16;
                if (nybble < 10)
                {
                    temp_string[tmp_i++] = nybble + '0';
                }
                else
                {
                    temp_string[tmp_i++] = nybble - 10 + 'A';
                }
            }
            else
            {
                temp_string[tmp_i++] = ' ';
                temp_string[tmp_i++] = ' ';
            }
            
            if ((j + 1) % _num_bytes_before_space == 0)
            {
                temp_string[tmp_i++] = ' ';
            }
        }
        temp_string[sizeof(temp_string)-1] = '\0';
        TERM_DebugOut(_lvl, "  %s", temp_string);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Private Function Definitions
///////////////////////////////////////////////////////////////////////////////

bool get_dbg_verbose_level(dbg_verbosity_lvl_t* _verbose_lvl)
{
    bool success = true;
    *_verbose_lvl = m_verbose_level;
    return success;
}
