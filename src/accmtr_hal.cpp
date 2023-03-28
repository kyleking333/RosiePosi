//------------------------------------------------------------------------------
// accmtr_hal.cpp
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
#include "accmtr_hal.h"
#include "utils.h"

//------------------------------------------------------------------------------
// Data types
//------------------------------------------------------------------------------
// Register mapping
#define REG_OUT_T_L_ADDR (0x0D)
#define REG_OUT_T_H_ADDR (0x0E)
#define REG_WHO_AM_I_ADDR (0x0F)
#define REG_CTRL1_ADDR (0x20)
#define REG_CTRL2_ADDR (0x21)
#define REG_CTRL3_ADDR (0x22)
#define REG_CTRL4_INT1_PAD_CTRL_ADDR (0x23)
#define REG_CTRL5_INT2_PAD_CTRL_ADDR (0x24)
#define REG_CTRL6_ADDR (0x25)
#define REG_OUT_T_ADDR (0x26)
#define REG_STATUS_ADDR (0x27)
    #define STATUS_FIFO_THRESH_POS 7
#define REG_OUT_X_L_ADDR (0x28)
#define REG_OUT_X_H_ADDR (0x29)
#define REG_OUT_Y_L_ADDR (0x2A)
#define REG_OUT_Y_H_ADDR (0x2B)
#define REG_OUT_Z_L_ADDR (0x2C)
#define REG_OUT_Z_H_ADDR (0x2D)
#define REG_FIFO_CTRL_ADDR (0x2E)
#define REG_FIFO_SAMPLES_ADDR (0x2F)
#define REG_TAP_THS_X_ADDR (0x30)
#define REG_TAP_THS_Y_ADDR (0x31)
#define REG_TAP_THS_Z_ADDR (0x32)
#define REG_INT_DUR_ADDR (0x33)
#define REG_WAKE_UP_THS_ADDR (0x34)
#define REG_WAKE_UP_DUR_ADDR (0x35)
#define REG_FREE_FALL_ADDR (0x36)
#define REG_STATUS_DUP_ADDR (0x37)
#define REG_WAKE_UP_SRC_ADDR (0x38)
#define REG_TAP_SRC_ADDR (0x39)
#define REG_SIXD_SRC_ADDR (0x3A)
#define REG_ALL_INT_SRC_ADDR (0x3B)
#define REG_X_OFS_USR_ADDR (0x3C)
#define REG_Y_OFS_USR_ADDR (0x3D)
#define REG_Z_OFS_USR_ADDR (0x3E)
#define REG_CTRL7_ADDR (0x3F)

#define ACCMTR_I2C_ADDR 0x18
#define FIFO_LEN 32
#define DATA_RES_NUM_BITS 16
#define NUM_SAMPLES 50

//------------------------------------------------------------------------------
// Private variables
//------------------------------------------------------------------------------
static bool m_disabled = false;
static bool m_interrupted = false;
static accmtr_sample_t m_samples[NUM_SAMPLES];
static uint8_t m_samples_i=0;

//------------------------------------------------------------------------------
// Private function declarations
//------------------------------------------------------------------------------
bool read_reg(uint8_t reg_addr, uint8_t *out);
bool write_reg(uint8_t reg_addr, uint8_t val);
void fifo_int_handler(void);

uint8_t read_status(void);
uint8_t num_fifo_samples(void);
accmtr_sample_t read_sample(void);

///////////////////////////////////////////////////////////////////////////////
// Public Function Definitions
///////////////////////////////////////////////////////////////////////////////

