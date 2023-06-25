/**
 *
 * Copyright 2021-2023 Ribose Inc. (https://www.ribose.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Original copyright
 *
 * SEXP implementation code sexp-input.c
 * Ron Rivest
 * 7/21/1997
 */

#include "sexpp/sexp.h"

namespace sexp {

/**************************************/
/* CHARACTER ROUTINES AND DEFINITIONS */
/**************************************/
std::locale sexp_char_defs_t::c_locale{"C"};

const unsigned char sexp_char_defs_t::values[256][3] =
  {/* values of c as { dec. hex, base64 } digit */
   {/* 0x00   */ 0x00, 0x00, 0x00}, {/* 0x01   */ 0x00, 0x00, 0x00},
   {/* 0x02   */ 0x00, 0x00, 0x00}, {/* 0x03   */ 0x00, 0x00, 0x00},
   {/* 0x04   */ 0x00, 0x00, 0x00}, {/* 0x05   */ 0x00, 0x00, 0x00},
   {/* 0x06   */ 0x00, 0x00, 0x00}, {/* 0x07   */ 0x00, 0x00, 0x00},
   {/* 0x08   */ 0x00, 0x00, 0x00}, {/* 0x09   */ 0x00, 0x00, 0x00},
   {/* 0x0a   */ 0x00, 0x00, 0x00}, {/* 0x0b   */ 0x00, 0x00, 0x00},
   {/* 0x0c   */ 0x00, 0x00, 0x00}, {/* 0x0d   */ 0x00, 0x00, 0x00},
   {/* 0x0e   */ 0x00, 0x00, 0x00}, {/* 0x0f   */ 0x00, 0x00, 0x00},
   {/* 0x10   */ 0x00, 0x00, 0x00}, {/* 0x11   */ 0x00, 0x00, 0x00},
   {/* 0x12   */ 0x00, 0x00, 0x00}, {/* 0x13   */ 0x00, 0x00, 0x00},
   {/* 0x14   */ 0x00, 0x00, 0x00}, {/* 0x15   */ 0x00, 0x00, 0x00},
   {/* 0x16   */ 0x00, 0x00, 0x00}, {/* 0x17   */ 0x00, 0x00, 0x00},
   {/* 0x18   */ 0x00, 0x00, 0x00}, {/* 0x19   */ 0x00, 0x00, 0x00},
   {/* 0x1a   */ 0x00, 0x00, 0x00}, {/* 0x1b   */ 0x00, 0x00, 0x00},
   {/* 0x1c   */ 0x00, 0x00, 0x00}, {/* 0x1d   */ 0x00, 0x00, 0x00},
   {/* 0x1e   */ 0x00, 0x00, 0x00}, {/* 0x1f   */ 0x00, 0x00, 0x00},
   {/* 0x20   */ 0x00, 0x00, 0x00}, {/* 0x21 ! */ 0x00, 0x00, 0x00},
   {/* 0x22 " */ 0x00, 0x00, 0x00}, {/* 0x23 # */ 0x00, 0x00, 0x00},
   {/* 0x24 $ */ 0x00, 0x00, 0x00}, {/* 0x25 % */ 0x00, 0x00, 0x00},
   {/* 0x26 & */ 0x00, 0x00, 0x00}, {/* 0x27 ' */ 0x00, 0x00, 0x00},
   {/* 0x28 ( */ 0x00, 0x00, 0x00}, {/* 0x29 ) */ 0x00, 0x00, 0x00},
   {/* 0x2a * */ 0x00, 0x00, 0x00}, {/* 0x2b + */ 0x00, 0x00, 0x3e},
   {/* 0x2c , */ 0x00, 0x00, 0x00}, {/* 0x2d - */ 0x00, 0x00, 0x00},
   {/* 0x2e . */ 0x00, 0x00, 0x00}, {/* 0x2f / */ 0x00, 0x00, 0x3f},
   {/* 0x30 0 */ 0x00, 0x00, 0x34}, {/* 0x31 1 */ 0x01, 0x01, 0x35},
   {/* 0x32 2 */ 0x02, 0x02, 0x36}, {/* 0x33 3 */ 0x03, 0x03, 0x37},
   {/* 0x34 4 */ 0x04, 0x04, 0x38}, {/* 0x35 5 */ 0x05, 0x05, 0x39},
   {/* 0x36 6 */ 0x06, 0x06, 0x3a}, {/* 0x37 7 */ 0x07, 0x07, 0x3b},
   {/* 0x38 8 */ 0x08, 0x08, 0x3c}, {/* 0x39 9 */ 0x09, 0x09, 0x3d},
   {/* 0x3a : */ 0x00, 0x00, 0x00}, {/* 0x3b ; */ 0x00, 0x00, 0x00},
   {/* 0x3c < */ 0x00, 0x00, 0x00}, {/* 0x3d = */ 0x00, 0x00, 0x00},
   {/* 0x3e > */ 0x00, 0x00, 0x00}, {/* 0x3f ? */ 0x00, 0x00, 0x00},
   {/* 0x40 @ */ 0x00, 0x00, 0x00}, {/* 0x41 A */ 0x00, 0x0a, 0x00},
   {/* 0x42 B */ 0x00, 0x0b, 0x01}, {/* 0x43 C */ 0x00, 0x0c, 0x02},
   {/* 0x44 D */ 0x00, 0x0d, 0x03}, {/* 0x45 E */ 0x00, 0x0e, 0x04},
   {/* 0x46 F */ 0x00, 0x0f, 0x05}, {/* 0x47 G */ 0x00, 0x00, 0x06},
   {/* 0x48 H */ 0x00, 0x00, 0x07}, {/* 0x49 I */ 0x00, 0x00, 0x08},
   {/* 0x4a J */ 0x00, 0x00, 0x09}, {/* 0x4b K */ 0x00, 0x00, 0x0a},
   {/* 0x4c L */ 0x00, 0x00, 0x0b}, {/* 0x4d M */ 0x00, 0x00, 0x0c},
   {/* 0x4e N */ 0x00, 0x00, 0x0d}, {/* 0x4f O */ 0x00, 0x00, 0x0e},
   {/* 0x50 P */ 0x00, 0x00, 0x0f}, {/* 0x51 Q */ 0x00, 0x00, 0x10},
   {/* 0x52 R */ 0x00, 0x00, 0x11}, {/* 0x53 S */ 0x00, 0x00, 0x12},
   {/* 0x54 T */ 0x00, 0x00, 0x13}, {/* 0x55 U */ 0x00, 0x00, 0x14},
   {/* 0x56 V */ 0x00, 0x00, 0x15}, {/* 0x57 W */ 0x00, 0x00, 0x16},
   {/* 0x58 X */ 0x00, 0x00, 0x17}, {/* 0x59 Y */ 0x00, 0x00, 0x18},
   {/* 0x5a Z */ 0x00, 0x00, 0x19}, {/* 0x5b [ */ 0x00, 0x00, 0x00},
   {/* 0x5c \ */ 0x00, 0x00, 0x00}, {/* 0x5d ] */ 0x00, 0x00, 0x00},
   {/* 0x5e ^ */ 0x00, 0x00, 0x00}, {/* 0x5f _ */ 0x00, 0x00, 0x00},
   {/* 0x60 ` */ 0x00, 0x00, 0x00}, {/* 0x61 a */ 0x00, 0x0a, 0x1a},
   {/* 0x62 b */ 0x00, 0x0b, 0x1b}, {/* 0x63 c */ 0x00, 0x0c, 0x1c},
   {/* 0x64 d */ 0x00, 0x0d, 0x1d}, {/* 0x65 e */ 0x00, 0x0e, 0x1e},
   {/* 0x66 f */ 0x00, 0x0f, 0x1f}, {/* 0x67 g */ 0x00, 0x00, 0x20},
   {/* 0x68 h */ 0x00, 0x00, 0x21}, {/* 0x69 i */ 0x00, 0x00, 0x22},
   {/* 0x6a j */ 0x00, 0x00, 0x23}, {/* 0x6b k */ 0x00, 0x00, 0x24},
   {/* 0x6c l */ 0x00, 0x00, 0x25}, {/* 0x6d m */ 0x00, 0x00, 0x26},
   {/* 0x6e n */ 0x00, 0x00, 0x27}, {/* 0x6f o */ 0x00, 0x00, 0x28},
   {/* 0x70 p */ 0x00, 0x00, 0x29}, {/* 0x71 q */ 0x00, 0x00, 0x2a},
   {/* 0x72 r */ 0x00, 0x00, 0x2b}, {/* 0x73 s */ 0x00, 0x00, 0x2c},
   {/* 0x74 t */ 0x00, 0x00, 0x2d}, {/* 0x75 u */ 0x00, 0x00, 0x2e},
   {/* 0x76 v */ 0x00, 0x00, 0x2f}, {/* 0x77 w */ 0x00, 0x00, 0x30},
   {/* 0x78 x */ 0x00, 0x00, 0x31}, {/* 0x79 y */ 0x00, 0x00, 0x32},
   {/* 0x7a z */ 0x00, 0x00, 0x33}, {/* 0x7b { */ 0x00, 0x00, 0x00},
   {/* 0x7c | */ 0x00, 0x00, 0x00}, {/* 0x7d } */ 0x00, 0x00, 0x00},
   {/* 0x7e ~ */ 0x00, 0x00, 0x00}, {/* 0x7f   */ 0x00, 0x00, 0x00},
   {/* 0x80   */ 0x00, 0x00, 0x00}, {/* 0x81   */ 0x00, 0x00, 0x00},
   {/* 0x82   */ 0x00, 0x00, 0x00}, {/* 0x83   */ 0x00, 0x00, 0x00},
   {/* 0x84   */ 0x00, 0x00, 0x00}, {/* 0x85   */ 0x00, 0x00, 0x00},
   {/* 0x86   */ 0x00, 0x00, 0x00}, {/* 0x87   */ 0x00, 0x00, 0x00},
   {/* 0x88   */ 0x00, 0x00, 0x00}, {/* 0x89   */ 0x00, 0x00, 0x00},
   {/* 0x8a   */ 0x00, 0x00, 0x00}, {/* 0x8b   */ 0x00, 0x00, 0x00},
   {/* 0x8c   */ 0x00, 0x00, 0x00}, {/* 0x8d   */ 0x00, 0x00, 0x00},
   {/* 0x8e   */ 0x00, 0x00, 0x00}, {/* 0x8f   */ 0x00, 0x00, 0x00},
   {/* 0x90   */ 0x00, 0x00, 0x00}, {/* 0x91   */ 0x00, 0x00, 0x00},
   {/* 0x92   */ 0x00, 0x00, 0x00}, {/* 0x93   */ 0x00, 0x00, 0x00},
   {/* 0x94   */ 0x00, 0x00, 0x00}, {/* 0x95   */ 0x00, 0x00, 0x00},
   {/* 0x96   */ 0x00, 0x00, 0x00}, {/* 0x97   */ 0x00, 0x00, 0x00},
   {/* 0x98   */ 0x00, 0x00, 0x00}, {/* 0x99   */ 0x00, 0x00, 0x00},
   {/* 0x9a   */ 0x00, 0x00, 0x00}, {/* 0x9b   */ 0x00, 0x00, 0x00},
   {/* 0x9c   */ 0x00, 0x00, 0x00}, {/* 0x9d   */ 0x00, 0x00, 0x00},
   {/* 0x9e   */ 0x00, 0x00, 0x00}, {/* 0x9f   */ 0x00, 0x00, 0x00},
   {/* 0xa0   */ 0x00, 0x00, 0x00}, {/* 0xa1   */ 0x00, 0x00, 0x00},
   {/* 0xa2   */ 0x00, 0x00, 0x00}, {/* 0xa3   */ 0x00, 0x00, 0x00},
   {/* 0xa4   */ 0x00, 0x00, 0x00}, {/* 0xa5   */ 0x00, 0x00, 0x00},
   {/* 0xa6   */ 0x00, 0x00, 0x00}, {/* 0xa7   */ 0x00, 0x00, 0x00},
   {/* 0xa8   */ 0x00, 0x00, 0x00}, {/* 0xa9   */ 0x00, 0x00, 0x00},
   {/* 0xaa   */ 0x00, 0x00, 0x00}, {/* 0xab   */ 0x00, 0x00, 0x00},
   {/* 0xac   */ 0x00, 0x00, 0x00}, {/* 0xad   */ 0x00, 0x00, 0x00},
   {/* 0xae   */ 0x00, 0x00, 0x00}, {/* 0xaf   */ 0x00, 0x00, 0x00},
   {/* 0xb0   */ 0x00, 0x00, 0x00}, {/* 0xb1   */ 0x00, 0x00, 0x00},
   {/* 0xb2   */ 0x00, 0x00, 0x00}, {/* 0xb3   */ 0x00, 0x00, 0x00},
   {/* 0xb4   */ 0x00, 0x00, 0x00}, {/* 0xb5   */ 0x00, 0x00, 0x00},
   {/* 0xb6   */ 0x00, 0x00, 0x00}, {/* 0xb7   */ 0x00, 0x00, 0x00},
   {/* 0xb8   */ 0x00, 0x00, 0x00}, {/* 0xb9   */ 0x00, 0x00, 0x00},
   {/* 0xba   */ 0x00, 0x00, 0x00}, {/* 0xbb   */ 0x00, 0x00, 0x00},
   {/* 0xbc   */ 0x00, 0x00, 0x00}, {/* 0xbd   */ 0x00, 0x00, 0x00},
   {/* 0xbe   */ 0x00, 0x00, 0x00}, {/* 0xbf   */ 0x00, 0x00, 0x00},
   {/* 0xc0   */ 0x00, 0x00, 0x00}, {/* 0xc1   */ 0x00, 0x00, 0x00},
   {/* 0xc2   */ 0x00, 0x00, 0x00}, {/* 0xc3   */ 0x00, 0x00, 0x00},
   {/* 0xc4   */ 0x00, 0x00, 0x00}, {/* 0xc5   */ 0x00, 0x00, 0x00},
   {/* 0xc6   */ 0x00, 0x00, 0x00}, {/* 0xc7   */ 0x00, 0x00, 0x00},
   {/* 0xc8   */ 0x00, 0x00, 0x00}, {/* 0xc9   */ 0x00, 0x00, 0x00},
   {/* 0xca   */ 0x00, 0x00, 0x00}, {/* 0xcb   */ 0x00, 0x00, 0x00},
   {/* 0xcc   */ 0x00, 0x00, 0x00}, {/* 0xcd   */ 0x00, 0x00, 0x00},
   {/* 0xce   */ 0x00, 0x00, 0x00}, {/* 0xcf   */ 0x00, 0x00, 0x00},
   {/* 0xd0   */ 0x00, 0x00, 0x00}, {/* 0xd1   */ 0x00, 0x00, 0x00},
   {/* 0xd2   */ 0x00, 0x00, 0x00}, {/* 0xd3   */ 0x00, 0x00, 0x00},
   {/* 0xd4   */ 0x00, 0x00, 0x00}, {/* 0xd5   */ 0x00, 0x00, 0x00},
   {/* 0xd6   */ 0x00, 0x00, 0x00}, {/* 0xd7   */ 0x00, 0x00, 0x00},
   {/* 0xd8   */ 0x00, 0x00, 0x00}, {/* 0xd9   */ 0x00, 0x00, 0x00},
   {/* 0xda   */ 0x00, 0x00, 0x00}, {/* 0xdb   */ 0x00, 0x00, 0x00},
   {/* 0xdc   */ 0x00, 0x00, 0x00}, {/* 0xdd   */ 0x00, 0x00, 0x00},
   {/* 0xde   */ 0x00, 0x00, 0x00}, {/* 0xdf   */ 0x00, 0x00, 0x00},
   {/* 0xe0   */ 0x00, 0x00, 0x00}, {/* 0xe1   */ 0x00, 0x00, 0x00},
   {/* 0xe2   */ 0x00, 0x00, 0x00}, {/* 0xe3   */ 0x00, 0x00, 0x00},
   {/* 0xe4   */ 0x00, 0x00, 0x00}, {/* 0xe5   */ 0x00, 0x00, 0x00},
   {/* 0xe6   */ 0x00, 0x00, 0x00}, {/* 0xe7   */ 0x00, 0x00, 0x00},
   {/* 0xe8   */ 0x00, 0x00, 0x00}, {/* 0xe9   */ 0x00, 0x00, 0x00},
   {/* 0xea   */ 0x00, 0x00, 0x00}, {/* 0xeb   */ 0x00, 0x00, 0x00},
   {/* 0xec   */ 0x00, 0x00, 0x00}, {/* 0xed   */ 0x00, 0x00, 0x00},
   {/* 0xee   */ 0x00, 0x00, 0x00}, {/* 0xef   */ 0x00, 0x00, 0x00},
   {/* 0xf0   */ 0x00, 0x00, 0x00}, {/* 0xf1   */ 0x00, 0x00, 0x00},
   {/* 0xf2   */ 0x00, 0x00, 0x00}, {/* 0xf3   */ 0x00, 0x00, 0x00},
   {/* 0xf4   */ 0x00, 0x00, 0x00}, {/* 0xf5   */ 0x00, 0x00, 0x00},
   {/* 0xf6   */ 0x00, 0x00, 0x00}, {/* 0xf7   */ 0x00, 0x00, 0x00},
   {/* 0xf8   */ 0x00, 0x00, 0x00}, {/* 0xf9   */ 0x00, 0x00, 0x00},
   {/* 0xfa   */ 0x00, 0x00, 0x00}, {/* 0xfb   */ 0x00, 0x00, 0x00},
   {/* 0xfc   */ 0x00, 0x00, 0x00}, {/* 0xfd   */ 0x00, 0x00, 0x00},
   {/* 0xfe   */ 0x00, 0x00, 0x00}, {/* 0xff   */ 0x00, 0x00, 0x00}};

const bool sexp_char_defs_t::base64digit[256] =
  {/* c is base64 digit */
   /* 0x00   */ false, /* 0x01   */ false, /* 0x02   */ false,
   /* 0x03   */ false, /* 0x04   */ false, /* 0x05   */ false,
   /* 0x06   */ false, /* 0x07   */ false, /* 0x08   */ false,
   /* 0x09   */ false, /* 0x0a   */ false, /* 0x0b   */ false,
   /* 0x0c   */ false, /* 0x0d   */ false, /* 0x0e   */ false,
   /* 0x0f   */ false, /* 0x10   */ false, /* 0x11   */ false,
   /* 0x12   */ false, /* 0x13   */ false, /* 0x14   */ false,
   /* 0x15   */ false, /* 0x16   */ false, /* 0x17   */ false,
   /* 0x18   */ false, /* 0x19   */ false, /* 0x1a   */ false,
   /* 0x1b   */ false, /* 0x1c   */ false, /* 0x1d   */ false,
   /* 0x1e   */ false, /* 0x1f   */ false, /* 0x20   */ false,
   /* 0x21 ! */ false, /* 0x22 " */ false, /* 0x23 # */ false,
   /* 0x24 $ */ false, /* 0x25 % */ false, /* 0x26 & */ false,
   /* 0x27 ' */ false, /* 0x28 ( */ false, /* 0x29 ) */ false,
   /* 0x2a * */ false, /* 0x2b + */ true,  /* 0x2c , */ false,
   /* 0x2d - */ false, /* 0x2e . */ false, /* 0x2f / */ true,
   /* 0x30 0 */ true,  /* 0x31 1 */ true,  /* 0x32 2 */ true,
   /* 0x33 3 */ true,  /* 0x34 4 */ true,  /* 0x35 5 */ true,
   /* 0x36 6 */ true,  /* 0x37 7 */ true,  /* 0x38 8 */ true,
   /* 0x39 9 */ true,  /* 0x3a : */ false, /* 0x3b ; */ false,
   /* 0x3c < */ false, /* 0x3d = */ false, /* 0x3e > */ false,
   /* 0x3f ? */ false, /* 0x40 @ */ false, /* 0x41 A */ true,
   /* 0x42 B */ true,  /* 0x43 C */ true,  /* 0x44 D */ true,
   /* 0x45 E */ true,  /* 0x46 F */ true,  /* 0x47 G */ true,
   /* 0x48 H */ true,  /* 0x49 I */ true,  /* 0x4a J */ true,
   /* 0x4b K */ true,  /* 0x4c L */ true,  /* 0x4d M */ true,
   /* 0x4e N */ true,  /* 0x4f O */ true,  /* 0x50 P */ true,
   /* 0x51 Q */ true,  /* 0x52 R */ true,  /* 0x53 S */ true,
   /* 0x54 T */ true,  /* 0x55 U */ true,  /* 0x56 V */ true,
   /* 0x57 W */ true,  /* 0x58 X */ true,  /* 0x59 Y */ true,
   /* 0x5a Z */ true,  /* 0x5b [ */ false, /* 0x5c \ */ false,
   /* 0x5d ] */ false, /* 0x5e ^ */ false, /* 0x5f _ */ false,
   /* 0x60 ` */ false, /* 0x61 a */ true,  /* 0x62 b */ true,
   /* 0x63 c */ true,  /* 0x64 d */ true,  /* 0x65 e */ true,
   /* 0x66 f */ true,  /* 0x67 g */ true,  /* 0x68 h */ true,
   /* 0x69 i */ true,  /* 0x6a j */ true,  /* 0x6b k */ true,
   /* 0x6c l */ true,  /* 0x6d m */ true,  /* 0x6e n */ true,
   /* 0x6f o */ true,  /* 0x70 p */ true,  /* 0x71 q */ true,
   /* 0x72 r */ true,  /* 0x73 s */ true,  /* 0x74 t */ true,
   /* 0x75 u */ true,  /* 0x76 v */ true,  /* 0x77 w */ true,
   /* 0x78 x */ true,  /* 0x79 y */ true,  /* 0x7a z */ true,
   /* 0x7b { */ false, /* 0x7c | */ false, /* 0x7d } */ false,
   /* 0x7e ~ */ false, /* 0x7f   */ false, /* 0x80   */ false,
   /* 0x81   */ false, /* 0x82   */ false, /* 0x83   */ false,
   /* 0x84   */ false, /* 0x85   */ false, /* 0x86   */ false,
   /* 0x87   */ false, /* 0x88   */ false, /* 0x89   */ false,
   /* 0x8a   */ false, /* 0x8b   */ false, /* 0x8c   */ false,
   /* 0x8d   */ false, /* 0x8e   */ false, /* 0x8f   */ false,
   /* 0x90   */ false, /* 0x91   */ false, /* 0x92   */ false,
   /* 0x93   */ false, /* 0x94   */ false, /* 0x95   */ false,
   /* 0x96   */ false, /* 0x97   */ false, /* 0x98   */ false,
   /* 0x99   */ false, /* 0x9a   */ false, /* 0x9b   */ false,
   /* 0x9c   */ false, /* 0x9d   */ false, /* 0x9e   */ false,
   /* 0x9f   */ false, /* 0xa0   */ false, /* 0xa1   */ false,
   /* 0xa2   */ false, /* 0xa3   */ false, /* 0xa4   */ false,
   /* 0xa5   */ false, /* 0xa6   */ false, /* 0xa7   */ false,
   /* 0xa8   */ false, /* 0xa9   */ false, /* 0xaa   */ false,
   /* 0xab   */ false, /* 0xac   */ false, /* 0xad   */ false,
   /* 0xae   */ false, /* 0xaf   */ false, /* 0xb0   */ false,
   /* 0xb1   */ false, /* 0xb2   */ false, /* 0xb3   */ false,
   /* 0xb4   */ false, /* 0xb5   */ false, /* 0xb6   */ false,
   /* 0xb7   */ false, /* 0xb8   */ false, /* 0xb9   */ false,
   /* 0xba   */ false, /* 0xbb   */ false, /* 0xbc   */ false,
   /* 0xbd   */ false, /* 0xbe   */ false, /* 0xbf   */ false,
   /* 0xc0   */ false, /* 0xc1   */ false, /* 0xc2   */ false,
   /* 0xc3   */ false, /* 0xc4   */ false, /* 0xc5   */ false,
   /* 0xc6   */ false, /* 0xc7   */ false, /* 0xc8   */ false,
   /* 0xc9   */ false, /* 0xca   */ false, /* 0xcb   */ false,
   /* 0xcc   */ false, /* 0xcd   */ false, /* 0xce   */ false,
   /* 0xcf   */ false, /* 0xd0   */ false, /* 0xd1   */ false,
   /* 0xd2   */ false, /* 0xd3   */ false, /* 0xd4   */ false,
   /* 0xd5   */ false, /* 0xd6   */ false, /* 0xd7   */ false,
   /* 0xd8   */ false, /* 0xd9   */ false, /* 0xda   */ false,
   /* 0xdb   */ false, /* 0xdc   */ false, /* 0xdd   */ false,
   /* 0xde   */ false, /* 0xdf   */ false, /* 0xe0   */ false,
   /* 0xe1   */ false, /* 0xe2   */ false, /* 0xe3   */ false,
   /* 0xe4   */ false, /* 0xe5   */ false, /* 0xe6   */ false,
   /* 0xe7   */ false, /* 0xe8   */ false, /* 0xe9   */ false,
   /* 0xea   */ false, /* 0xeb   */ false, /* 0xec   */ false,
   /* 0xed   */ false, /* 0xee   */ false, /* 0xef   */ false,
   /* 0xf0   */ false, /* 0xf1   */ false, /* 0xf2   */ false,
   /* 0xf3   */ false, /* 0xf4   */ false, /* 0xf5   */ false,
   /* 0xf6   */ false, /* 0xf7   */ false, /* 0xf8   */ false,
   /* 0xf9   */ false, /* 0xfa   */ false, /* 0xfb   */ false,
   /* 0xfc   */ false, /* 0xfd   */ false, /* 0xfe   */ false};

const bool sexp_char_defs_t::tokenchar[256] =
  {/* c can be in a token */
   /* 0x00   */ false, /* 0x01   */ false, /* 0x02   */ false,
   /* 0x03   */ false, /* 0x04   */ false, /* 0x05   */ false,
   /* 0x06   */ false, /* 0x07   */ false, /* 0x08   */ false,
   /* 0x09   */ false, /* 0x0a   */ false, /* 0x0b   */ false,
   /* 0x0c   */ false, /* 0x0d   */ false, /* 0x0e   */ false,
   /* 0x0f   */ false, /* 0x10   */ false, /* 0x11   */ false,
   /* 0x12   */ false, /* 0x13   */ false, /* 0x14   */ false,
   /* 0x15   */ false, /* 0x16   */ false, /* 0x17   */ false,
   /* 0x18   */ false, /* 0x19   */ false, /* 0x1a   */ false,
   /* 0x1b   */ false, /* 0x1c   */ false, /* 0x1d   */ false,
   /* 0x1e   */ false, /* 0x1f   */ false, /* 0x20   */ false,
   /* 0x21 ! */ false, /* 0x22 " */ false, /* 0x23 # */ false,
   /* 0x24 $ */ false, /* 0x25 % */ false, /* 0x26 & */ false,
   /* 0x27 ' */ false, /* 0x28 ( */ false, /* 0x29 ) */ false,
   /* 0x2a * */ true,  /* 0x2b + */ true,  /* 0x2c , */ false,
   /* 0x2d - */ true,  /* 0x2e . */ true,  /* 0x2f / */ true,
   /* 0x30 0 */ true,  /* 0x31 1 */ true,  /* 0x32 2 */ true,
   /* 0x33 3 */ true,  /* 0x34 4 */ true,  /* 0x35 5 */ true,
   /* 0x36 6 */ true,  /* 0x37 7 */ true,  /* 0x38 8 */ true,
   /* 0x39 9 */ true,  /* 0x3a : */ true,  /* 0x3b ; */ false,
   /* 0x3c < */ false, /* 0x3d = */ true,  /* 0x3e > */ false,
   /* 0x3f ? */ false, /* 0x40 @ */ false, /* 0x41 A */ true,
   /* 0x42 B */ true,  /* 0x43 C */ true,  /* 0x44 D */ true,
   /* 0x45 E */ true,  /* 0x46 F */ true,  /* 0x47 G */ true,
   /* 0x48 H */ true,  /* 0x49 I */ true,  /* 0x4a J */ true,
   /* 0x4b K */ true,  /* 0x4c L */ true,  /* 0x4d M */ true,
   /* 0x4e N */ true,  /* 0x4f O */ true,  /* 0x50 P */ true,
   /* 0x51 Q */ true,  /* 0x52 R */ true,  /* 0x53 S */ true,
   /* 0x54 T */ true,  /* 0x55 U */ true,  /* 0x56 V */ true,
   /* 0x57 W */ true,  /* 0x58 X */ true,  /* 0x59 Y */ true,
   /* 0x5a Z */ true,  /* 0x5b [ */ false, /* 0x5c \ */ false,
   /* 0x5d ] */ false, /* 0x5e ^ */ false, /* 0x5f _ */ true,
   /* 0x60 ` */ false, /* 0x61 a */ true,  /* 0x62 b */ true,
   /* 0x63 c */ true,  /* 0x64 d */ true,  /* 0x65 e */ true,
   /* 0x66 f */ true,  /* 0x67 g */ true,  /* 0x68 h */ true,
   /* 0x69 i */ true,  /* 0x6a j */ true,  /* 0x6b k */ true,
   /* 0x6c l */ true,  /* 0x6d m */ true,  /* 0x6e n */ true,
   /* 0x6f o */ true,  /* 0x70 p */ true,  /* 0x71 q */ true,
   /* 0x72 r */ true,  /* 0x73 s */ true,  /* 0x74 t */ true,
   /* 0x75 u */ true,  /* 0x76 v */ true,  /* 0x77 w */ true,
   /* 0x78 x */ true,  /* 0x79 y */ true,  /* 0x7a z */ true,
   /* 0x7b { */ false, /* 0x7c | */ false, /* 0x7d } */ false,
   /* 0x7e ~ */ false, /* 0x7f   */ false, /* 0x80   */ false,
   /* 0x81   */ false, /* 0x82   */ false, /* 0x83   */ false,
   /* 0x84   */ false, /* 0x85   */ false, /* 0x86   */ false,
   /* 0x87   */ false, /* 0x88   */ false, /* 0x89   */ false,
   /* 0x8a   */ false, /* 0x8b   */ false, /* 0x8c   */ false,
   /* 0x8d   */ false, /* 0x8e   */ false, /* 0x8f   */ false,
   /* 0x90   */ false, /* 0x91   */ false, /* 0x92   */ false,
   /* 0x93   */ false, /* 0x94   */ false, /* 0x95   */ false,
   /* 0x96   */ false, /* 0x97   */ false, /* 0x98   */ false,
   /* 0x99   */ false, /* 0x9a   */ false, /* 0x9b   */ false,
   /* 0x9c   */ false, /* 0x9d   */ false, /* 0x9e   */ false,
   /* 0x9f   */ false, /* 0xa0   */ false, /* 0xa1   */ false,
   /* 0xa2   */ false, /* 0xa3   */ false, /* 0xa4   */ false,
   /* 0xa5   */ false, /* 0xa6   */ false, /* 0xa7   */ false,
   /* 0xa8   */ false, /* 0xa9   */ false, /* 0xaa   */ false,
   /* 0xab   */ false, /* 0xac   */ false, /* 0xad   */ false,
   /* 0xae   */ false, /* 0xaf   */ false, /* 0xb0   */ false,
   /* 0xb1   */ false, /* 0xb2   */ false, /* 0xb3   */ false,
   /* 0xb4   */ false, /* 0xb5   */ false, /* 0xb6   */ false,
   /* 0xb7   */ false, /* 0xb8   */ false, /* 0xb9   */ false,
   /* 0xba   */ false, /* 0xbb   */ false, /* 0xbc   */ false,
   /* 0xbd   */ false, /* 0xbe   */ false, /* 0xbf   */ false,
   /* 0xc0   */ false, /* 0xc1   */ false, /* 0xc2   */ false,
   /* 0xc3   */ false, /* 0xc4   */ false, /* 0xc5   */ false,
   /* 0xc6   */ false, /* 0xc7   */ false, /* 0xc8   */ false,
   /* 0xc9   */ false, /* 0xca   */ false, /* 0xcb   */ false,
   /* 0xcc   */ false, /* 0xcd   */ false, /* 0xce   */ false,
   /* 0xcf   */ false, /* 0xd0   */ false, /* 0xd1   */ false,
   /* 0xd2   */ false, /* 0xd3   */ false, /* 0xd4   */ false,
   /* 0xd5   */ false, /* 0xd6   */ false, /* 0xd7   */ false,
   /* 0xd8   */ false, /* 0xd9   */ false, /* 0xda   */ false,
   /* 0xdb   */ false, /* 0xdc   */ false, /* 0xdd   */ false,
   /* 0xde   */ false, /* 0xdf   */ false, /* 0xe0   */ false,
   /* 0xe1   */ false, /* 0xe2   */ false, /* 0xe3   */ false,
   /* 0xe4   */ false, /* 0xe5   */ false, /* 0xe6   */ false,
   /* 0xe7   */ false, /* 0xe8   */ false, /* 0xe9   */ false,
   /* 0xea   */ false, /* 0xeb   */ false, /* 0xec   */ false,
   /* 0xed   */ false, /* 0xee   */ false, /* 0xef   */ false,
   /* 0xf0   */ false, /* 0xf1   */ false, /* 0xf2   */ false,
   /* 0xf3   */ false, /* 0xf4   */ false, /* 0xf5   */ false,
   /* 0xf6   */ false, /* 0xf7   */ false, /* 0xf8   */ false,
   /* 0xf9   */ false, /* 0xfa   */ false, /* 0xfb   */ false,
   /* 0xfc   */ false, /* 0xfd   */ false, /* 0xfe   */ false};

} // namespace sexp
