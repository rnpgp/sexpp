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
class BaselineTests : public testing::Test {
protected:
  static const size_t base_sample_advanced = 0;
  static const size_t base_sample_base64 = 1;
  static const size_t base_sample_canonical = 2;
  static const size_t n_base_samples = base_sample_canonical + 1;

  static std::string base_samples[n_base_samples];

  static void SetUpTestSuite() {
    base_samples[base_sample_advanced] = sexp_samples_folder + "/sexp-sample-a";
    base_samples[base_sample_base64] = sexp_samples_folder + "/sexp-sample-b";
    base_samples[base_sample_canonical] = sexp_samples_folder + "/sexp-sample-c";
  };

  static void TearDownTestSuite() {

  }
};

const size_t BaselineTests::n_base_samples;
const size_t BaselineTests::base_sample_advanced;
const size_t BaselineTests::base_sample_base64;
const size_t BaselineTests::base_sample_canonical;
std::string BaselineTests::base_samples[n_base_samples];

TEST_F(BaselineTests, Scan2Canonical) {
  for (size_t i = 0; i < n_base_samples; i++) {
    std::ifstream ifs(base_samples[i], std::ifstream::binary);
    EXPECT_FALSE(ifs.fail());

    if (!ifs.fail()) {
      sexpInputStream is(&ifs);
      sexpObject *obj = is.setByteSize(8)->getChar()->scanObject();

      char fn[L_tmpnam];
      std::tmpnam(fn);
      std::ofstream ofs(fn, std::iostream::binary);
      EXPECT_FALSE(ofs.fail());

      if (!ofs.fail()) {
        sexpOutputStream os(&ofs);
        os.printCanonical(obj);
        ofs.close();
        EXPECT_TRUE(compare_binary_files(base_samples[base_sample_canonical], fn));
      }
      unlink(fn);
    }
  }
}

TEST_F(BaselineTests, Scan2Base64) {
  for (size_t i = 0; i < n_base_samples; i++) {
    std::ifstream ifs(base_samples[i], std::ifstream::binary);
    EXPECT_FALSE(ifs.fail());

    if (!ifs.fail()) {
      sexpInputStream is(&ifs);
      sexpObject *object = is.setByteSize(8)->getChar()->scanObject();

      char fn[L_tmpnam];
      std::tmpnam(fn);
      std::ofstream ofs(fn, std::iostream::binary);
      EXPECT_FALSE(ofs.fail());

      if (!ofs.fail()) {
        sexpOutputStream os(&ofs);
        os.setMaxColumn(0);
        os.printBase64(object);
        ofs << std::endl;
        ofs.close();
        EXPECT_TRUE(compare_text_files(base_samples[base_sample_base64], fn));
      }
      unlink(fn);
    }
  }
}

TEST_F(BaselineTests, Scan2Advanced) {
  for (size_t i = 0; i < n_base_samples; i++) {
    std::ifstream ifs(base_samples[i], std::ifstream::binary);
    EXPECT_FALSE(ifs.fail());

    if (!ifs.fail()) {
      sexpInputStream is(&ifs);
      sexpObject *object = is.setByteSize(8)->getChar()->scanObject();

      char fn[L_tmpnam];
      std::tmpnam(fn);
      std::ofstream ofs(fn, std::iostream::binary);
      EXPECT_FALSE(ofs.fail());

      if (!ofs.fail()) {
        sexpOutputStream os(&ofs);
        os.printAdvanced(object);
        ofs << std::endl;
        ofs.close();
        EXPECT_TRUE(compare_text_files(base_samples[base_sample_advanced], fn));
      }
      unlink(fn);
    }
  }
}
} // namespace
