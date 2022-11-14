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

#include <algorithm>
#include <fstream>

#include <sexp-tests.h>

bool compare_binary_files(const std::string &filename1, const std::string &filename2)
{
    bool          res = false;
    std::ifstream file1(filename1, std::ifstream::ate | std::ifstream::binary);
    std::ifstream file2(filename2, std::ifstream::ate | std::ifstream::binary);

    if (file1.tellg() == file2.tellg()) { // otherwise different file size
        file1.seekg(0);
        file2.seekg(0);

        std::istreambuf_iterator<char> begin1(file1);
        std::istreambuf_iterator<char> begin2(file2);

        res = std::equal(begin1, std::istreambuf_iterator<char>(), begin2);
    }

    return res;
}

bool compare_binary_files(const std::string &filename1, std::istream &file2)
{
    std::ifstream file1(filename1, std::ifstream::binary);

    std::istreambuf_iterator<char> begin1(file1);
    std::istreambuf_iterator<char> begin2(file2);
    return std::equal(begin1, std::istreambuf_iterator<char>(), begin2);
}

std::istream &safe_get_line(std::istream &is, std::string &t)
{
    t.clear();

    // The characters in the stream are read one-by-one using a std::streambuf.
    // That is faster than reading them one-by-one using the std::istream.
    // Code that uses streambuf this way must be guarded by a sentry object.
    // The sentry object performs various tasks,
    // such as thread synchronization and updating the stream state.

    std::istream::sentry se(is, true);
    std::streambuf *     sb = is.rdbuf();

    for (;;) {
        int c = sb->sbumpc();
        switch (c) {
        case '\n':
            return is;
        case '\r':
            if (sb->sgetc() == '\n')
                sb->sbumpc();
            return is;
        case std::streambuf::traits_type::eof():
            // Also handle the case when the last line has no line ending
            if (t.empty())
                is.setstate(std::ios::eofbit);
            return is;
        default:
            t += (char) c;
        }
    }
}

bool compare_text_files(const std::string &filename1, const std::string &filename2)
{
    bool          res = true;
    std::ifstream file1(filename1, std::ifstream::binary);
    std::ifstream file2(filename2, std::ifstream::binary);
    std::string   s1, s2;

    while (res) {
        if (file1.eof() && file2.eof())
            break;
        safe_get_line(file1, s1);
        safe_get_line(file2, s2);
        if (s1 != s2)
            res = false;
    }

    return res;
}

bool compare_text_files(const std::string &filename1, std::istream &file2)
{
    bool          res = true;
    std::ifstream file1(filename1, std::ifstream::binary);
    std::string   s1, s2;
    file2.seekg(0);

    while (res) {
        if (file1.eof() && file2.eof())
            break;
        safe_get_line(file1, s1);
        safe_get_line(file2, s2);
        if (s1 != s2)
            res = false;
    }

    return res;
}
