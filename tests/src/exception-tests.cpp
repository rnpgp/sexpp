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

#include <sexp-tests.h>

using namespace sexp;

namespace {
class ExceptionTests : public testing::Test {
  protected:
    static void do_scan_ex(const char *fn, const char *msg)
    {
        std::ifstream ifs(sexp_samples_folder + fn, std::ifstream::binary);
        EXPECT_FALSE(ifs.fail());
        if (!ifs.fail()) {
            try {
                sexp_input_stream_t is(&ifs);
                is.set_byte_size(8)->get_char()->scan_object();
                FAIL() << "sexp::sexp_exception_t expected but has not been thrown";
            } catch (sexp::sexp_exception_t &e) {
                EXPECT_STREQ(e.what(), msg);
            }
        }
    }
};

TEST_F(ExceptionTests, UnexpectedEof)
{
    do_scan_ex("/malformed/sexp-malformed-1",
               "SEXP ERROR: unxpected end of file at position 20");
}

TEST_F(ExceptionTests, UnexpectedCharacter4bit)
{
    do_scan_ex("/malformed/sexp-malformed-2",
               "SEXP ERROR: character ')' found in 4-bit coding region at position 22");
}

TEST_F(ExceptionTests, IllegalCharacter)
{
    do_scan_ex("/malformed/sexp-malformed-3",
               "SEXP ERROR: illegal character '?' (63 decimal) at position 16");
}

TEST_F(ExceptionTests, UnexpectedEofAfterQoute)
{
    do_scan_ex("/malformed/sexp-malformed-4",
               "SEXP ERROR: unxpected end of file at position 4");
}

TEST_F(ExceptionTests, IllegalCharacterBase64)
{
    do_scan_ex("/malformed/sexp-malformed-5",
               "SEXP ERROR: illegal character '}' (125 decimal) at position 35");
}

TEST_F(ExceptionTests, UnusedBits)
{
    sexp::sexp_exception_t::set_verbosity(sexp::sexp_exception_t::warning);
    do_scan_ex("/malformed/sexp-malformed-6",
               "SEXP WARNING: 6-bit region ended with 4 unused bits left-over at position 17");
}

TEST_F(ExceptionTests, NotAListWhenExpected)
{
    std::ifstream ifs(sexp_samples_folder + "/malformed/sexp-not-a-list",
                      std::ifstream::binary);
    EXPECT_FALSE(ifs.fail());
    if (!ifs.fail()) {
        try {
            sexp_list_t         a_list;
            sexp_input_stream_t is(&ifs);
            a_list.parse(is.set_byte_size(8)->get_char());
            FAIL() << "sexp::sexp_exception_t expected but has not been thrown";
        } catch (sexp::sexp_exception_t &e) {
            EXPECT_STREQ(
              e.what(),
              "SEXP ERROR: character '|' found where '(' was expected at position 0");
        }
    }
}

static void do_parse_list_from_string(const char *str)
{
    std::istringstream  iss(str);
    sexp_input_stream_t is(&iss);
    sexp_list_t         lst;
    lst.parse(is.set_byte_size(8)->get_char());
}

static void do_parse_list_from_string_with_limit(const char *str, size_t m_depth)
{
    std::istringstream  iss(str);
    sexp_input_stream_t is(&iss, m_depth);
    sexp_list_t         lst;
    lst.parse(is.set_byte_size(8)->get_char());
}

TEST_F(ExceptionTests, MaxDepth)
{
    const char *depth_1 = "(sexp_list_1)";
    const char *depth_4 = "(sexp_list_1 (sexp_list_2 (sexp_list_3 (sexp_list_4))))";
    const char *depth_4e = "(sexp_list_1 (sexp_list_2 (sexp_list_3 ())))";
    const char *depth_5 =
      "(sexp_list_1 (sexp_list_2 (sexp_list_3 (sexp_list_4 (sexp_list_5)))))";
    const char *depth_5e = "(sexp_list_1 (sexp_list_2 (sexp_list_3 (sexp_list_4 ()))))";

    do_parse_list_from_string(depth_1);
    do_parse_list_from_string(depth_4);
    do_parse_list_from_string(depth_4e);
    do_parse_list_from_string(depth_5);
    do_parse_list_from_string(depth_5e);

    do_parse_list_from_string_with_limit(depth_1, 4);
    do_parse_list_from_string_with_limit(depth_4, 4);
    do_parse_list_from_string_with_limit(depth_4e, 4);

    try {
        do_parse_list_from_string_with_limit(depth_5, 4);
        FAIL() << "sexp::sexp_exception_t expected but has not been thrown";
    } catch (sexp::sexp_exception_t &e) {
        EXPECT_STREQ(
          e.what(),
          "SEXP ERROR: Maximum allowed SEXP list depth (4) is exceeded at position 53");
    }

    try {
        do_parse_list_from_string_with_limit(depth_5e, 4);
        FAIL() << "sexp::sexp_exception_t expected but has not been thrown";
    } catch (sexp::sexp_exception_t &e) {
        EXPECT_STREQ(
          e.what(),
          "SEXP ERROR: Maximum allowed SEXP list depth (4) is exceeded at position 53");
    }
}
} // namespace
