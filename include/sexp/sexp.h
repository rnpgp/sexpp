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
 * Original copyright
 *
 * SEXP standard header file: sexp.h
 * Ronald L. Rivest
 * 6/29/1997
 */

#pragma once

#include <vector>
#include <algorithm>
#include <fstream>

namespace sexp {

class sexp_char_defs {
protected:
  static char upper[256];       /* upper[c] is upper case version of c */
  static bool whitespace[256];  /* whitespace[c] is nonzero if c is whitespace */
  static bool decdigit[256];    /* decdigit[c] is nonzero if c is a dec digit */
  static char decvalue[256];    /* decvalue[c] is value of c as dec digit */
  static bool hexdigit[256];    /* hexdigit[c] is nonzero if c is a hex digit */
  static char hexvalue[256];    /* hexvalue[c] is value of c as a hex digit */
  static bool base64digit[256]; /* base64char[c] is nonzero if c is base64 digit */
  static char base64value[256]; /* base64value[c] is value of c as base64 digit */
  static bool tokenchar[256];   /* tokenchar[c] is true if c can be in a token */
  static bool alpha[256];       /* alpha[c] is true if c is alphabetic A-Z a-z */

  static bool initialized;
  static void initializeCharacterTables(void);
  static bool isWhiteSpace(int c);
  static bool isDecDigit(int c);
  static bool isHexDigit(int c);
  static bool isBase64Digit(int c);
  static bool isTokenChar(int c);
  static bool isAlpha(int c);

  sexp_char_defs(void) {
      if (!initialized) {
        initializeCharacterTables();
        initialized = true;
      }
  }
};

/***********************************/
/* SEXP SIMPLE STRING              */
/***********************************/
class sexpOutputStream;

typedef unsigned char octet;

class sexpSimpleString : public std::basic_string<octet>, private sexp_char_defs {
public:
  sexpSimpleString& append(int c) {
    (*this) += (octet) (c & 0xFF);
    return *this;
  }
  // Returns length for printing simple string ss as a token
  size_t advancedLengthToken(void) const { return length(); }
  // Returns length for printing simple string ss as a base64 string
  size_t advancedLengthBase64(void) const { return(2+4*((length()+2)/3)); }
  // Returns length for printing simple string ss in verbatim mode
  size_t advancedLengthVerbatim(void) const {
    size_t len = length(), i = 1;
    while (len > 9L) { i++; len = len / 10; }
    return(i+1+len);
  }
  // Returns length for printing simple string ss in quoted-string mode
  size_t advancedLengthQuotedString(void) const { return(1+length()+1); }
  // Returns length for printing simple string ss in hexadecimal mode
  size_t advancedLengthHexadecimal(void) const { return(1+2*length()+1); }
  size_t advancedLength(sexpOutputStream *os) const;

  sexpOutputStream* printCanonicalVerbatim(sexpOutputStream *os) const;
  sexpOutputStream* printAdvanced(sexpOutputStream *os) const;
  sexpOutputStream* printToken(sexpOutputStream *os) const;
  sexpOutputStream* printVerbatim(sexpOutputStream *os) const;
  sexpOutputStream* printQuoted(sexpOutputStream *os) const;
  sexpOutputStream* printHex(sexpOutputStream *os) const;
  sexpOutputStream* printBase64(sexpOutputStream *os) const;

  bool canPrintAsQuotedString(void) const;
  bool canPrintAsToken(const sexpOutputStream *os) const;

};

/***********************************/
/* SEXP OBJECT                     */
/***********************************/

class sexpObject {
public:
  virtual ~sexpObject() { };

  virtual sexpOutputStream* printCanonical(sexpOutputStream* os) const = 0;
  virtual sexpOutputStream* printAdvanced(sexpOutputStream* os) const;
  virtual size_t            advancedLength(sexpOutputStream* os) const = 0;
};

/***********************************/
/* SEXP STRING                     */
/***********************************/
class sexpString : public sexpObject {
protected:
  sexpSimpleString *presentationHint;
  sexpSimpleString *string;

public:
  sexpString(void): presentationHint(NULL),
                    string(NULL) {
  }

  virtual ~sexpString() {
    if (presentationHint) delete presentationHint;
    if (string) delete string;
  }

  sexpSimpleString* getString(void) const { return string; }
  sexpSimpleString* setString(sexpSimpleString*  ss) { return string = ss; }
  sexpSimpleString* getPresentationHint(void) const { return presentationHint; }
  sexpSimpleString* setPresentationHint(sexpSimpleString* ph) { return presentationHint = ph; }