void accmtr_init(void)
{
    pinMode(D2, OUTPUT);
    digitalWrite(D2, LOW); //accmtr gnd (also SD0)
    pinMode(D3, OUTPUT);
    digitalWrite(D3, HIGH); //accmtr vcc (also CS)
    
    pinMode(D5, INPUT); //accmtr int
    attachInterrupt(D5, fifo_int_handler, RISING);

    delay(250); // guessing some startup time is needed

    Wire.setSpeed(400000);
    Wire.begin();

    if (!accmtr_is_active())
    {
        m_disabled = true;
        return;
    }

    // configure accmtr

    //ctrl1 -- 50Hz sample rate, low power, 12bit res
    uint8_t ctl1 = (0x04 << 4) | (0x00<<2) | 0x00;
    #define PASSES_CTL1(rb) ((rb) == ctl1)

    //ctrl2 -- don't do boot/softreset stuff, pull up CS, contin update, I2C en
    uint8_t ctl2 = 0 | 0 | 0 | 0 | 0 | (1<<2) | 0 | 0;
    #define PASSES_CTL2(rb) ((rb) == ctl2)

    //ctrl3 -- no post, pp, int pulsed, int act high, use lowest bit for single data conversion on demand mode, sdcodm = OFF
    uint8_t ctl3 = 0 | 0 | 0 | 0 | (1<<1) | 0;
    #define PASSES_CTL3(rb) ((rb) == ctl3)

    //ctrl4 -- int1 triggers on fifo threshold
    uint8_t ctl4 = 0 | 0 | 0 | 0 | 0 | 0 | (1<<1) | 0;
    #define PASSES_CTL4(rb) ((rb) == ctl4)

    //ctrl5 -- int2 triggers on nothing
    uint8_t ctl5 = 0;
    #define PASSES_CTL5(rb) ((rb) == ctl5)

    //ctrl6 -- bandwidth filtering=ODR/2, range=+/-2g, FDS=LowPass, low noise=false
    uint8_t ctl6 = 0 | 0 | 0 | 0 | 0 | 0;
    #define PASSES_CTL6(rb) ((rb) == ctl6)

    //fifo ctrl -- stop when full, threshold=25/32
    uint8_t fifo_ctl = (1<<5) | 25;
    #define PASSES_FIFO(rb) ((rb) == fifo_ctl)

    uint8_t rb;
    if (
       write_reg(REG_CTRL1_ADDR, ctl1)
    && read_reg(REG_CTRL1_ADDR, &rb)
    && PASSES_CTL1(rb)

    && write_reg(REG_CTRL2_ADDR, ctl2)
    && read_reg(REG_CTRL2_ADDR, &rb)
    && PASSES_CTL2(rb)

    && write_reg(REG_CTRL3_ADDR, ctl3)
    && read_reg(REG_CTRL3_ADDR, &rb)
    && PASSES_CTL3(rb)

    && write_reg(REG_CTRL4_INT1_PAD_CTRL_ADDR, ctl4)
    && read_reg(REG_CTRL4_INT1_PAD_CTRL_ADDR, &rb)
    && PASSES_CTL4(rb)

    && write_reg(REG_CTRL5_INT2_PAD_CTRL_ADDR, ctl5)
    && read_reg(REG_CTRL5_INT2_PAD_CTRL_ADDR, &rb)
    && PASSES_CTL5(rb)

    && write_reg(REG_CTRL6_ADDR, ctl6)
    && read_reg(REG_CTRL6_ADDR, &rb)
    && PASSES_CTL6(rb)

    && write_reg(REG_FIFO_CTRL_ADDR, fifo_ctl)
    && read_reg(REG_FIFO_CTRL_ADDR, &rb)
    && PASSES_FIFO(rb)
    )
    {
        TERM_crt("config successful");
    }
}

void accmtr_loop(void)
{
    if (m_interrupted)
    {
        m_interrupted = false;
        uint8_t num_samples = num_fifo_samples();
        for(uint8_t i=0; i<num_samples; i++)
        {
            m_samples[(m_samples_i++ + NUM_SAMPLES) % NUM_SAMPLES] = read_sample();
            if (m_samples_i >= NUM_SAMPLES)
            {
                m_samples_i = 0;
            }
            //TERM_crt("sample[%d]=%f,%f,%f", i, ((float)sample.x) / ACCMTR_1G, ((float)sample.y) / ACCMTR_1G, ((float)sample.z) / ACCMTR_1G);
        }
    }
}

