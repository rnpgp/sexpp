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
 * SEXP implementation code sexp-output.c
 * Ron Rivest
 * 5/5/1997
 */

#include <sexp/sexp-error.h>
#include <sexp/sexp.h>

namespace sexp {

static const char *hexDigits = "0123456789ABCDEF";
static const char *base64Digits =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
 * sexpOutputStream::newSexpOutputStream()
 * Creates and initializes new sexpOutputStream object.
 */
sexpOutputStream::sexpOutputStream(std::ostream *o)
    : outputFile(o), column(0), maxcolumn(defaultlineliength), indent(0), byte_size(8),
      bits(0), n_bits(0), mode(canonical)
{
}

/*
 * sexpOutputStream::putChar(c)
 * Puts the character c out on the output stream os.
 * Keeps track of the "column" the next output char will go to.
 */
sexpOutputStream *sexpOutputStream::putChar(int c)
{
    outputFile->put(c);
    column++;
    return this;
}

/*
 * sexpOutputStream::var_put_char(c)
 * putChar with variable sized output bytes considered.
 * int c;  -- this is always an eight-bit byte being output
 */
sexpOutputStream *sexpOutputStream::var_put_char(int c)
{
    c &= 0xFF;
    bits = (bits << 8) | c;
    n_bits += 8;
    while (n_bits >= byte_size) {
        if ((byte_size == 6 || byte_size == 4 || c == '}' || c == '{' || c == '#' ||
             c == '|') &&
            maxcolumn > 0 && column >= maxcolumn)
            new_line(mode);
        if (byte_size == 4)
            putChar(hexDigits[(bits >> (n_bits - 4)) & 0x0F]);
        else if (byte_size == 6)
            putChar(base64Digits[(bits >> (n_bits - 6)) & 0x3F]);
        else if (byte_size == 8)
            putChar(bits & 0xFF);
        n_bits -= byte_size;
        base64Count++;
    }
    return this;
}

/*
 * sexpOutputStream::change_output_byte_size(newByteSize,newMode)
 * Change os->byte_size to newByteSize
 * record mode in output stream for automatic line breaks
 */
sexpOutputStream *sexpOutputStream::change_output_byte_size(int           newByteSize,
                                                            sexpPrintMode newMode)
{
    if (newByteSize != 4 && newByteSize != 6 && newByteSize != 8)
        sexp_error(sexp_exception::error, "Illegal output base %d", newByteSize, 0, EOF);
    if (newByteSize != 8 && byte_size != 8)
        sexp_error(sexp_exception::error,
                   "Illegal change of output byte size from %d to %d",
                   byte_size,
                   newByteSize,
                   EOF);
    byte_size = newByteSize;
    n_bits = 0;
    bits = 0;
    base64Count = 0;
    mode = newMode;
    return this;
}

/*
 * sexpOutputStream::flush()
 * flush out any remaining bits
 */
sexpOutputStream *sexpOutputStream::flush(void)
{
    if (n_bits > 0) {
        if (byte_size == 4)
            putChar(hexDigits[(bits << (4 - n_bits)) & 0x0F]);
        else if (byte_size == 6)
            putChar(base64Digits[(bits << (6 - n_bits)) & 0x3F]);
        else if (byte_size == 8)
            putChar(bits & 0xFF);
        n_bits = 0;
        base64Count++;
    }
    if (byte_size == 6) { /* and add switch here */
        while ((base64Count & 3) != 0) {
            if (maxcolumn > 0 && column >= maxcolumn)
                new_line(mode);
            putChar('=');
            base64Count++;
        }
    }
    return this;
}

/*
 * sexpOutputStream::new_line(mode)
 * Outputs a newline symbol to the output stream os.
 * For advanced mode, also outputs indentation as one blank per
 * indentation level (but never indents more than half of maxcolumn).
 * Resets column for next output character.
 */
sexpOutputStream *sexpOutputStream::new_line(sexpPrintMode mode)
{
    if (mode == advanced || mode == base64) {
        putChar('\n');
        column = 0;
    }
    if (mode == advanced) {
        for (int i = 0; i < indent && (4 * i) < maxcolumn; i++)
            putChar(' ');
    }
    return this;
}

/*
 * sexpOutputStream::print_decimal(n)
 * print out n in decimal to output stream os
 */
sexpOutputStream *sexpOutputStream::print_decimal(size_t n)
{
    char buffer[50];
    snprintf(buffer,
             sizeof(buffer) / sizeof(buffer[0]),
             "%zu",
             n); // since itoa is not a part of any standard
    for (size_t i = 0; buffer[i] != 0; i++)
        var_put_char(buffer[i]);
    return this;
}

/*
 * base64 MODE
 * Same as canonical, except all characters get put out as base 64 ones
 */

sexpOutputStream *sexpOutputStream::print_base64(const sexpObject *object)
{
    change_output_byte_size(8, base64)->var_put_char('{')->change_output_byte_size(6, base64);
    printCanonical(object);
    return flush()->change_output_byte_size(8, base64)->var_put_char('}');
}
} // namespace sexp
