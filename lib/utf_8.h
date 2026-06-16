#ifndef UTF_8_H
#define UTF_8_H

#include <stdbool.h>
#include <stdint.h>

enum utf8_status { UTF8_VALID, UTF8_INVALID, UTF8_INCOMPLETE };

struct utf8_decode_result {
  enum utf8_status status;
  uint8_t bytes_consumed;
  uint8_t expected_bytes;
};

struct utf8_decode_result decode_utf8_char(const uint8_t *stream,
                                           uint64_t num_of_bytes_available);

#endif
