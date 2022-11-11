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
 */

#include <sexp/sexp-error.h>
#include <sexp/sexp.h>

namespace sexp {

sexp_exception::severity sexp_exception::verbosity_ = sexp_exception::error;
bool                     sexp_exception::interactive_ = false;

std::string sexp_exception::format(std::string message, severity level, size_t position)
{
    std::string r =
      std::string("SEXP ") + (level == error ? "ERROR: " : "WARNING: ") + message;
    if (position >= 0)
        r += " at position " + std::to_string(position);
    return r;
};

void sexp_error(
  sexp_exception::severity level, const char *msg, size_t c1, size_t c2, size_t pos)
{
    char                     tmp[256];
    sexp_exception::severity l = (sexp_exception::severity) level;
    snprintf(tmp, sizeof(tmp) / sizeof(tmp[0]), msg, c1, c2);
    if (sexp_exception::shall_throw(l))
        throw sexp_exception(tmp, l, pos);
    if (sexp_exception::interactive()) {
        std::cout.flush() << std::endl
                          << "*** " << sexp_exception::format(tmp, l, pos) << " ***"
                          << std::endl;
    }
}
} // namespace sexp