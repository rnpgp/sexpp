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
 *    documentation and/or other materials provided with the distribution.
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
 */

#include <sexp/ext-key-format.h>

using namespace sexp;

namespace ext_key_format {

void ext_key_error(
  sexp_exception_t::severity level, const char *msg, size_t c1, size_t c2, int pos)
{
    char                       tmp[256];
    sexp_exception_t::severity l = (sexp_exception_t::severity) level;
    snprintf(tmp, sizeof(tmp) / sizeof(tmp[0]), msg, c1, c2);
    if (sexp_exception_t::shall_throw(l))
        throw sexp_exception_t(tmp, l, pos, "EXTENDED KEY FORMAT");
    if (sexp_exception_t::interactive()) {
        std::cout.flush() << std::endl
                          << "*** "
                          << sexp_exception_t::format("EXTENDED KEY FORMAT", tmp, l, pos)
                          << " ***" << std::endl;
    }
}

bool ext_key_input_stream_t::initialized = false;
// Valid characters are all ASCII letters, numbers and the hyphen.
// true if allowed in the name field
bool ext_key_input_stream_t::namechar[256];

/*
 * ext_key_input_stream_t::initializeCharacterTables
 */
ext_key_input_stream_t::ext_key_input_stream_t(std::istream *i, size_t md)
    : sexp_input_stream_t(i, md), is_scanning_value(false), has_key(false)
{
    if (!initialized) {
        int i;
        for (i = 0; i < 256; i++) {
            namechar[i] = false;
        }
        for (i = 'a'; i <= 'z'; i++) {
            namechar[i] = namechar[upper[i]] = true;
        }
        for (i = '0'; i <= '9'; i++) {
            namechar[i] = true;
        }
        namechar[(int) '-'] = true;
        initialized = true;
    }
}

/*
 * ext_key_input_stream_t::skip_line
 */
int ext_key_input_stream_t::skip_line(void)
{
    int c;
    do {
        c = input_file->get();
    } while (!is_newline_char(c) && c != EOF);
    return c;
}

/*
 * ext_key_input_stream_t::read_char
 */
int ext_key_input_stream_t::read_char(void)
{
    int lookahead_1 = input_file->get();
    count++;
    if (is_scanning_value && is_newline_char(lookahead_1)) {
        while (true) {
            int lookahead_2 = input_file->peek();
            if (lookahead_1 == '\r' && lookahead_2 == '\n') {
                lookahead_1 = input_file->get();
                count++;
                lookahead_2 = input_file->peek();
            }
            if (lookahead_2 == ' ') {
                input_file->get();
                count++;
                lookahead_2 = input_file->peek();
                if (lookahead_2 == '#') {
                    lookahead_1 = skip_line();
                    continue;
                }
                if (is_newline_char(lookahead_2)) {
                    lookahead_1 = lookahead_2;
                    continue;
                }
                lookahead_1 = input_file->get();
                count++;
            }
            return lookahead_1;
        }
    }
    return lookahead_1;
}

/*
 * ext_key_input_stream_t::scan_name
 * A name must start with a letter and end with a colon. Valid characters are all ASCII
 * letters, numbers and the hyphen. Comparison of names is done case insensitively. Names may
 * be used several times to represent an array of values. Note that the name “Key” is special
 * in that it is madandory must occur only once.
 */

std::string ext_key_input_stream_t::scan_name(int c)
{
    std::string name;
    if (!is_alpha(c)) {
        ext_key_error(sexp_exception_t::error,
                      isprint(next_char) ?
                        "unexpected character '%c' (0x%x) found starting a name field" :
                        "unexpected character '0x%x' found starting a name field",
                      c,
                      c,
                      count);
    } else {
        name += (char) c;
        c = read_char();
        while (c != ':') {
            if (c == EOF) {
                ext_key_error(sexp_exception_t::error, "unexpected end of file", 0, 0, count);
            }
            if (is_newline_char(c)) {
                ext_key_error(sexp_exception_t::error, "unexpected end of line", 0, 0, count);
            }
            if (!is_namechar(c)) {
                ext_key_error(sexp_exception_t::error,
                              isprint(next_char) ?
                                "unexpected character '%c' (0x%x) found in a name field" :
                                "unexpected character '0x%x' found in a name field",
                              c,
                              c,
                              count);
            }
            name += (int) c;
            c = read_char();
        }
    }
    return name;
}

/*
 * ext_key_input_stream_t::scan_value
 * Values are UTF-8 encoded strings. Values can be wrapped at any point, and continued in
 * the next line indicated by leading whitespace. A continuation line with one leading space
 * does not introduce a blank so that the lines can be effectively concatenated. A blank
 * line as part of a continuation line encodes a newline.
 */
std::string ext_key_input_stream_t::scan_value(void)
{
    std::string value;
    int         c;
    do {
        c = read_char();
    } while (is_white_space(c));
    while (c != EOF && !is_newline_char(c)) {
        value += c;
        c = read_char();
    }
    return value;
}

/*
 * ext_key_input_stream_t::scan
 * GnuPG 2.3+ uses a new format to store private keys that is both more flexible and easier to
 * read and edit by human beings. The new format stores name, value-pairs using the common mail
 * and http header convention.
 */
void ext_key_input_stream_t::scan(extended_private_key_t &res)
{
    set_byte_size(8);
    int c = read_char();
    if (c == '(') {
        set_next_char(c);
        res.key.parse(this);
        has_key = true;
    } else {
        while (c != EOF) {
            // Comparison of names is done case insensitively
            std::string name = scan_name(c);
            // The name “Key” is special in that it is mandatory and must occur only once.
            // The associated value holds the actual S-expression with the cryptographic key.
            // The S-expression is formatted using the ‘Advanced Format’
            // (GCRYSEXP_FMT_ADVANCED) that avoids non-printable characters so that the file
            // can be easily inspected and edited.
            is_scanning_value = true;
            if (extended_private_key_t::iequals(name, "key")) {
                if (has_key) {
                    ext_key_error(sexp_exception_t::error,
                                  "'key' field must occur only once",
                                  0,
                                  0,
                                  count);
                }
                do {
                    c = read_char();
                } while (is_white_space(c));
                set_next_char(c);
                res.key.parse(this);
                has_key = true;
            } else {
                std::string value = scan_value();
                res.fields.insert(std::pair<std::string, std::string>{name, value});
            }
            c = read_char();
            is_scanning_value = false;
        }
    }
    if (!has_key) {
        ext_key_error(sexp_exception_t::error, "missing mandatory 'key' field", 0, 0, count);
    }
}

/*
 * extended_private_key_t::parse
 */
void extended_private_key_t::parse(ext_key_input_stream_t &is)
{
    is.scan(*this);
}

} // namespace ext_key_format