/**
 *
 * Copyright (c) 2022-2023, [Ribose Inc](https://www.ribose.com).
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
class G10CompatTests : public testing::Test {
};

TEST_F(G10CompatTests, Canonical)
{
    std::string   keyfile(sexp_samples_folder + "/compat/g10/canonical.key");
    std::ifstream ifs(keyfile, std::ifstream::binary);
    EXPECT_FALSE(ifs.fail());

    if (!ifs.fail()) {
        sexp_input_stream_t is(&ifs);
        auto                obj =
          std::unique_ptr<sexp_object_t>(is.set_byte_size(8)->get_char()->scan_object());

        std::ostringstream   oss(std::ios_base::binary);
        sexp_output_stream_t os(&oss);
        os.print_canonical(obj);

        std::istringstream iss(oss.str(), std::ios_base::binary);
        EXPECT_TRUE(compare_binary_files(keyfile, iss));
    }
}

} // namespace
