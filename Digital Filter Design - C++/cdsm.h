// -------------------------------------
/// DSM digital signal monitor class
/// to be sent by UART, recv. by RTWin
/// 15.02.2017 Wolfgang Schulter
// -------------------------------------
#ifndef __DSM_H__           // only once
#define __DSM_H__

#define NDSM_BYTES    26    // number of bytes to be sent, starting from sig
#define NDSM_SIGNALS  11    // number of signals
class Cdsm {
  public:
    Cdsm() {
      sig = 0xAD;
      seq = 0;
      bsend = 1;            // 1: send dsm initially
    }
    uint16_t calc_xor16(uint8_t *buf, int n) {  // own simple xor-crc
      uint16_t y = 0;
      uint16_t *pbuf = (uint16_t *)buf;
      for (int i=0; i < (n >> 1); i++) y ^= *pbuf++;
      return y;
    }
    void init() {
      SerialUSB.begin(80000000);     // dsm: hi-speed native UART (CDC mode)
    }
    void send() {
      if (!bsend) return;
      n = ((NDSM_BYTES) & 63) | (((seq++) & 3) << 6); // upper 2 bits as sequence counter
      crc16 = 0;
      crc16 = calc_xor16((unsigned char *)&sig, NDSM_BYTES-2);  // own xor-16
      SerialUSB.write((const char *)&sig, NDSM_BYTES);
    }
    bool bsend;
    int16_t seq;
    // -----------------------------------
    uint8_t sig;    // the signature, RTWin expects. send starts here
    uint8_t n;      // the number of signal bytes ^seq
    int16_t adc_in;
    int16_t fgen_out;
    int16_t filt_x;
    int16_t filt_y;
    int16_t dac0;
    int16_t dac1;
    int16_t dt;
    float aux0;
    float aux1;
    uint16_t crc16;             
    const char *sig_name[NDSM_SIGNALS] = {
      "u2:sigseq",
      "i2:adc_in",
      "i2:fgen_out",
      "i2:filt_x",
      "i2:filt_y",
      "i2:dac0",
      "i2:dac1",
      "i2:dt",
      "f4:aux0",
      "f4:aux1",
      "u2:crc16"            
    };
};

extern Cdsm dsm;  // the one dsm object

#endif // #ifndef __DSM_H__      // only once
