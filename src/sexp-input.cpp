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

/*
 * newSexpInputStream()
 * Creates and initializes a new sexpInputStream object.
 * (Prefixes stream with one blank, and initializes stream
 *  so that it reads from standard input.)
 */

sexpInputStream::sexpInputStream(std::istream *i)
    : inputFile(i), next_char(' '), count(-1), byte_size(8), bits(0), n_bits(0)
{
}

/*
 * sexpInputStream::setByteSize(newByteSize)
 */
sexpInputStream *sexpInputStream::setByteSize(size_t newByteSize)
{
    byte_size = newByteSize;
    n_bits = 0;
    bits = 0;
    return this;
}

/*
 * sexpInputStream::getChar()
 * This is one possible character input routine for an input stream.
 * (This version uses the standard input stream.)
 * getChar places next 8-bit character into is->next_char.
 * It also updates the count of number of 8-bit characters read.
 * The value EOF is obtained when no more input is available.
 * This code handles 4-bit/6-bit/8-bit channels.
 */
sexpInputStream *sexpInputStream::getChar(void)
{
    int c;
    if (next_char == EOF) {
        byte_size = 8;
        return this;
    }

    while (true) {
        c = next_char = inputFile->get();
        if (c == EOF)
            return this;
        if ((byte_size == 6 && (c == '|' || c == '}')) || (byte_size == 4 && (c == '#'))) {
            // end of region reached; return terminating character, after checking for
            // unused bits
            if (n_bits > 0 && (((1 << n_bits) - 1) & bits) != 0) {
                sexp_error(sexp_exception::warning,
                           "%d-bit region ended with %d unused bits left-over",
                           byte_size,
                           n_bits,
                           count);
            }
            return setByteSize(8);
        } else if (byte_size != 8 && is_white_space(c))
            ; /* ignore white space in hex and base64 regions */
        else if (byte_size == 6 && c == '=')
            ; /* ignore equals signs in base64 regions */
        else if (byte_size == 8) {
            count++;
            return this;
        } else if (byte_size < 8) {
            bits = bits << byte_size;
            n_bits += byte_size;
            if (byte_size == 6 && is_base64_digit(c))
                bits = bits | base64value[c];
            else if (byte_size == 4 && is_hex_digit(c))
                bits = bits | hexvalue[c];
            else {
                sexp_error(sexp_exception::error,
                           "character '%c' found in %d-bit coding region",
                           next_char,
                           byte_size,
                           count);
            }
            if (n_bits >= 8) {
                next_char = (bits >> (n_bits - 8)) & 0xFF;
                n_bits -= 8;
                count++;
                return this;
            }
        }
    }
}

/*
 * sexpInputStream::skipWhiteSpace
 * Skip over any white space on the given sexpInputStream.
 */
sexpInputStream *sexpInputStream::skipWhiteSpace(void)
{
    while (is_white_space(next_char))
        getChar();
    return this;
}

/*
 * sexpInputStream::skipChar(c)
 * Skip the following input character on input stream is, if it is
 * equal to the character c.  If it is not equal, then an error occurs.
 */
sexpInputStream *sexpInputStream::skipChar(int c)
{
    if (next_char != c)
        sexp_error(sexp_exception::error,
                   "character %x (hex) found where %c (char) expected",
                   next_char,
                   c,
                   count);
    return getChar();
}

/*
 * sexpInputStream::scanToken(ss)
 * scan one or more characters into simple string ss as a token.
 */
void sexpInputStream::scanToken(sexpSimpleString *ss)
{
    skipWhiteSpace();
    while (is_token_char(next_char)) {
        ss->append(next_char);
        getChar();
    }
    return;
}

/*
 * sexpInputStream::scanToEOF(void)
 * scan one or more characters (until EOF reached)
 * return an object that is just that string
 */
sexpObject *sexpInputStream::scanToEOF(void)
{
    sexpSimpleString *ss = new sexpSimpleString();
    sexpString *      s = new sexpString();
    s->setString(ss);
    skipWhiteSpace();
    while (next_char != EOF) {
        ss->append(next_char);
        getChar();
    }
    return s;
}

/*
 * scanDecimal(is)
 * returns long integer that is value of decimal number
 */
