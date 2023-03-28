//------------------------------------------------------------------------------
// filesystem.cpp
// Kyle King
// 2023
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// C library includes
//------------------------------------------------------------------------------
#include <fcntl.h>

//------------------------------------------------------------------------------
// Platform includes
//------------------------------------------------------------------------------
#include <Particle.h>

//------------------------------------------------------------------------------
// Application includes
//------------------------------------------------------------------------------
#include "filesystem.h"

//------------------------------------------------------------------------------
// Data types
//------------------------------------------------------------------------------
typedef struct
{
    file_id_t id;
    int fd;
    file_access_t access_type;
} file_meta_t;

//------------------------------------------------------------------------------
// Private variables
//------------------------------------------------------------------------------
static file_meta_t m_active_file;
static char m_tmp_fname[sizeof("file00")+1]; // accessed via construct_filename()

//------------------------------------------------------------------------------
// Private function declarations
//------------------------------------------------------------------------------
char* construct_filename(file_id_t file_id);
int construct_file_flags(file_access_t access_type);
void invalidate_active_file(void);

///////////////////////////////////////////////////////////////////////////////
// Public Function Definitions
///////////////////////////////////////////////////////////////////////////////

void init_filesystem(void)
{
    invalidate_active_file();
}

bool activate_file(file_id_t _file_id, file_access_t _access_type)
{
    if (_file_id >= NUM_FILES)
    {
        return false;
    }

    if (m_active_file.id == _file_id && m_active_file.access_type == _access_type)
    {
        return true;
    }

    if (m_active_file.id < NUM_FILES)
    {
        (void)close(m_active_file.fd);
    }

    m_active_file.id = _file_id;
    m_active_file.access_type = _access_type;
    m_active_file.fd = open(construct_filename(m_active_file.id), construct_file_flags(_access_type));
    if (m_active_file.fd >= 3)
    {
        return true;
    }
    else
    {
        invalidate_active_file();
        return false;
    }
}

bool append_active_file(void * _data, uint16_t _size)
{
    if (m_active_file.id < NUM_FILES
    && (m_active_file.access_type != FILE_ACCESS_TYPE_READ_ONLY)
    && (lseek(m_active_file.fd, 0, SEEK_END) < INT64_MAX) // should always be true
    && (write(m_active_file.fd, _data, _size) == _size)
    && (fsync(m_active_file.fd) == 0)) // for now, make sure all writes hit the flash (pending battery checking)
    {
        return true;
    }
    return false;
}

bool read_active_file(int _offset, void * _data, uint16_t _size) 
{
    uint32_t file_size; // upcasted to signed value later for signed comparisons

    if (get_active_file_size(&file_size)
    && (_size <= file_size)
    && (_offset <= (int64_t)file_size)
    && (_size + _offset <= (int64_t)file_size)
    && (m_active_file.access_type != FILE_ACCESS_TYPE_WRITE_ONLY)
    && (lseek(m_active_file.fd, _offset, _offset >= 0 ? SEEK_SET : SEEK_END) < (int64_t)file_size)
    && (read(m_active_file.fd, _data, _size) == _size))
    {
        return true;
    }
    return false;
}

bool get_active_file_size(uint32_t * _size)
{
    if (m_active_file.id >= NUM_FILES)
    {
        return false;
    }

    struct stat info;
    if (fstat(m_active_file.fd, &info) == 0)
    {
        *_size = (uint32_t)info.st_size;
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Private Function Definitions
///////////////////////////////////////////////////////////////////////////////

char* construct_filename(file_id_t _file_id)
{
    sprintf(m_tmp_fname, "file%02X", _file_id);
    return m_tmp_fname;
}

int construct_file_flags(file_access_t _access_type)
{
    switch (_access_type)
    {
        case FILE_ACCESS_TYPE_READ_ONLY:
        {
            return O_RDONLY | O_CREAT;
        }
        case FILE_ACCESS_TYPE_READ_WRITE:
        {
            return O_RDWR | O_CREAT;
        }
        case FILE_ACCESS_TYPE_WRITE_ONLY:
        {
            return O_WRONLY | O_CREAT;
        }
        default:
        {
            return 0;
        }
    }
}

void invalidate_active_file(void)
{
    m_active_file.id = NUM_FILES;
    m_active_file.fd = 0;
    m_active_file.access_type = NUM_FILE_ACCESS_TYPES;
}
