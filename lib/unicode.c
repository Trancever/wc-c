#include "unicode.h"

bool is_unicode_whitespace(uint32_t cp) {
  return cp == 0x0009 || // tab
         cp == 0x000A || // newline
         cp == 0x000B ||
         cp == 0x000C ||
         cp == 0x000D ||
         cp == 0x0020 ||
         cp == 0x0085 ||
         cp == 0x00A0 ||
         cp == 0x1680 ||
         (cp >= 0x2000 && cp <= 0x200A) ||
         cp == 0x2028 ||
         cp == 0x2029 ||
         cp == 0x202F ||
         cp == 0x205F ||
         cp == 0x3000;
}
