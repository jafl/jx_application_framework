// code/CBPropertiesScanner.h generated by reflex 3.0.11 from code/CBPropertiesScanner.ll

#ifndef REFLEX_CODE_CBPROPERTIESSCANNER_H
#define REFLEX_CODE_CBPROPERTIESSCANNER_H
#define IN_HEADER 1
#define REFLEX_VERSION "3.0.11"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  OPTIONS USED                                                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#undef REFLEX_OPTION_freespace
#undef REFLEX_OPTION_full
#undef REFLEX_OPTION_header_file
#undef REFLEX_OPTION_lex
#undef REFLEX_OPTION_lexer
#undef REFLEX_OPTION_namespace
#undef REFLEX_OPTION_nodefault
#undef REFLEX_OPTION_outfile
#undef REFLEX_OPTION_token_type
#undef REFLEX_OPTION_unicode

#define REFLEX_OPTION_freespace           true
#define REFLEX_OPTION_full                true
#define REFLEX_OPTION_header_file         "code/CBPropertiesScanner.h"
#define REFLEX_OPTION_lex                 NextToken
#define REFLEX_OPTION_lexer               Scanner
#define REFLEX_OPTION_namespace           CB::Properties
#define REFLEX_OPTION_nodefault           true
#define REFLEX_OPTION_outfile             "code/CBPropertiesScanner.cpp"
#define REFLEX_OPTION_token_type          CBStylingScannerBase::Token
#define REFLEX_OPTION_unicode             true

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  SECTION 1: %top user code                                                 //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#line 1 "code/CBPropertiesScanner.ll"

/*
Copyright © 2015 by John Lindal.

This scanner reads a .properties file and returns CB::Properties::Scanner::Tokens.
*/

#include "CBStylingScannerBase.h"
#include <jAssert.h>


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  REGEX MATCHER                                                             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <reflex/matcher.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  ABSTRACT LEXER CLASS                                                      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <reflex/abslexer.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  LEXER CLASS                                                               //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

namespace CB {
namespace Properties {

class Scanner : public CBStylingScannerBase {
#line 16 "code/CBPropertiesScanner.ll"


public:

	// these types are ordered to correspond to the type table in CBPropertiesStyler

	enum TokenType
	{
		kNonPrintChar = kEOF+1,

		kWhitespace,	// must be the one before the first item in type table

		kKeyOrValue,
		kAssignment,

		kComment,

		kError			// place holder for CBPropertiesStyler
	};

 public:
  typedef CBStylingScannerBase AbstractBaseLexer;
  Scanner(
      const reflex::Input& input = reflex::Input(),
      std::ostream&        os    = std::cout)
    :
      AbstractBaseLexer(input, os)
  {
  }
  static const int INITIAL = 0;
  virtual CBStylingScannerBase::Token NextToken(void);
  CBStylingScannerBase::Token NextToken(const reflex::Input& input)
  {
    in(input);
    return NextToken();
  }
  CBStylingScannerBase::Token NextToken(const reflex::Input& input, std::ostream *os)
  {
    in(input);
    if (os)
      out(*os);
    return NextToken();
  }
};

} // namespace CB
} // namespace Properties

#endif
