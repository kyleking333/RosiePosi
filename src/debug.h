//------------------------------------------------------------------------------
// debug.h
// Kyle King
// 2023
//------------------------------------------------------------------------------

#ifndef DEBUG_INCLUDED
#define DEBUG_INCLUDED

//------------------------------------------------------------------------------
// C library includes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Platform includes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Application includes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Data types && preprocessors
//------------------------------------------------------------------------------
#define MAX_DEBUG_MSG_LEN 200

typedef enum {
    VRB_OFF,
    VRB_CRITICAL,   // LOW
    VRB_DEBUG,      // MED
    VRB_TRACE       // HIGH
} dbg_verbosity_lvl_t;

#define TERM_crt(...)  TERM_DebugOut(VRB_CRITICAL, __VA_ARGS__)
#define TERM_dbg(...)  TERM_DebugOut(VRB_DEBUG, __VA_ARGS__)
#define TERM_trc(...)  TERM_DebugOut(VRB_TRACE, __VA_ARGS__)

//------------------------------------------------------------------------------
// Extern variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Public functions
//------------------------------------------------------------------------------
void TERM_init(bool override_to_debug);
void TERM_DebugOut( const dbg_verbosity_lvl_t lvl, String fmt, ... );
void TERM_DebugOut_UART_Busy( const bool busy );
bool TERM_is_verbose_as(dbg_verbosity_lvl_t min_level);
bool set_dbg_verbose_level(dbg_verbosity_lvl_t verbose_lvl);

void TERM_dump(const dbg_verbosity_lvl_t lvl, void * data, uint16_t size, String msg="", uint8_t num_bytes_per_line=16, uint8_t num_bytes_before_space=4);

#endif //DEBUG_INCLUDED