int sexpInputStream::scanDecimal(void)
{
    int    value = 0;
    size_t i = 0;
    while (is_dec_digit(next_char)) {
        value = value * 10 + decvalue[next_char];
        getChar();
        if (i++ > 8)
            sexp_error(
              sexp_exception::error, "Decimal number %d... too long.", (int) value, 0, count);
    }
    return (value);
}

/*
 * sexpInputStream::scanVerbatimString(is,ss,length)
 * Reads verbatim string of given length into simple string ss.
 */
void sexpInputStream::scanVerbatimString(sexpSimpleString *ss, int length)
{
    skipWhiteSpace()->skipChar(':');
    if (length == -1L) /* no length was specified */
        sexp_error(
          sexp_exception::error, "Verbatim string had no declared length.", 0, 0, count);
    for (int i = 0; i < length; i++) {
        ss->append(next_char);
        getChar();
    }
    return;
}

/*
 * sexpInputStream::scanQuotedString(ss,length)
 * Reads quoted string of given length into simple string ss.
 * Handles ordinary C escapes.
 * If of indefinite length, length is -1.
 */
void sexpInputStream::scanQuotedString(sexpSimpleString *ss, int length)
{
    int c;
    skipChar('"');
    while (length == -1 || ss->length() <= length) {
        if (next_char == '\"') {
            if (length == -1 || (ss->length() == length)) {
                skipChar('\"');
                return;
            } else
                sexp_error(sexp_exception::error,
                           "Quoted string ended too early. Declared length was %d",
                           (int) length,
                           0,
                           count);
        } else if (next_char == '\\') /* handle escape sequence */
        {
            getChar();
            c = next_char;
            if (c == 'b')
                ss->append('\b');
            else if (c == 't')
                ss->append('\t');
            else if (c == 'v')
                ss->append('\v');
            else if (c == 'n')
                ss->append('\n');
            else if (c == 'f')
                ss->append('\f');
            else if (c == 'r')
                ss->append('\r');
            else if (c == '\"')
                ss->append('\"');
            else if (c == '\'')
                ss->append('\'');
            else if (c == '\\')
                ss->append('\\');
            else if (c >= '0' && c <= '7') { /* octal number */
                int j, val;
                val = 0;
                for (j = 0; j < 3; j++) {
                    if (c >= '0' && c <= '7') {
                        val = ((val << 3) | (c - '0'));
                        if (j < 2) {
                            getChar();
                            c = next_char;
                        }
                    } else
                        sexp_error(sexp_exception::error,
                                   "Octal character \\%o... too short",
                                   val,
                                   0,
                                   count);
                }
                if (val > 255)
                    sexp_error(
                      sexp_exception::error, "Octal character \\%o... too big", val, 0, count);
                ss->append(val);
            } else if (c == 'x') /* hexadecimal number */
            {
                int j, val;
                val = 0;
                getChar();
                c = next_char;
                for (j = 0; j < 2; j++) {
                    if (is_hex_digit(c)) {
                        val = ((val << 4) | hexvalue[c]);
                        if (j < 1) {
                            getChar();
                            c = next_char;
                        }
                    } else
                        sexp_error(sexp_exception::error,
                                   "Hex character \\x%x... too short",
                                   val,
                                   0,
                                   count);
                }
                ss->append(val);
            } else if (c == '\n') /* ignore backslash line feed */
            {                     /* also ignore following carriage-return if present */
                getChar();
                if (next_char != '\r')
                    continue;
                ;
            } else if (c == '\r') /* ignore backslash carriage-return */
            {                     /* also ignore following linefeed if present */
                getChar();
                if (next_char != '\n')
                    continue;
            } else
                sexp_error(
                  sexp_exception::warning, "Escape character \\%c... unknown.", c, 0, count);
        } /* end of handling escape sequence */
        else if (next_char == EOF) {
            sexp_error(sexp_exception::error, "unxpected end of file", 0, 0, count);
        } else {
            ss->append(next_char);
        }
        getChar();
    } /* end of main while loop */
    return;
}

/*
 * scanHexString(ss,length)
 * Reads hexadecimal string into simple string ss.
 * String is of given length result, or length = -1 if indefinite length.
 */
