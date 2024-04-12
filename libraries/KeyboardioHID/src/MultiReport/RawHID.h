

#ifndef RAW_HID_H_
#define RAW_HID_H_


#if defined(__cplusplus)


#include "Stream.h"

class RawHID_ : public Stream {
public:
  RawHID_(void){};
  void begin(void);
  virtual ~RawHID_();

  // Stream API
  virtual int available(void);
  virtual int peek(void);

  virtual int read(void);
  size_t read(uint8_t *buffer, size_t size);

  virtual void flush(void);
  virtual size_t write(uint8_t);

  virtual size_t write(const uint8_t *buffer, size_t size);
  size_t write(const char *buffer, size_t size) {
    return write((const uint8_t *)buffer, size);
  }

  virtual int availableForWrite(void);
  using Print::write; // pull in write(str) from Print

};


// "Serial" is used with TinyUSB CDC
extern RawHID_ RawHID;

#endif // __cplusplus


#endif