uint8_t read_chip_id(void)
{
    uint8_t addr;
    if (read_reg(REG_WHO_AM_I_ADDR, &addr))
    {
        return addr;
    }
    else
    {
        return ACCMTR_CHIP_ID_NOT_FOUND;
    }
}

bool accmtr_is_active(void)
{
    uint8_t returned_id = read_chip_id();
    if (returned_id == ACCMTR_CHIP_ID)
    {
        TERM_trc("accmtr id=%02X", returned_id);
        return returned_id == 0x44;
    }
    else
    {
        TERM_crt("Could not find accmtr ID");
        return false;
    }
}

bool get_next_sample(accmtr_sample_t * _sample)
{
    static uint8_t s_last_gather_i = 0;
    if (s_last_gather_i != m_samples_i)
    {
        *_sample = m_samples[(s_last_gather_i++ + NUM_SAMPLES)%NUM_SAMPLES];
        if (s_last_gather_i >= NUM_SAMPLES)
        {
            s_last_gather_i = 0;
        }
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Private Function Definitions
///////////////////////////////////////////////////////////////////////////////

bool read_reg(uint8_t _reg_addr, uint8_t *out)
{
    uint8_t returned_id;
    Wire.beginTransmission(ACCMTR_I2C_ADDR);
    Wire.write(_reg_addr);
    Wire.endTransmission(false);
    Wire.requestFrom(ACCMTR_I2C_ADDR, 1);
    uint32_t start = millis();
    while (!Wire.available() && ms_since(start) < 100)
    {
        //delay(10);
    }

    if (Wire.available())
    {
        *out = Wire.read();
        return true;
    }
    else
    {
        return false;
    }

}

bool write_reg(uint8_t _reg_addr, uint8_t val)
{
    uint8_t tx[2] = {_reg_addr, val};
    Wire.beginTransmission(ACCMTR_I2C_ADDR);
    uint8_t num_bytes_written = Wire.write(tx, sizeof(tx));
    Wire.endTransmission(true);
    return num_bytes_written == sizeof(tx);
}

// runs in interrupt context
void fifo_int_handler(void)
{
    m_interrupted = true;
}

uint8_t read_status(void)
{
    uint8_t status = 0x00;

    if (!read_reg(REG_STATUS_ADDR, &status))
    {
        TERM_crt("acmmtr read status failed!");
    }
    return status;
}

uint8_t num_fifo_samples(void)
{
    uint8_t num_samples = 0;

    if (!read_reg(REG_FIFO_SAMPLES_ADDR, &num_samples))
    {
        TERM_crt("acmmtr read num fifo samples failed!");
    }
    return num_samples & 0x3F;
}

accmtr_sample_t read_sample(void)
{
    uint8_t v[2];
    accmtr_sample_t sample;

    if (!read_reg(REG_OUT_X_L_ADDR, &v[0]))
    {
        TERM_crt("acmmtr read x samples failed!");
    }
    if (!read_reg(REG_OUT_X_H_ADDR, &v[1]))
    {
        TERM_crt("acmmtr read x samples failed!");
    }
    sample.x = (v[1] << 8) | v[0];

    if (!read_reg(REG_OUT_Y_L_ADDR, &v[0]))
    {
        TERM_crt("acmmtr read y samples failed!");
    }
    if (!read_reg(REG_OUT_Y_H_ADDR, &v[1]))
    {
        TERM_crt("acmmtr read y samples failed!");
    }
    sample.y = (v[1] << 8) | v[0];

    if (!read_reg(REG_OUT_Z_L_ADDR, &v[0]))
    {
        TERM_crt("acmmtr read z samples failed!");
    }
    if (!read_reg(REG_OUT_Z_H_ADDR, &v[1]))
    {
        TERM_crt("acmmtr read z samples failed!");
    }
    sample.z = (v[1] << 8) | v[0];
    return sample;
}
