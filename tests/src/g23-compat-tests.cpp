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
#include <sexp/ext-key-format.h>

using namespace sexp;
using namespace ext_key_format;

using ::testing::UnitTest;

namespace {
class G23CompatTests : public testing::Test {
  protected:
    static void scan_and_check_correct(const char *fn)
    {
        std::ifstream ifs(sexp_samples_folder + "/compat/g23/" + fn, std::ifstream::binary);
        bool          r = ifs.fail();
        EXPECT_FALSE(r);

        if (!ifs.fail()) {
            ext_key_input_stream_t is(&ifs);
            extended_private_key_t extended_key;
            is.scan(extended_key);
            EXPECT_EQ(extended_key.fields.size(), 2);
            EXPECT_EQ(extended_key.fields.count("Created"), 1);
            EXPECT_EQ(extended_key.fields.count("creaTed"), 1);
            EXPECT_EQ(extended_key.fields.count("something"), 0);

            auto search = extended_key.fields.find("Created");
            if (search != extended_key.fields.end()) {
                EXPECT_EQ(search->second, "20221130T160847");
            }
        }
    }
};

TEST_F(G23CompatTests, G10Test)
{
    std::ifstream ifs(sexp_samples_folder + "/compat/g10/canonical.key",
                      std::ifstream::binary);
    bool          r = ifs.fail();
    EXPECT_FALSE(r);

    if (!ifs.fail()) {
        ext_key_input_stream_t is(&ifs);
        extended_private_key_t extended_key;
        is.scan(extended_key);
        EXPECT_EQ(extended_key.fields.size(), 0);
    }
}

// Correct extended key format
TEST_F(G23CompatTests, G23Correct)
{
    scan_and_check_correct("correct.key");
}

// Correct extended key format, no terminating end of line
TEST_F(G23CompatTests, G23CorrectNoEol)
{
    scan_and_check_correct("correct_no_eol.key");
}

// Correct extended key format, with a comment
TEST_F(G23CompatTests, G23CorrectWithComment)
{
    scan_and_check_correct("correct_with_comment.key");
}

// Correct extended key format, with an empty line (which is comment a well)
TEST_F(G23CompatTests, G23CorrectWithTwoEmptyLines)
{
    scan_and_check_correct("correct_with_two_empty_lines.key");
}

// Correct extended key format, with two empty linea
TEST_F(G23CompatTests, G23CorrectWithEmptyLine)
{
    scan_and_check_correct("correct_with_empty_line.key");
}

// Correct extended key format, witg windows line endings
TEST_F(G23CompatTests, G23CorrectWithWindowsEol)
{
    scan_and_check_correct("correct_with_windows_eol.key");
}

// Correct extended key format, with a comment at the end of file
TEST_F(G23CompatTests, G23CorrectWithCommentAtEof)
{
    scan_and_check_correct("correct_with_comment_at_eof.key");
}

// Correct extended key format, with multiple fields of the same name
TEST_F(G23CompatTests, G23CorrectWithMultFields)
{
    std::ifstream ifs(sexp_samples_folder + "/compat/g23/correct_mult_fields.key",
                      std::ifstream::binary);
    bool          r = ifs.fail();
    EXPECT_FALSE(r);

    if (!ifs.fail()) {
        ext_key_input_stream_t is(&ifs);
        extended_private_key_t extended_key;
        extended_key.parse(is);
        EXPECT_EQ(extended_key.fields.size(), 4);
        EXPECT_EQ(extended_key.fields.count("Created"), 1);
        EXPECT_EQ(extended_key.fields.count("Description"), 3);
        EXPECT_EQ(extended_key.fields.count("something"), 0);

        auto search = extended_key.fields.find("Description");
        if (search != extended_key.fields.end()) {
            EXPECT_EQ(search->second, "RSA/RSA");
        }
    }
}

} // namespace
