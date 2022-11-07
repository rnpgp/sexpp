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
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <exception>
#include <iostream>
#include <string>

namespace sexp {

class sexp_exception : public std::exception {
  public:
    enum severity {
        error = 0,
        warning = 1
    };
  protected:
    static severity verbosity_;
    static bool interactive_;

    int position_;           // May be EOF aka -1
    std::string msg_;
    severity level_;

  public:
    sexp_exception(std::string message, severity level, int position):
        position_{position},
        level_{level},
        msg_{format(message, level, position)} {
    };
    static std::string format(std::string message, severity level, int position);
    static bool shall_throw(severity level) {
        return level == error || verbosity_ != error;
    };
    virtual const char * what() const throw() {
        return msg_.c_str();
    };
    int level() const {
        return level_;
    };
    int position() const {
        return position_;
    };
    static severity verbosity() {
        return verbosity_;
    };
    static bool interactive() {
        return interactive_;
    };
    static void set_verbosity(severity vrb) {
        verbosity_ = vrb;
    };
    static void set_interactive(bool intr) {
        interactive_ = intr;
    };
};

void ErrorMessage(int level, const char *msg, int c1, int c2, int pos);

} // namespace sexp
