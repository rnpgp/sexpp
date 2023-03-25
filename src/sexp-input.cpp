/**
 *
 * Copyright (c) 2022-2023, [Ribose Inc](https://www.ribose.com).
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
 * Original copyright
 *
 * SEXP implementation code sexp-input.c
 * Ron Rivest
 * 7/21/1997
 */

#include <sexp/sexp.h>

namespace sexp {

/*
 * newSexpInputStream()
 * Creates and initializes a new sexp_input_stream_t object.
 * (Prefixes stream with one blank, and initializes stream
 *  so that it reads from standard input.)
 */
std::istream *input_file;
uint32_t      byte_size; /* 4 or 6 or 8 == currently scanning mode */
int           next_char; /* character currently being scanned */
uint32_t      bits;      /* Bits waiting to be used */
uint32_t      n_bits;    /* number of such bits waiting to be used */
int           count;     /* number of 8-bit characters output by get_char */

sexp_input_stream_t::sexp_input_stream_t(std::istream *i, size_t m_depth)
{
    set_input(i, m_depth);
}

/*
 * sexp_input_stream_t::set_input(std::istream *i)
 */

sexp_input_stream_t *sexp_input_stream_t::set_input(std::istream *i, size_t m_depth)
{
    input_file = i;
    byte_size = 8;
    next_char = ' ';
    bits = 0;
    n_bits = 0;
    count = -1;
    depth = 0;
    max_depth = m_depth;
    return this;
}

/*
 * sexp_input_stream_t::set_byte_size(newByteSize)
 */
sexp_input_stream_t *sexp_input_stream_t::set_byte_size(uint32_t newByteSize)
{
    byte_size = newByteSize;
    n_bits = 0;
    bits = 0;
    return this;
}

int sexp_input_stream_t::read_char(void)
{
    count++;
    return input_file->get();
}

/*
 * sexp_input_stream_t::get_char()
 * This is one possible character input routine for an input stream.
 * (This version uses the standard input stream.)
 * get_char places next 8-bit character into is->next_char.
 * It also updates the count of number of 8-bit characters read.
 * The value EOF is obtained when no more input is available.
 * This code handles 4-bit/6-bit/8-bit channels.
 */
sexp_input_stream_t *sexp_input_stream_t::get_char(void)
{
    int c;
    if (next_char == EOF) {
        byte_size = 8;
        return this;
    }

    while (true) {
        c = next_char = read_char();
        if (c == EOF)
            return this;
        if ((byte_size == 6 && (c == '|' || c == '}')) || (byte_size == 4 && (c == '#'))) {
            // end of region reached; return terminating character, after checking for
            // unused bits
            if (n_bits > 0 && (((1 << n_bits) - 1) & bits) != 0) {
                sexp_error(sexp_exception_t::warning,
                           "%d-bit region ended with %d unused bits left-over",
                           byte_size,
                           n_bits,
                           count);
            }
            return set_byte_size(8);
        } else if (byte_size != 8 && is_white_space(c))
            ; /* ignore white space in hex and base64 regions */
        else if (byte_size == 6 && c == '=')
            ; /* ignore equals signs in base64 regions */
        else if (byte_size == 8) {
            return this;
        } else if (byte_size < 8) {
            bits = bits << byte_size;
            n_bits += byte_size;
            if (byte_size == 6 && is_base64_digit(c))
                bits = bits | base64value(c);
            else if (byte_size == 4 && is_hex_digit(c))
                bits = bits | hexvalue(c);
            else {
                sexp_error(sexp_exception_t::error,
                           "character '%c' found in %u-bit coding region",
                           next_char,
                           byte_size,
                           count);
            }
            if (n_bits >= 8) {
                next_char = (bits >> (n_bits - 8)) & 0xFF;
                n_bits -= 8;
                return this;
            }
        }
    }
}

/*
 * sexp_input_stream_t::skip_white_space
 * Skip over any white space on the given sexp_input_stream_t.
 */
sexp_input_stream_t *sexp_input_stream_t::skip_white_space(void)
{
    while (is_white_space(next_char))
        get_char();
    return this;
}

/*
 * sexp_input_stream_t::skip_char(c)
 * Skip the following input character on input stream is, if it is
 * equal to the character c.  If it is not equal, then an error occurs.
 */
sexp_input_stream_t *sexp_input_stream_t::skip_char(int c)
{
    if (next_char != c)
        sexp_error(sexp_exception_t::error,
                   "character '%c' found where '%c' was expected",
                   next_char,
                   c,
                   count);
    return get_char();
}

/*
 * sexp_input_stream_t::scan_token(ss)
 * scan one or more characters into simple string ss as a token.
 */
void sexp_input_stream_t::scan_token(sexp_simple_string_t &ss)
{
    skip_white_space();
    while (is_token_char(next_char)) {
        ss.append(next_char);
        get_char();
    }
}

/*
 * sexp_input_stream_t::scan_to_eof(void)
 * scan one or more characters (until EOF reached)
 * return an object that is just that string
 */
std::shared_ptr<sexp_object_t> sexp_input_stream_t::scan_to_eof(void)
{
    sexp_simple_string_t           ss;
    std::shared_ptr<sexp_string_t> s(new sexp_string_t());
    skip_white_space();
    while (next_char != EOF) {
        ss.append(next_char);
        get_char();
    }
    s->set_string(ss);
    return s;
}

/*
 * scan_decimal_string(is)
 * returns long integer that is value of decimal number
 */
uint32_t sexp_input_stream_t::scan_decimal_string(void)
{
    uint32_t value = 0;
    uint32_t i = 0;
    while (is_dec_digit(next_char)) {
        value = value * 10 + decvalue(next_char);
        get_char();
        if (i++ > 8)
            sexp_error(sexp_exception_t::error, "Decimal number is too long", 0, 0, count);
    }
    return value;
}

/*
 * sexp_input_stream_t::scan_verbatim_string(is,ss,length)
 * Reads verbatim string of given length into simple string ss.
 */
void sexp_input_stream_t::scan_verbatim_string(sexp_simple_string_t &ss, uint32_t length)
{
    skip_white_space()->skip_char(':');

    // Some length is specified always, this is ensured by the caller's logic
    assert(length != std::numeric_limits<uint32_t>::max());
    for (uint32_t i = 0; i < length; i++) {
        ss.append(next_char);
        get_char();
    }
}

/*
 * sexp_input_stream_t::scan_quoted_string(ss,length)
 * Reads quoted string of given length into simple string ss.
 * Handles ordinary C escapes.
 * If of indefinite length, length is std::numeric_limits<uint32_t>::max().
 */
void sexp_input_stream_t::scan_quoted_string(sexp_simple_string_t &ss, uint32_t length)
{
    skip_char('"');
    while (ss.length() <= length) {
        if (next_char == '\"') {
            if (length == std::numeric_limits<uint32_t>::max() || (ss.length() == length)) {
                skip_char('\"');
                return;
            } else
                sexp_error(sexp_exception_t::error,
                           "Declared length was %d, but quoted string ended too early",
                           (int) length,
                           0,
                           count);
        } else if (next_char == '\\') /* handle escape sequence */
        {
            get_char();
            switch (next_char) {
            case 'b':
                ss.append('\b');
                break;
            case 't':
                ss.append('\t');
                break;
            case 'v':
                ss.append('\v');
                break;
            case 'n':
                ss.append('\n');
                break;
            case 'f':
                ss.append('\f');
                break;
            case 'r':
                ss.append('\r');
                break;
            case '\"':
                ss.append('\"');
                break;
            case '\'':
                ss.append('\'');
                break;
            case '\\':
                ss.append('\\');
                break;
            case 'x': /* hexadecimal number */
            {
                int j, val;
                val = 0;
                get_char();
                for (j = 0; j < 2; j++) {
                    if (is_hex_digit(next_char)) {
                        val = ((val << 4) | hexvalue(next_char));
                        if (j < 1) {
                            get_char();
                        }
                    } else
                        sexp_error(sexp_exception_t::error,
                                   "Hex character \x5cx%x... too short",
                                   val,
                                   0,
                                   count);
                }
                ss.append(val);
            } break;
            case '\n':      /* ignore backslash line feed */
                get_char(); /* also ignore following carriage-return if present */
                if (next_char != '\r')
                    continue;
                break;
            case '\r':      /* ignore backslash carriage-return */
                get_char(); /* also ignore following linefeed if present */
                if (next_char != '\n')
                    continue;
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7': { /* octal number */
                int j, val;
                val = 0;
                for (j = 0; j < 3; j++) {
                    if (next_char >= '0' && next_char <= '7') {
                        val = ((val << 3) | (next_char - '0'));
                        if (j < 2)
                            get_char();
                    } else
                        sexp_error(sexp_exception_t::error,
                                   "Octal character \\%o... too short",
                                   val,
                                   0,
                                   count);
                }
                if (val > 255)
                    sexp_error(sexp_exception_t::error,
                               "Octal character \\%o... too big",
                               val,
                               0,
                               count);
                ss.append(val);
            } break;
            default:
                sexp_error(sexp_exception_t::error,
                           "Unknown escape sequence \\%c",
                           next_char,
                           0,
                           count);
            }
        } /* end of handling escape sequence */
        else if (next_char == EOF) {
            sexp_error(sexp_exception_t::error, "unexpected end of file", 0, 0, count);
        } else {
            ss.append(next_char);
        }
        get_char();
    } /* end of main while loop */
}

