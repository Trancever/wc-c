#include "utf_8.h"

struct decode_result decode_leading_byte(uint8_t byte) {
  // has highest order bit 0 -> ascii -> 1 byte
  bool is_one_byte = (byte & 0x80) == 0x00;

  if (is_one_byte) {
    return (struct decode_result){1, false };
  }

  // has 2 highest order bits set to 1 and 3rd highest order bit set to 0 -> 2 byte unicode
  bool is_two_bytes = (byte & 0xE0) == 0xC0;

  if (is_two_bytes) {
    return (struct decode_result){2, false };
  }

  // has 3 highest order bits set to 1 and 4th highest order bit set to 0 -> 3 byte unicode
  bool is_three_bytes = (byte & 0xF0) == 0xE0;

  if (is_three_bytes) {
    return (struct decode_result){3, false };
  }

  // has 4 highest order bits set to 1 and 5th highest order bit set to 0 -> 4 byte unicode
  bool is_four_bytes = (byte & 0xF8) == 0xF0;

  if (is_four_bytes) {
    return (struct decode_result){4, false };
  }

  return (struct decode_result){1, true };
}
