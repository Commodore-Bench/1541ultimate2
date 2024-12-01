#include <stdio.h>
#include <string.h>
#include <math.h>
#include "u64.h"
#include "hw_i2c_drv.h"
#include "hdmi_scan.h"
#include "dump_hex.h"
#include "color_timings.h" // FIXME: Doesn't belong here
#include "itu.h"

#define PLL1 0xC8
#define HUB  0x58
#define EXP1 0x40
#define EXP2 0x42
#define AUD  0x34

typedef struct {
    int n, m, p;
    float error;
    float vco;
    bool valid;
} t_setting;


#define min(a,b) ((a > b)?b:a)
//#define max(a,b) ((a > b)?a:b)
void calc_pll(const float ref, const float target, uint8_t *bytes)
{
    const float Fvco_min = 80.0f;
    const float Fvco_max = 201.0f;
    const int M_max = 511;
    const int N_max = 4095;
    const int P_max = 127;

    int max_P = min(P_max, int(Fvco_max / target)); // This is where fVCO is near but <= than max
    int min_P = max(1, int((Fvco_min / target) + 0.999)); // This is there fVCO is near but >= than min

    printf("Range P: %d..%d\n", min_P, max_P);
    printf("VCO: %.2f - %.2f\n", target * min_P, target * max_P);

    float best = 999.9f;
    t_setting setting;
    memset(&setting, 0, sizeof(setting));
    setting.valid = false;

    for(int p=min_P; p <= max_P; p++) {
        float vco = target * p;
        float ratio = vco / ref;
        // Find best N/M 
        for(int m=1; m <= M_max; m++) {
            int n = int(ratio * m + 0.5);
            if (n > N_max)
                break;
            float achieved = float(n) / float(m);
            float error = 1e6f * fabs((achieved / ratio) - 1.0f);
            if (error < best) {
                best = error;
                setting.valid = true;
                setting.n = n;
                setting.m = m;
                setting.p = p;
                setting.error = error;
                setting.vco = vco;
            }
        }
    }
    dump_hex_relative(&setting, sizeof(setting));
    // printf("N/M = %d/%d. P=%d. Error=%.3f ppm. VCO=%.2f\n", setting.n, setting.m, setting.p, setting.error, setting.vco);
    printf("N/M = %d/%d. P=%d.\n", setting.n, setting.m, setting.p);

    //np = max(0, 4 - int(math.log2(n/m)))
    int frac = setting.n / setting.m;
    int np = 4;
    while((frac >= 2) && (np > 0)) { np--; frac >>= 1; }

    int ntick = setting.n << np;
    int q = ntick / setting.m;
    int r = ntick - setting.m * q;

    printf("P: %d, N': %d, Q: %d, R: %d\n", np, ntick, q, r);

    int rnge = (setting.vco < 125.0f)? 0 :
               (setting.vco < 150.0f)? 1 : 
               (setting.vco < 175.0f)? 2 : 3;

    // Create a byte array of the bytes to be written to the device
    memset(bytes, 0, 16);
    bytes[4] = 0x57;  // 0 1 01 01 11 
    bytes[6] = setting.p;
    bytes[7] = setting.p;
    bytes[8] = setting.n >> 4;
    bytes[9] = ((setting.n & 15) << 4) | ((r >> 5) & 15);
    bytes[10] = ((r & 31) << 3) | ((q >> 3) & 7);
    bytes[11] = ((q & 7) << 5) | (np << 2) | rnge;

    dump_hex_relative(bytes, 16);
}

// PAL Color from 24 MHz: 57000505cf54534a
// HDMI Color from 54.18867 MHz: 570001010fa2caad

void initialize_usb_hub();
void nau8822_init(int channel);
extern "C" void ResetHdmiPll(void)
{
    U64_HDMI_PLL_RESET = 3;
    U64_HDMI_PLL_RESET = 0;
}
extern "C" void SetHdmiPll(t_video_mode mode, uint8_t _mode_bits);
extern "C" void SetVideoPll(t_video_mode mode);

