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
/*
 * sexpSimpleString::printCanonicalVerbatim(os)
 * Print out simple string on output stream os as verbatim string.
 */
sexpOutputStream *
sexpSimpleString::printCanonicalVerbatim(sexpOutputStream *os) const
{
    const octet *c = c_str();
    /* print out len: */
    os->printDecimal(length())->varPutChar(':');
    /* print characters in fragment */
    for (size_t i = 0; i < length(); i++)
        os->varPutChar((int) *c++);
    return os;
}

/*
 * sexpSimpleString::advancedLength(os)
 * Returns length of printed image of s
 */
size_t
sexpSimpleString::advancedLength(sexpOutputStream *os) const
{
    if (canPrintAsToken(os))
        return advancedLengthToken();
    else if (canPrintAsQuotedString())
        return advancedLengthQuotedString();
    else if (length() <= 4 && os->getByteSize() == 8)
        return advancedLengthHexadecimal();
    else if (os->getByteSize() == 8)
        return advancedLengthBase64();
    else
        return 0; /* an error condition */
}

/*
 * sexpSimpleString::printToken(os)
 * Prints out simple string ss as a token (assumes that this is OK).
 * May run over max-column, but there is no fragmentation allowed...
 */
sexpOutputStream *
sexpSimpleString::printToken(sexpOutputStream *os) const
{
    const octet *c = c_str();
    if (os->getMaxColumn() > 0 && os->getColumn() > (os->getMaxColumn() - length()))
        os->newLine(sexpOutputStream::advanced);
    for (size_t i = 0; i < length(); i++)
        os->putChar((int) (*c++));
    return os;
}

/*
 * sexpSimpleString::printVerbatim(os)
 * Print out simple string ss on output stream os as verbatim string.
 * Again, can't fragment string, so max-column is just a suggestion...
 */
sexpOutputStream *
sexpSimpleString::printVerbatim(sexpOutputStream *os) const
{
    const octet *c = c_str();
    if (os->getMaxColumn() > 0 && os->getColumn() > (os->getMaxColumn() - length()))
        os->newLine(sexpOutputStream::advanced)->printDecimal(length())->putChar(':');
    for (size_t i = 0; i < length(); i++)
        os->putChar((int) *c++);
    return os;
}

/*
 * sexpSimpleString::printBase64(os)
 * Prints out simple string ss as a base64 value.
 */
sexpOutputStream *
sexpSimpleString::printBase64(sexpOutputStream *os) const
{
    const octet *c = c_str();
    os->varPutChar('|')->changeOutputByteSize(6, sexpOutputStream::advanced);
    for (size_t i = 0; i < length(); i++)
        os->varPutChar((int) (*c++));
    return os->flushOutput()->changeOutputByteSize(8, sexpOutputStream::advanced)->varPutChar('|');
}

/*
 * sexpSimpleString::printHex(os)
 * Prints out simple string as a hexadecimal value.
 */
sexpOutputStream *
sexpSimpleString::printHex(sexpOutputStream *os) const
{
    const octet *c = c_str();
    os->putChar('#')->changeOutputByteSize(4, sexpOutputStream::advanced);
    for (size_t i = 0; i < length(); i++)
        os->varPutChar((int) (*c++));
    return os->flushOutput()->changeOutputByteSize(8, sexpOutputStream::advanced)->putChar('#');
}

/*
 * sexpSimpleString::printQuoted(os)
 * Prints out simple string ss as a quoted string
 * This code assumes that all characters are tokenchars and blanks,
 *  so no escape sequences need to be generated.
 * May run over max-column, but there is no fragmentation allowed...
 */
sexpOutputStream *
sexpSimpleString::printQuoted(sexpOutputStream *os) const
{
    const octet *c = c_str();
    os->putChar('\"');
    for (size_t i = 0; i < length(); i++) {
        if (os->getMaxColumn() > 0 && os->getColumn() >= os->getMaxColumn() - 2) {
            os->putChar('\\')->putChar('\n');
            os->resetColumn();
        }
        os->putChar(*c++);
    }
    return os->putChar('\"');
}

/*
 * sexpSimpleString::printAdvanced(os)
 * Prints out simple string onto output stream ss
 */
sexpOutputStream *
sexpSimpleString::printAdvanced(sexpOutputStream *os) const
{
    if (canPrintAsToken(os))
        printToken(os);
    else if (canPrintAsQuotedString())
        printQuoted(os);
    else if (length() <= 4 && os->getByteSize() == 8)
        printHex(os);
    else if (os->getByteSize() == 8)
        printBase64(os);
    else
        ErrorMessage(
          sexp_exception::error, "Can't print advanced mode with restricted output character set", 0, 0, EOF);
    return os;
}

/*
 * sexpSimpleString::canPrintAsQuotedString(void)
 * Returns TRUE if simple string can be printed as a quoted string.
 * Must have only tokenchars and blanks.
 */
bool
sexpSimpleString::canPrintAsQuotedString(void) const
{
    const octet *c = c_str();
    if (length() < 0)
        return false;
    for (size_t i = 0; i < length(); i++, c++) {
        if (!isTokenChar((int) (*c)) && *c != ' ')
            return false;
    }
    return true;
}

/*
 * sexpSimpleString::canPrintAsToken(os)
 * Returns TRUE if simple string can be printed as a token.
 * Doesn't begin with a digit, and all characters are tokenchars.
 */
bool
sexpSimpleString::canPrintAsToken(const sexpOutputStream *os) const
{
    const octet *c = c_str();
    if (length() <= 0)
        return false;
    if (isDecDigit((int) *c))
        return false;
    if (os->getMaxColumn() > 0 && os->getColumn() + length() >= os->getMaxColumn())
        return false;
    for (size_t i = 0; i < length(); i++) {
        if (!isTokenChar((int) (*c++)))
            return false;
    }
    return true;
}

} // namespace sexp
