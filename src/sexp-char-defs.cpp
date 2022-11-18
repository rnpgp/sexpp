/**
 *
 * Copyright (c) 2022, [Ribose Inc](https://www.ribose.com).
 * All rights reserved.
 * This file is a part of RNP sexp library
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other matrials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Original copyright
 *
 * SEXP implementation code sexp-input.c
 * Ron Rivest
 * 7/21/1997
 */

#include <sexp/sexp-error.h>
#include <sexp/sexp.h>

namespace sexp {

/**************************************/
/* CHARACTER ROUTINES AND DEFINITIONS */
/**************************************/
bool          sexp_char_defs::initialized = false;
unsigned char sexp_char_defs::upper[256];       /* upper case version of c */
unsigned char sexp_char_defs::decvalue[256];    /* a value of c as dec digit */
unsigned char sexp_char_defs::hexvalue[256];    /* a value of c as a hex digit */
bool          sexp_char_defs::base64digit[256]; /* c is base64 digit */
unsigned char sexp_char_defs::base64value[256]; /* a value of c as base64 digit */
bool          sexp_char_defs::tokenchar[256];   /* c can be in a token */

/*
 * sexp_char_defs::initializeCharacterTables
 * initializes all of the above arrays
 */
void sexp_char_defs::initialize_character_tables()
{
    int i;
    for (i = 0; i < 256; i++) {
        upper[i] = i;
        base64digit[i] = false;
        tokenchar[i] = false;
    }
    for (i = 'a'; i <= 'z'; i++)
        upper[i] = i - 'a' + 'A';
    for (i = '0'; i <= '9'; i++) {
        base64digit[i] = true;
        decvalue[i] = hexvalue[i] = i - '0';
        base64value[i] = (i - '0') + 52;
    }
    for (i = 'a'; i <= 'f'; i++) {
        hexvalue[i] = hexvalue[upper[i]] = i - 'a' + 10;
    }
    for (i = 'a'; i <= 'z'; i++) {
        base64digit[i] = base64digit[upper[i]] = true;
        base64value[i] = i - 'a' + 26;
        base64value[upper[i]] = i - 'a';
    }
    base64digit['+'] = base64digit['/'] = true;
    base64value['+'] = 62;
    base64value['/'] = 63;
    base64value['='] = 0;
    for (i = 'a'; i <= 'z'; i++)
        tokenchar[i] = tokenchar[upper[i]] = true;
    for (i = '0'; i <= '9'; i++)
        tokenchar[i] = true;
    tokenchar['-'] = true;
    tokenchar['.'] = true;
    tokenchar['/'] = true;
    tokenchar['_'] = true;
    tokenchar[':'] = true;
    tokenchar['*'] = true;
    tokenchar['+'] = true;
    tokenchar['='] = true;
}

/*
 * sexp_char_defs::is_white_space(c)
 * Returns true if c is a whitespace character (space, tab, etc. ).
 */
bool sexp_char_defs::is_white_space(int c)
{
    return ((c >= 0 && c <= 255) && std::isspace(c));
}

/*
 * sexp_char_defs::is_hex_digit(c)
 * Returns true if c is a hexadecimal digit.
 */
bool sexp_char_defs::is_hex_digit(int c)
{
    return ((c >= 0 && c <= 255) && std::isxdigit(c));
}

/*
 * sexp_char_defs::is_dec_digit(c)
 * Returns true if c is a decimal digit.
 */
bool sexp_char_defs::is_dec_digit(int c)
{
    return ((c >= 0 && c <= 255) && std::isdigit(c));
}

/*
 * sexp_char_defs::is_base64_digit(c)
 * returns true if c is a base64 digit A-Z,a-Z,0-9,+,/
 */
bool sexp_char_defs::is_base64_digit(int c)
{
    return ((c >= 0 && c <= 255) && base64digit[c]);
}

/*
 * sexp_char_defs::is_token_char(c)
 * Returns true if c is allowed in a token
 */
bool sexp_char_defs::is_token_char(int c)
{
    return ((c >= 0 && c <= 255) && tokenchar[c]);
}

/*
 * sexp_char_defs::is_alpha(c)
 * Returns true if c is alphabetic
 */
bool sexp_char_defs::is_alpha(int c)
{
    return ((c >= 0 && c <= 255) && std::isalpha(c));
}

} // namespace sexp