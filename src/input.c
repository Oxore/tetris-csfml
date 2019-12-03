#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "vector.h"
#include "input.h"

static size_t utf8_detect_back(const char *text)
{
    if ((*text-- & 0x80) == 0)
        return 1;

    if ((*text-- & 0xE0) == 0xC0)
        return 2;

    if ((*text-- & 0xF0) == 0xE0)
        return 3;

    if ((*text-- & 0xF8) == 0xF0)
        return 4;

    return 0;
}

void input_clear(struct input *input)
{
    memset(input->text, 0, sizeof(input->text));
    input->nbytes = 0;
}

void input_append_cstring_n(struct input *input, const char *str, size_t n)
{
    assert(input->nbytes < sizeof(input->text));

    size_t size = sizeof(input->text);
    size_t *nbytes = &input->nbytes;
    size_t appended_len = strlen(str);

    assert(appended_len < n); // c string with terminator must fit

    if (size - *nbytes < appended_len)
        return;

    strncpy(&input->text[*nbytes], str, n);
    *nbytes += appended_len;
}

void input_rm_last_char(struct input *input)
{
    assert(input->nbytes < sizeof(input->text));

    size_t *nbytes = &input->nbytes;

    if (*nbytes == 0)
        return;

    size_t clen = utf8_detect_back(&input->text[*nbytes - 1]);
    memset(&input->text[*nbytes - clen], 0, clen);

    assert(*nbytes >= clen);

    *nbytes -= clen;
}
