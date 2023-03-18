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
    static void do_scan_with_exception(const char *str_in, const char *msg)
    {
        try {
            std::istringstream  iss(str_in);
            sexp_input_stream_t is(&iss);
            is.set_byte_size(8)->get_char()->scan_object();
            FAIL() << "sexp::sexp_exception_t expected but has not been thrown";
        } catch (sexp::sexp_exception_t &e) {
            EXPECT_STREQ(e.what(), msg);
        }
    }
};

TEST_F(ExceptionTests, UnexpectedEof)
{
    do_scan_with_exception("(4:This2:is1:a4:test",
                           "SEXP ERROR: unexpected end of file at position 20");
}

TEST_F(ExceptionTests, UnexpectedCharacter4bit)
{
    do_scan_with_exception(
      "(4:This2:is1:a4:test #)",
      "SEXP ERROR: character ')' found in 4-bit coding region at position 22");
}

TEST_F(ExceptionTests, IllegalCharacter)
{
    do_scan_with_exception("(This is a test ?)",
                           "SEXP ERROR: illegal character '?' (0x3f) at position 16");
}

TEST_F(ExceptionTests, UnexpectedEofAfterQoute)
{
    do_scan_with_exception("(\")\n", "SEXP ERROR: unexpected end of file at position 4");
}

TEST_F(ExceptionTests, IllegalCharacterBase64)
{
    do_scan_with_exception("(Test {KDQ6VGhpczI6aXMxOmE0OnRlc3Qq})",
                           "SEXP ERROR: illegal character '}' (0x7d) at position 35");
}

TEST_F(ExceptionTests, InvalidHex)
{
    do_scan_with_exception("(\"\\x1U\")",
                           "SEXP ERROR: Hex character \\x1... too short at position 5");
}

TEST_F(ExceptionTests, InvalidOctal)
{
    do_scan_with_exception("(\"\\12U\")",
                           "SEXP ERROR: Octal character \\12... too short at position 5");
}

TEST_F(ExceptionTests, TooBigOctal)
{
    do_scan_with_exception("(\"\\666U\")",
                           "SEXP ERROR: Octal character \\666... too big at position 5");
}

TEST_F(ExceptionTests, InvalidEscape)
{
    do_scan_with_exception("(\"\\?\")",
                           "SEXP ERROR: Unknown escape sequence \\? at position 3");
}

TEST_F(ExceptionTests, StringTooShortQuoted)
{
    do_scan_with_exception(
      "(4\"ABC\")",
      "SEXP ERROR: Declared length was 4, but quoted string ended too early at position 6");
}

TEST_F(ExceptionTests, StringTooShortBase64)
{
    sexp::sexp_exception_t::set_verbosity(sexp::sexp_exception_t::warning);
    do_scan_with_exception("(8|NDpBQkNE|)",
                           "SEXP WARNING: Base64 string has length 6 different than declared "
                           "length 8 at position 12");
    sexp::sexp_exception_t::set_verbosity(sexp::sexp_exception_t::error);
}

TEST_F(ExceptionTests, StringTooShortHex)
{
    sexp::sexp_exception_t::set_verbosity(sexp::sexp_exception_t::warning);
    do_scan_with_exception(
      "(8#AAABFCAD#)",
      "SEXP WARNING: Hex string has length 4 different than declared length 8 at position 12");
    sexp::sexp_exception_t::set_verbosity(sexp::sexp_exception_t::error);
}

TEST_F(ExceptionTests, StringBadLength)
{
    do_scan_with_exception("(1A:AAABFCAD)",
                           "SEXP ERROR: illegal character 'A' (0x41) at position 2");
}

TEST_F(ExceptionTests, DecimalTooLong)
{
    do_scan_with_exception("(1234567890:AAABFCAD)",
                           "SEXP ERROR: Decimal number is too long at position 11");
}

TEST_F(ExceptionTests, UnusedBits)
{
    sexp::sexp_exception_t::set_verbosity(sexp::sexp_exception_t::warning);
    do_scan_with_exception(
      "(Test |AABBCCDD11|)",
      "SEXP WARNING: 6-bit region ended with 4 unused bits left-over at position 17");
    sexp::sexp_exception_t::set_verbosity(sexp::sexp_exception_t::error);
}

TEST_F(ExceptionTests, NotAListWhenExpected)
{
    try {
        std::istringstream iss(
          "|d738/4ghP9rFZ0gAIYZ5q9y6iskDJwASi5rEQpEQq8ZyMZeIZzIAR2I5iGE=|");
        sexp_input_stream_t is(&iss);

        sexp_list_t a_list;
        a_list.parse(is.set_byte_size(8)->get_char());
        FAIL() << "sexp::sexp_exception_t expected but has not been thrown";
    } catch (sexp::sexp_exception_t &e) {
        EXPECT_STREQ(e.what(),
                     "SEXP ERROR: character '|' found where '(' was expected at position 0");
    }
}

TEST_F(ExceptionTests, InvalidByteSizeAndMode)
{
    try {
        std::istringstream             iss("(3:a\011c)");
        sexp_input_stream_t            is(&iss);
        std::unique_ptr<sexp_object_t> obj = std::unique_ptr<sexp_object_t>(is.set_byte_size(8)->get_char()->scan_object());

        std::ostringstream   oss(std::ios_base::binary);
        sexp_output_stream_t os(&oss);
        os.change_output_byte_size(4, sexp_output_stream_t::advanced)->print_advanced(obj);
        FAIL() << "sexp::sexp_exception_t expected but has not been thrown";
    } catch (sexp::sexp_exception_t &e) {
        EXPECT_STREQ(
          e.what(),
          "SEXP ERROR: Can't print in advanced mode with restricted output character set");
    }
}

TEST_F(ExceptionTests, SexpWarning)
{
    testing::internal::CaptureStdout();
    sexp::sexp_exception_t::set_interactive(true);
    sexp_error(sexp_exception_t::warning, "Test warning", 0, 0, 200);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "\n*** SEXP WARNING: Test warning at position 200 ***\n");
    sexp::sexp_exception_t::set_interactive(false);
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