/*
 * scan_hexadecimal_string(ss,length)
 * Reads hexadecimal string into simple string ss.
 * String is of given length result, or length = std::numeric_limits<uint32_t>::max()
 * if indefinite length.
 */
void sexp_input_stream_t::scan_hexadecimal_string(sexp_simple_string_t &ss, uint32_t length)
{
    set_byte_size(4)->skip_char('#');
    while (next_char != EOF && (next_char != '#' || get_byte_size() == 4)) {
        ss.append(next_char);
        get_char();
    }
    skip_char('#');
    if (ss.length() != length && length != std::numeric_limits<uint32_t>::max())
        sexp_error(sexp_exception_t::warning,
                   "Hex string has length %d different than declared length %d",
                   ss.length(),
                   length,
                   count);
}

/*
 * sexp_input_stream_t::scan_base64_string(ss,length)
 * Reads base64 string into simple string ss.
 * String is of given length result, or length = std::numeric_limits<uint32_t>::max()
 * if indefinite length.
 */
void sexp_input_stream_t::scan_base64_string(sexp_simple_string_t &ss, uint32_t length)
{
    set_byte_size(6)->skip_char('|');
    while (next_char != EOF && (next_char != '|' || get_byte_size() == 6)) {
        ss.append(next_char);
        get_char();
    }
    skip_char('|');
    if (ss.length() != length && length != std::numeric_limits<uint32_t>::max())
        sexp_error(sexp_exception_t::warning,
                   "Base64 string has length %d different than declared length %d",
                   ss.length(),
                   length,
                   count);
}

