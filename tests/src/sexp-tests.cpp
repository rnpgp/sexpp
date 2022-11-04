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

#include <sexp-tests.h>

namespace {
	TEST(LoadTests, smoke) {
		int i = 1;
		EXPECT_EQ(1, i);
	}
}

/*int main(int argc, char** argv) {
    gcry_sexp_t gexp = NULL;
    gcry_error_t err;
    size_t erroff;
    const char *buffer = "(Name Otto Meier (address Burgplatz \"3\"))";
    err = gcry_sexp_build(&gexp, &erroff, buffer);

    printf ("err %x (%s)\n", err, gcry_strerror(err));

    size_t len;
    const char *name;

    len = gcry_sexp_length(gexp);
    printf ("Expression length is %lu\n", len);

    name = gcry_sexp_nth_data (gexp, 2, &len);
    printf ("my name is %.*s\n", (int)len, name);

    if (gexp!=NULL) gcry_sexp_release(gexp);
    return 0;
}
*/
