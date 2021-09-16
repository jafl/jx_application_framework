// code/CBRubyScanner.h generated by reflex 3.0.10 from code/CBRubyScanner.ll

#ifndef allow_multiple_includesREFLEX_CODE_CBRUBYSCANNER_H
#define allow_multiple_includesREFLEX_CODE_CBRUBYSCANNER_H
#define allow_multiple_includesIN_HEADER 1
#define REFLEX_VERSION "3.0.10"

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
#undef REFLEX_OPTION_prefix
#undef REFLEX_OPTION_token_type
#undef REFLEX_OPTION_unicode

#define REFLEX_OPTION_freespace           true
#define REFLEX_OPTION_full                true
#define REFLEX_OPTION_header_file         "code/CBRubyScanner.h"
#define REFLEX_OPTION_lex                 NextToken
#define REFLEX_OPTION_lexer               Scanner
#define REFLEX_OPTION_namespace           CB::Ruby
#define REFLEX_OPTION_nodefault           true
#define REFLEX_OPTION_outfile             "code/CBRubyScanner.cpp"
#define REFLEX_OPTION_prefix              allow_multiple_includes
#define REFLEX_OPTION_token_type          CBStylingScannerBase::Token
#define REFLEX_OPTION_unicode             true

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  SECTION 1: %top user code                                                 //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#line 1 "code/CBRubyScanner.ll"

/*
Copyright © 2003 by John Lindal.

This scanner reads a Ruby file and returns CB::Ruby::Scanner::Tokens.
*/

#include "CBStylingScannerBase.h"
#include <JStringIterator.h>
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
namespace Ruby {

class Scanner : public CBStylingScannerBase {
#line 17 "code/CBRubyScanner.ll"


public:

	// these types are ordered to correspond to the type table in CBRubyStyler

	enum TokenType
	{
		kBadInt = kEOF+1,
		kBadBinary,
		kBadHex,
		kUnterminatedString,
		kUnterminatedRegex,
		kUnterminatedWordList,
		kUnterminatedEmbeddedDoc,
		kIllegalChar,
		kNonPrintChar,

		kWhitespace,	// must be the one before the first item in type table

		kLocalVariable,
		kInstanceVariable,
		kGlobalVariable,
		kSymbol,
		kReservedKeyword,

		kOperator,
		kDelimiter,

		kSingleQuoteString,
		kDoubleQuoteString,
		kHereDocString,
		kExecString,
		kWordList,

		kFloat,
		kDecimalInt,
		kBinaryInt,
		kOctalInt,
		kHexInt,

		kRegex,

		kComment,
		kEmbeddedDoc,

		kError			// place holder for CBRubyStyler
	};

public:

	virtual void	BeginScan(const JStyledText* text,
							  const JStyledText::TextIndex& startIndex,
							  std::istream& input);

private:

	bool		itsProbableOperatorFlag;	// kTrue if /,? are most likely operators instead of regex
	JString		itsHereDocTag;
	TokenType	itsHereDocType;

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
  static const int SINGLE_STRING_STATE = 1;
  static const int DOUBLE_STRING_STATE = 2;
  static const int EXEC_STRING_STATE = 3;
  static const int HEREDOC_STRING_STATE = 4;
  static const int REGEX_STATE = 5;
  static const int EMBEDDED_DOC_STATE = 6;
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
} // namespace Ruby

#endif