extern "C" {
void custom_hardware_init()
{
    i2c = new Hw_I2C_Driver((volatile t_hw_i2c *)U64II_HW_I2C_BASE);

    // We are not yet running under FreeRTOS, so we cannot use mutexes here.

    // Initialize Audio codec
    nau8822_init(I2C_CHANNEL_1V8);

    // Initialize USB hub
    initialize_usb_hub();

/*
    U64_HDMI_ENABLE = 0;
    uint8_t bytes[16];
    printf("C64 PLL\n");
    calc_pll(24.0f, 31.527928889f, bytes); // should result in 3317/505, P=5
    printf("HDMI PLL\n");
    calc_pll(54.18867f, 148.8699824f, bytes); // should result in a multiplication of 250/91.
    //t_video_mode systemMode = e_PAL_50;
    t_video_mode systemMode = e_NTSC_60;
    const t_video_color_timing *ct = color_timings[(int)systemMode];

    printf("HDMI clock 1: %08x\n", U64_CLOCK_FREQ);
    SetVideoPll(systemMode);
    SetHdmiPll(systemMode, ct->mode_bits);
    C64_VIDEOFORMAT = ct->mode_bits;
    printf("Mode bits: %b\n", ct->mode_bits);
    ResetHdmiPll();
    wait_ms(300);
    printf("HDMI clock 2: %08x\n", U64_CLOCK_FREQ);
    wait_ms(300);
    printf("HDMI clock 3: %08x\n", U64_CLOCK_FREQ);
    SetVideoMode1080p(systemMode);
    wait_ms(300);
    printf("HDMI clock 4: %08x\n", U64_CLOCK_FREQ);
*/

    U64_HDMI_REG = U64_HDMI_DDC_ENABLE;
    i2c->set_channel(I2C_CHANNEL_1V8);
    i2c->i2c_write_byte(0x40, 0x01, 0x00); // Output Port
    i2c->i2c_write_byte(0x40, 0x03, 0x00); // All pins output
    i2c->i2c_write_byte(0x40, 0x02, 0x00); // No polarity inversion

    // Column is A  (drive), Row is B  (read)
    // Port 0,               Port 1
    i2c->i2c_write_byte(0x42, 0x06, 0x00); // All pins output on port 0
    i2c->i2c_write_byte(0x42, 0x07, 0xFF); // All pins input on port 1
    i2c->i2c_write_byte(0x42, 0x02, 0x00); // Output Port, all columns selected

}
}

extern "C" void SetVideoPll(t_video_mode mode)
{
    const t_video_color_timing *ct = color_timings[(int)mode];

    float m = ct->m + (ct->frac / (65536.0f * 65536.0f));
    m *= 50.0f;
    m /= 30.0f;

    printf("--> Requested video frequency = %.3f\n", m);
    uint8_t hw_version = (U2PIO_BOARDREV >> 3);

    uint8_t bytes[16];
    calc_pll(24.0f, m, bytes);
    i2c->i2c_lock("SetVideoPll");
    i2c->set_channel(hw_version == 0x15 ? I2C_CHANNEL_HDMI : I2C_CHANNEL_1V8);
    i2c->i2c_write_block(PLL1, 0x14, bytes+4, 8);
    i2c->i2c_unlock();
}

extern "C" void SetHdmiPll(t_video_mode mode, uint8_t mode_bits)
{
    const t_video_color_timing *ct = color_timings[(int)mode];
    printf("--> Requested HDMI frequency mode = %d (%d Hz)\n", mode, ct->mode_bits & VIDEO_FMT_60_HZ ? 60 : 50);
    const uint8_t init_hdmi_50[] = { 0x57, 0x00, 0x01, 0x01, 0x0F, 0xA2, 0xCA, 0xAD };
    const uint8_t init_hdmi_60[] = { 0x57, 0x00, 0x01, 0x01, 0x55, 0xf7, 0x82, 0x89 };

    uint8_t hw_version = (U2PIO_BOARDREV >> 3);

    C64_VIDEOFORMAT = VIDEO_FMT_60_HZ;
    i2c->i2c_lock("SetHdmiPll");
    i2c->set_channel(hw_version == 0x15 ? I2C_CHANNEL_1V8 : I2C_CHANNEL_HDMI);
    i2c->i2c_write_byte(PLL1, 0x81, 0x18); // LVCMOS input, powerdown
    i2c->i2c_write_block(PLL1, 0x14, (ct->mode_bits & VIDEO_FMT_60_HZ) ? init_hdmi_60 : init_hdmi_50, 8);
    C64_VIDEOFORMAT = mode_bits;
    i2c->i2c_write_byte(PLL1, 0x81, 0x08); // LVCMOS input, powerup
    i2c->i2c_unlock();
}

