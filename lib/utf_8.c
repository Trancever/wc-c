#include "utf_8.h"
#include <stdint.h>
#include <stdlib.h>

struct leading_byte_decode_result {
  uint8_t num_of_bytes;
  bool has_error;
};

static struct leading_byte_decode_result decode_leading_byte(uint8_t byte) {
  uint8_t num_of_bytes = 1;
  bool has_error = false;

  if ((byte & 0x80) == 0x00) {
    num_of_bytes = 1;
  } else if ((byte & 0xE0) == 0xC0) {
    num_of_bytes = 2;
  } else if ((byte & 0xF0) == 0xE0) {
    num_of_bytes = 3;
  } else if ((byte & 0xF8) == 0xF0) {
    num_of_bytes = 4;
  } else {
    has_error = true;
  }

  return (struct leading_byte_decode_result){num_of_bytes, has_error};
}

static bool decode_continuation_byte(uint8_t byte) {
  bool valid = true;

  // For not, just check 2 first most significant bits
  if ((byte & 0xC0) == 0x80) {
    return valid;
  }

  valid = false;
  return valid;
}

struct utf8_decode_result decode_utf8_char(const uint8_t *stream,
                                           uint64_t num_of_bytes_available) {
  if (num_of_bytes_available == 0) {
    exit(1);
  }

  struct leading_byte_decode_result leading_byte_decode_result =
      decode_leading_byte(*stream);

  if (leading_byte_decode_result.has_error) {
    return (struct utf8_decode_result){UTF8_INVALID, 1, 1};
  }

  if (leading_byte_decode_result.num_of_bytes > num_of_bytes_available) {
    return (struct utf8_decode_result){UTF8_INCOMPLETE, 0,
                                       leading_byte_decode_result.num_of_bytes};
  }

  if (leading_byte_decode_result.num_of_bytes == 1) {
    return (struct utf8_decode_result){UTF8_VALID, 1, 1};
  }

  uint8_t continuation_byte_index = 1;

  while (continuation_byte_index < leading_byte_decode_result.num_of_bytes) {
    if (decode_continuation_byte(*(stream + continuation_byte_index)) ==
        false) {
      return (struct utf8_decode_result){
          UTF8_INVALID, continuation_byte_index,
          leading_byte_decode_result.num_of_bytes};
    }

    continuation_byte_index++;
  }

  return (struct utf8_decode_result){UTF8_VALID,
                                     leading_byte_decode_result.num_of_bytes,
                                     leading_byte_decode_result.num_of_bytes};
}
