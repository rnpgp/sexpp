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

using namespace sexp;

namespace {
class PrimitivesTests : public testing::Test {
  protected:
    static void do_test_advanced(const char *str_in, const char *str_out = nullptr)
    {
        std::istringstream iss(str_in);
        sexpInputStream    is(&iss);
        sexpObject *       obj = is.set_byte_size(8)->get_char()->scan_object();

        std::ostringstream oss(std::ios_base::binary);
        sexpOutputStream   os(&oss);
        os.print_advanced(obj);
        const char *sample = str_out == nullptr ? str_in : str_out;
        EXPECT_EQ(oss.str(), sample);
    }

    static void do_test_canonical(const char *str_in, const char *str_out = nullptr)
    {
        std::istringstream iss(str_in);
        sexpInputStream    is(&iss);
        sexpObject *       obj = is.set_byte_size(8)->get_char()->scan_object();

        std::ostringstream oss(std::ios_base::binary);
        sexpOutputStream   os(&oss);
        os.print_canonical(obj);
        const char *sample = str_out == nullptr ? str_in : str_out;
        EXPECT_EQ(oss.str(), sample);
    }
};

TEST_F(PrimitivesTests, EmptyList)
{
    do_test_canonical("( )", "()");
    do_test_advanced("( )", "()");
}

TEST_F(PrimitivesTests, EmptyString)
{
    sexp::sexp_exception::set_verbosity(sexp::sexp_exception::error);
    do_test_canonical("(\"\")", "(0:)");
    do_test_advanced("(\"\")", "(\"\")");
}

TEST_F(PrimitivesTests, String)
{
    sexp::sexp_exception::set_verbosity(sexp::sexp_exception::error);
    do_test_canonical("(ab)", "(2:ab)");
    do_test_advanced("(ab)", "(ab)");
}

TEST_F(PrimitivesTests, QuotedStringWithOctal)
{
    sexp::sexp_exception::set_verbosity(sexp::sexp_exception::error);
    do_test_canonical("\"ab\\015\"", "3:ab\r");
    do_test_advanced("\"ab\\015\"", "#61620D#");
}

TEST_F(PrimitivesTests, QuotedStringWithEscape)
{
    sexp::sexp_exception::set_verbosity(sexp::sexp_exception::error);
    do_test_canonical("\"ab\\tc\"", "4:ab\tc");
    do_test_advanced("4:ab\tc", "#61620963#");
}

TEST_F(PrimitivesTests, HexString)
{
    sexp::sexp_exception::set_verbosity(sexp::sexp_exception::error);
    do_test_canonical("#616263#", "3:abc");
    do_test_advanced("#616263#", "abc");
}

TEST_F(PrimitivesTests, ListList)
{
    do_test_canonical("(string-level-1 (string-level-2) )",
                      "(14:string-level-1(14:string-level-2))");
    do_test_advanced("(string-level-1 (string-level-2) )",
                     "(string-level-1 (string-level-2))");
}

TEST_F(PrimitivesTests, Base64OfOctet)
{
    do_test_canonical("|YWJj|", "3:abc");
    do_test_advanced("|YWJj|", "abc");
}

TEST_F(PrimitivesTests, Base64OfVerbatium)
{
    do_test_canonical("{MzphYmM=}", "3:abc");
    do_test_advanced("{MzphYmM=}", "abc");
}

TEST_F(PrimitivesTests, MultilineLinux)
{
    do_test_canonical("\"abcd\\\nef\"", "6:abcdef");
    do_test_advanced("\"abcd\\\nef\"", "abcdef");
}

TEST_F(PrimitivesTests, MultilineMac)
{
    do_test_canonical("\"abcd\\\ref\"", "6:abcdef");
    do_test_advanced("\"abcd\\\ref\"", "abcdef");
}

TEST_F(PrimitivesTests, MultilineWin)
{
    do_test_canonical("\"abcd\\\r\nef\"", "6:abcdef");
    do_test_advanced("\"abcd\\\r\nef\"", "abcdef");
}

TEST_F(PrimitivesTests, MultilineBsd)
{
    do_test_canonical("\"abcd\\\n\ref\"", "6:abcdef");
    do_test_advanced("\"abcd\\\n\ref\"", "abcdef");
}

TEST_F(PrimitivesTests, Wrap)
{
    const char *reallyLong = "(a (b (c ddddddddddddddd"
                             "ddddddddddddddddddddddddddddddddddddddddddddddddd"
                             "ddddddddddddddddddddddddddddddddddddddddddddddddd"
                             "ddddddddddddddddddddddddddddddddddddddddddddddddd"
                             "ddddddd)))";
    const char *stillLong = "(1:a(1:b(1:c169:ddddddddd"
                            "ddddddddddddddddddddddddddddddddddddddddddddddddd"
                            "ddddddddddddddddddddddddddddddddddddddddddddddddd"
                            "ddddddddddddddddddddddddddddddddddddddddddddddddd"
                            "ddddddddddddd)))";
    const char *broken = "(a\n"
                         " (b\n"
                         "  (c\n"
                         "   "
                         "\"ddddddddddddddddddddddddddddddddddddddddddddddddddddd"
                         "dddddddddddddddd\\\n"
                         "ddddddddddddddddddddddddddddddddddddddddddddddddddddddd"
                         "dddddddddddddddddd\\\n"
                         "ddddddddddddddddddddddddddd\")))";
    do_test_canonical(reallyLong, stillLong);
    do_test_advanced(reallyLong, broken);
}
} // namespace
