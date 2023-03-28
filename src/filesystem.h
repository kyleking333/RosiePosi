//------------------------------------------------------------------------------
// filesystem.h
// Kyle King
// 2023
//------------------------------------------------------------------------------

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

//------------------------------------------------------------------------------
// C library includes
//------------------------------------------------------------------------------
#include <stdint.h>

//------------------------------------------------------------------------------
// Platform includes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Application includes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Data types && preprocessors
//------------------------------------------------------------------------------
typedef enum
{
    FILE_ACCMTR_SAMPLE,
    // new files here
    NUM_FILES
} file_id_t;
#define MAX_NUM_FILES 255
//static_assert(NUM_FILES <= MAX_NUM_FILES);

typedef enum
{
    FILE_ACCESS_TYPE_READ_ONLY,
    FILE_ACCESS_TYPE_READ_WRITE,
    FILE_ACCESS_TYPE_WRITE_ONLY,
    // new file access types here
    NUM_FILE_ACCESS_TYPES
} file_access_t;

//------------------------------------------------------------------------------
// Extern variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Public functions
//------------------------------------------------------------------------------
void init_filesystem(void);
bool activate_file(file_id_t file_id, file_access_t access_type);
bool append_active_file(void * data, uint16_t size);
bool read_active_file(int offset, void * data, uint16_t size);
bool get_active_file_size(uint32_t * size);

#endif //FILESYSTEM_H
