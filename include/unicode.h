/*
 * unicode.h
 *
 * Unicode - module with some helper unicode functions.
 *
 * SFML uses UTF32 encoding for texts and there is no need to use any full
 * featured library just for a couple of encoding conversion functions.
 *
 * */

size_t  utf8_strlen(char *string);
void    utf8to32_strcpy(wchar_t *dest, char *src);
