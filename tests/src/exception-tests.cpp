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

#include <sexp-tests.h>

using ::testing::StrEq;
using ::testing::ThrowsMessage;

using namespace sexp;

namespace {
class ExceptionTests : public testing::Test {
  protected:
    static void
    SetUpTestSuite()
    {
    }

    static void
    TearDownTestSuite()
    {
    }

    static void
    do_scan(std::ifstream &ifs)
    {
        sexpInputStream is(&ifs);
        is.setByteSize(8)->getChar()->scanObject();
    }
};

TEST_F(ExceptionTests, UnexpectedEof)
{
    std::ifstream ifs(sexp_samples_folder + "/malformed/sexp-malformed-1", std::ifstream::binary);
    EXPECT_FALSE(ifs.fail());
    if (!ifs.fail()) {
        EXPECT_THAT(
          [&]() { do_scan(ifs); },
          ThrowsMessage<sexp::sexp_exception>(StrEq("SEXP ERROR: unxpected end of file at position 19")));
    }
}

TEST_F(ExceptionTests, UnexpectedCharacter4bit)
{
    std::ifstream ifs(sexp_samples_folder + "/malformed/sexp-malformed-2", std::ifstream::binary);
    EXPECT_FALSE(ifs.fail());
    if (!ifs.fail()) {
        EXPECT_THAT(
          [&]() { do_scan(ifs); },
          ThrowsMessage<sexp::sexp_exception>(StrEq("SEXP ERROR: character ')' found in 4-bit coding "
                                                    "region at position 21")));
    }
}

TEST_F(ExceptionTests, IllegalCharacter)
{
    std::ifstream ifs(sexp_samples_folder + "/malformed/sexp-malformed-3", std::ifstream::binary);
    EXPECT_FALSE(ifs.fail());
    if (!ifs.fail()) {
        EXPECT_THAT([&]() { do_scan(ifs); },
                    ThrowsMessage<sexp::sexp_exception>(
                      StrEq("SEXP ERROR: illegal character '?' (63 decimal) at position 16")));
    }
}

TEST_F(ExceptionTests, UnexpectedEofAfterQoute)
{
    std::ifstream ifs(sexp_samples_folder + "/malformed/sexp-malformed-4", std::ifstream::binary);
    EXPECT_FALSE(ifs.fail());
    if (!ifs.fail()) {
        EXPECT_THAT(
          [&]() { do_scan(ifs); },
          ThrowsMessage<sexp::sexp_exception>(StrEq("SEXP ERROR: unxpected end of file at position 3")));
    }
}

TEST_F(ExceptionTests, IllegalCharacterBase64)
{
    std::ifstream ifs(sexp_samples_folder + "/malformed/sexp-malformed-5", std::ifstream::binary);
    EXPECT_FALSE(ifs.fail());
    if (!ifs.fail()) {
        EXPECT_THAT([&]() { do_scan(ifs); },
                    ThrowsMessage<sexp::sexp_exception>(
                      StrEq("SEXP ERROR: illegal character '}' (125 decimal) at position 27")));
    }
}

TEST_F(ExceptionTests, UnusedBits)
{
    std::ifstream ifs(sexp_samples_folder + "/malformed/sexp-malformed-6", std::ifstream::binary);
    EXPECT_FALSE(ifs.fail());
    if (!ifs.fail()) {
        sexp::sexp_exception::set_verbosity(sexp::sexp_exception::warning);
        EXPECT_THAT(
          [&]() { do_scan(ifs); },
          ThrowsMessage<sexp::sexp_exception>(StrEq("SEXP WARNING: 6-bit region ended with 4 unused bits "
                                                    "left-over at position 13")));
    }
}

} // namespace
