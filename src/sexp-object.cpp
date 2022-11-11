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
 * sexpString::printCanonical(os)
 * Prints out sexp string onto output stream os
 */
sexpOutputStream *
sexpString::printCanonical(sexpOutputStream *os) const
{
    sexpSimpleString *ph, *ss;
    ph = getPresentationHint();
    if (presentationHint != NULL) {
        os->varPutChar('[');
        presentationHint->printCanonicalVerbatim(os);
        os->varPutChar(']');
    }
    if (string == NULL)
        ErrorMessage(sexp_exception::error, "NULL string can't be printed", 0, 0, EOF);
    string->printCanonicalVerbatim(os);
    return os;
}

/*
 * sexpString::printAdvanced(os)
 * Prints out sexp string onto output stream os
 */
sexpOutputStream *
sexpString::printAdvanced(sexpOutputStream *os) const
{
    sexpObject::printAdvanced(os);
    sexpSimpleString *ph = getPresentationHint();
    sexpSimpleString *ss = getString();
    if (ph != NULL) {
        os->putChar('[');
        ph->printAdvanced(os);
        os->putChar(']');
    }
    if (ss == NULL)
        ErrorMessage(sexp_exception::error, "NULL string can't be printed", 0, 0, EOF);
    ss->printAdvanced(os);
    return os;
}

/*
 * sexpString::advancedLength(os)
 * Returns length of printed image of string
 */
size_t
sexpString::advancedLength(sexpOutputStream *os) const
{
    size_t len = 0;
    if (presentationHint != NULL)
        len += 2 + presentationHint->advancedLength(os);
    if (string != NULL)
        len += string->advancedLength(os);
    return len;
}

/*
 * sexpList::printCanonical(os)
 * Prints out the list "list" onto output stream os
 */
sexpOutputStream *
sexpList::printCanonical(sexpOutputStream *os) const
{
    sexpObject *object;
    os->varPutChar('(');
    std::for_each(begin(), end(), [os](const sexpObject *object) { object->printCanonical(os); });
    os->varPutChar(')');
    return os;
}

/*
 * sexpList::printAdvanced(os)
 * Prints out the list onto output stream os.
 * Uses print-length to determine length of the image.  If it all fits
 * on the current line, then it is printed that way.  Otherwise, it is
 * written out in "vertical" mode, with items of the list starting in
 * the same column on successive lines.
 */
sexpOutputStream *
sexpList::printAdvanced(sexpOutputStream *os) const
{
    sexpObject::printAdvanced(os);
    int vertical = false;
    int firstelement = true;
    os->putChar('(')->incIndent();
    vertical = (advancedLength(os) > os->getMaxColumn() - os->getColumn());

    std::for_each(begin(), end(), [&](const sexpObject *obj) {
        if (!firstelement) {
            if (vertical)
                os->newLine(sexpOutputStream::advanced);
            else
                os->putChar(' ');
        }
        obj->printAdvanced(os);
        firstelement = false;
    });

    if (os->getMaxColumn() > 0 && os->getColumn() > os->getMaxColumn() - 2)
        os->newLine(sexpOutputStream::advanced);
    return os->decIndent()->putChar(')');
}

/*
 * sexpList::advancedLength(os)
 * Returns length of printed image of list given as iterator
 */
size_t
sexpList::advancedLength(sexpOutputStream *os) const
{
    size_t len = 1; /* for left paren */
    std::for_each(begin(), end(), [&](const sexpObject *obj) { len += obj->advancedLength(os); });
    return (len + 1); /* for final paren */
}

/*
 * sexpObject::printAdvanced(os)
 * Prints out object on output stream os
 */
sexpOutputStream *
sexpObject::printAdvanced(sexpOutputStream *os) const
{
    if (os->getMaxColumn() > 0 && os->getColumn() > os->getMaxColumn() - 4)
        os->newLine(sexpOutputStream::advanced);
    return os;
}

} // namespace sexp