  virtual sexpOutputStream* printCanonical(sexpOutputStream* os) const;
  virtual sexpOutputStream* printAdvanced(sexpOutputStream* os) const;
  virtual size_t            advancedLength(sexpOutputStream* os) const;

};

/***********************************/
/* SEXP LIST                     */
/***********************************/
class sexpList : public sexpObject, public std::vector<sexpObject*> {
public:
  virtual ~sexpList() {
      sexpObject* pd;
      for(auto it = begin(); it != end(); ++it) {
        pd = *it;
        delete pd;
    }
  }

  virtual sexpOutputStream* printCanonical(sexpOutputStream* os) const;
  virtual sexpOutputStream* printAdvanced(sexpOutputStream* os) const;
  virtual size_t            advancedLength(sexpOutputStream* os) const;
};

class sexpInputStream : private sexp_char_defs {
protected:
  std::istream* inputFile;
  size_t byteSize;        /* 4 or 6 or 8 == currently scanning mode */
public:
  int nextChar;        /* character currently being scanned */
  int bits;            /* Bits waiting to be used */
  int nBits;           /* number of such bits waiting to be used */
  int count;           /* number of 8-bit characters output by getChar */

  sexpInputStream(std::istream* i);
  void setInput(std::istream* i) { inputFile = i; }
  sexpInputStream* setByteSize(size_t newByteSize);
  size_t getByteSize(void) { return byteSize; }
  sexpInputStream* getChar(void);
  sexpInputStream* skipWhiteSpace(void);
  sexpInputStream* skipChar(int c);

  sexpObject* scanToEOF();
  sexpObject* scanObject(void);
  sexpString* scanString(void);
  sexpList* scanList(void);
  sexpSimpleString *scanSimpleString(void);
  void scanToken(sexpSimpleString *ss);
  void scanVerbatimString(sexpSimpleString *ss, int length);
  void scanQuotedString(sexpSimpleString *ss, int length);
  void scanHexString(sexpSimpleString *ss, int length);
  void scanBase64String(sexpSimpleString *ss, int length);
  int scanDecimal(void);
};

class sexpOutputStream {
public:
  const size_t defaultlineliength = 75;
  enum sexpPrintMode { /* PRINTING MODES */
    canonical = 1,     /* standard for hashing and tranmission */
    base64 = 2,        /* base64 version of canonical */
    advanced = 3       /* pretty-printed */
  };
protected:
  std::ostream* outputFile;
  size_t base64Count;       /* number of hex or base64 chars printed this region */
  size_t byteSize;          /* 4 or 6 or 8 depending on output mode */
  size_t bits;              /* bits waiting to go out */
  size_t nBits;             /* number of bits waiting to go out */
  sexpPrintMode mode;       /* base64, advanced, or canonical */
  size_t column;            /* column where next character will go */
  size_t maxcolumn;         /* max usable column, or 0 if no maximum */
  size_t indent;            /* current indentation level (starts at 0) */
public:
  sexpOutputStream(std::ostream* o);
  void setOutput(std::ostream* o) { outputFile = o; }
  sexpOutputStream* putChar(int c);                   /* output a character */
  sexpOutputStream* newLine(sexpPrintMode mode);      /* go to next line (and indent) */
  sexpOutputStream* varPutChar(int c);
  sexpOutputStream* flushOutput(void);
  sexpOutputStream* printDecimal(long int n);

  sexpOutputStream* changeOutputByteSize(int newByteSize, sexpPrintMode mode);

  sexpOutputStream* printCanonical(const sexpObject* obj) { return obj->printCanonical(this); }
  sexpOutputStream* printAdvanced(const sexpObject *obj)  { return obj->printAdvanced(this); };
  sexpOutputStream* printBase64(const sexpObject *obj);
  sexpOutputStream* printCanonical(const sexpSimpleString* ss) { return ss->printCanonicalVerbatim(this); }
  sexpOutputStream* printAdvanced(const sexpSimpleString *ss)  { return ss->printAdvanced(this); };

  size_t getByteSize(void) const { return byteSize; }
  size_t getColumn(void) const { return column; }
  size_t resetColumn(void) { return column = 0; }
  size_t getMaxColumn(void) const { return maxcolumn; }
  size_t setMaxColumn(size_t  mc) { return maxcolumn = mc; }
  sexpOutputStream* incIndent(void) { ++indent; return this; }
  sexpOutputStream* decIndent(void) { --indent; return this; }
};

}
