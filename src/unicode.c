#include <stdlib.h>
#include <string.h>

#include "unicode.h"

static inline unsigned int utf8_char_len(unsigned char c)
{
    if (c > 0x00 && c < 0xC0)
        return 1;
    else if (c >= 0xC2 && c < 0xE0)
        return 2;
    else if (c >= 0xE0 && c < 0xF0)
        return 3;
    else if (c >= 0xF0 && c < 0xF5)
        return 4;
    else
        return 0;
}

unsigned long utf8_strlen(char *string)
{
    unsigned long len = 0, keep = 0;
    for (char *c = string; *c; (keep ? --keep : ++len), ++c)
        if (!keep)
            keep = (keep = utf8_char_len(*c)) ? keep - 1 : keep;
    return len;
}

void utf8to32_strcpy(wchar_t *dest, char *src)
{
    wchar_t *dc = dest;
    char *c = src;
    unsigned long len = 0;
    while (*c) {
        int clen = utf8_char_len(*c);
        if (clen == 1) {
            dc[len] = c[0] & 0x7f;
        } else if (clen == 2) {
            dc[len] = ((c[0] & 0x1f) << 6) | ((c[1] & 0x3f) << 0);
        } else if (clen == 3) {
            dc[len] = ((c[0] & 0x0f) << 12) | ((c[1] & 0x3f) << 6) | ((c[2] & 0x3f) << 0);
        } else if (clen == 4) {
            dc[len] = ((c[0] & 0x07) << 18) | ((c[1] & 0x3f) << 12) | ((c[2] & 0x3f) << 6) | ((c[3] & 0x3f) << 0);
        } else {
            dc[len] = 0;
            return;
        }
        c += clen;
        ++len;
    }
    dc[len] = 0;
}