void sexpInputStream::scanHexString(sexpSimpleString *ss, int length)
{
    setByteSize(4)->skipChar('#');
    while (next_char != EOF && (next_char != '#' || getByteSize() == 4)) {
        ss->append(next_char);
        getChar();
    }
    skipChar('#');
    if (ss->length() != length && length >= 0)
        sexp_error(sexp_exception::warning,
                   "Hex string has length %d different than declared length %d",
                   ss->length(),
                   length,
                   count);
}

/*
 * sexpInputStream::scanBase64String(ss,length)
 * Reads base64 string into simple string ss.
 * String is of given length result, or length = -1 if indefinite length.
 */
void sexpInputStream::scanBase64String(sexpSimpleString *ss, int length)
{
    setByteSize(6)->skipChar('|');
    while (next_char != EOF && (next_char != '|' || getByteSize() == 6)) {
        ss->append(next_char);
        getChar();
    }
    skipChar('|');
    if (ss->length() != length && length >= 0)
        sexp_error(sexp_exception::warning,
                   "Base64 string has length %d different than declared length %d",
                   ss->length(),
                   length,
                   count);
}

/*
 * sexpInputStream::scanSimpleString(void)
 * Reads and returns a simple string from the input stream.
 * Determines type of simple string from the initial character, and
 * dispatches to appropriate routine based on that.
 */
sexpSimpleString *sexpInputStream::scanSimpleString(void)
{
    int               length;
    sexpSimpleString *ss = new sexpSimpleString;
    skipWhiteSpace();
    /* Note that it is important in the following code to test for token-ness
     * before checking the other cases, so that a token may begin with ":",
     * which would otherwise be treated as a verbatim string missing a length.
     */
    if (is_token_char(next_char) && !is_dec_digit(next_char)) {
        scanToken(ss);
    } else if (is_dec_digit(next_char) || next_char == '\"' || next_char == '#' ||
               next_char == '|' || next_char == ':') {
        if (is_dec_digit(next_char))
            length = scanDecimal();
        else
            length = -1L;
        if (next_char == '\"')
            scanQuotedString(ss, length);
        else if (next_char == '#')
            scanHexString(ss, length);
        else if (next_char == '|')
            scanBase64String(ss, length);
        else if (next_char == ':')
            scanVerbatimString(ss, length);
    } else {
        const char *const msg = (next_char == EOF) ? "unxpected end of file" :
                                isprint(next_char) ? "illegal character '%c' (%d decimal)" :
                                                     "illegal character %d (decimal)";
        sexp_error(sexp_exception::error, msg, next_char, next_char, count);
    }
    if (ss->length() == 0)
        sexp_error(sexp_exception::warning, "Simple string has zero length", 0, 0, count);
    return ss;
}

/*
 * sexpInputStream::scanString(void)
 * Reads and returns a string [presentationhint]string from input stream.
 */
sexpString *sexpInputStream::scanString(void)
{
    sexpString *s = new sexpString();
    if (next_char == '[') { /* scan presentation hint */
        skipChar('[');
        s->setPresentationHint(scanSimpleString());
        skipWhiteSpace()->skipChar(']')->skipWhiteSpace();
    }
    s->setString(scanSimpleString());
    return (s);
}

/*
 * sexpInputStream::scanList(void)
 * Read and return a sexpList from the input stream.
 */
sexpList *sexpInputStream::scanList(void)
{
    sexpList *list = new sexpList();
    skipChar('(')->skipWhiteSpace();
    if (next_char == ')') {
        ; /* OK */
    } else {
        list->push_back(scanObject());
    }

    while (true) {
        skipWhiteSpace();
        if (next_char == ')') { /* we just grabbed last element of list */
            skipChar(')');
            return list;
        } else {
            list->push_back(scanObject());
        }
    }
    return list;
}

/*
 * sexpInputStream::scanObject(void)
 * Reads and returns a sexpObject from the given input stream.
 */
sexpObject *sexpInputStream::scanObject(void)
{
    sexpObject *object;
    skipWhiteSpace();
    if (next_char == '{') {
        setByteSize(6)->skipChar('{');
        object = scanObject();
        skipChar('}');
    } else {
        if (next_char == '(')
            object = scanList();
        else
            object = scanString();
    }
    return object;
}

} // namespace sexp