/*
 * sexp_input_stream_t::scan_simple_string(void)
 * Reads and returns a simple string from the input stream.
 * Determines type of simple string from the initial character, and
 * dispatches to appropriate routine based on that.
 */
sexp_simple_string_t sexp_input_stream_t::scan_simple_string(void)
{
    int                  length;
    sexp_simple_string_t ss;
    skip_white_space();
    /* Note that it is important in the following code to test for token-ness
     * before checking the other cases, so that a token may begin with ":",
     * which would otherwise be treated as a verbatim string missing a length.
     */
    if (is_token_char(next_char) && !is_dec_digit(next_char)) {
        scan_token(ss);
    } else {
        length = is_dec_digit(next_char) ? scan_decimal_string() :
                                           std::numeric_limits<uint32_t>::max();

        switch (next_char) {
        case '\"':
            scan_quoted_string(ss, length);
            break;
        case '#':
            scan_hexadecimal_string(ss, length);
            break;
        case '|':
            scan_base64_string(ss, length);
            break;
        case ':':
            // ':' is 'tokenchar', so some length shall be defined
            scan_verbatim_string(ss, length);
            break;
        default: {
            const char *const msg = (next_char == EOF) ? "unexpected end of file" :
                                    isprint(next_char) ? "illegal character '%c' (0x%x)" :
                                                         "illegal character 0x%x";
            sexp_error(sexp_exception_t::error, msg, next_char, next_char, count);
        }
        }
    }

    if (ss.length() == 0)
        sexp_error(sexp_exception_t::warning, "Simple string has zero length", 0, 0, count);
    return ss;
}

/*
 * sexp_input_stream_t::scan_string(void)
 * Reads and returns a string [presentationhint]string from input stream.
 */
std::shared_ptr<sexp_string_t> sexp_input_stream_t::scan_string(void)
{
    std::shared_ptr<sexp_string_t> s(new sexp_string_t());
    s->parse(this);
    return s;
}

/*
 * sexp_input_stream_t::scan_list(void)
 * Read and return a sexp_list_t from the input stream.
 */
std::shared_ptr<sexp_list_t> sexp_input_stream_t::scan_list(void)
{
    std::shared_ptr<sexp_list_t> list(new sexp_list_t());
    list->parse(this);
    return list;
}

/*
 * sexp_input_stream_t::scan_object(void)
 * Reads and returns a sexp_object_t from the given input stream.
 */
std::shared_ptr<sexp_object_t> sexp_input_stream_t::scan_object(void)
{
    std::shared_ptr<sexp_object_t> object;
    skip_white_space();
    if (next_char == '{' && byte_size != 6) {
        set_byte_size(6)->skip_char('{');
        object = scan_object();
        skip_char('}');
    } else {
        if (next_char == '(')
            object = scan_list();
        else
            object = scan_string();
    }
    return object;
}

} // namespace sexp
