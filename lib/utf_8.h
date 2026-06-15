#ifndef UTF_8_H
#define UTF_8_H

#include <stdint.h>
#include <stdbool.h>

struct decode_result {
  uint8_t num_of_bytes;
  bool has_error;
};

struct decode_result decode_leading_byte(uint8_t bytes);

#endif
