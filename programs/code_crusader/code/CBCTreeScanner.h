// code/CBCTreeScanner.h generated by reflex 3.0.10 from code/CBCTreeScanner.l

#ifndef REFLEX_CODE_CBCTREESCANNER_H
#define REFLEX_CODE_CBCTREESCANNER_H
#define IN_HEADER 1
#define REFLEX_VERSION "3.0.10"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  OPTIONS USED                                                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#define REFLEX_OPTION_freespace           true
#define REFLEX_OPTION_full                true
#define REFLEX_OPTION_header_file         "code/CBCTreeScanner.h"
#define REFLEX_OPTION_lex                 CreateClasses
#define REFLEX_OPTION_lexer               Scanner
#define REFLEX_OPTION_namespace           CB::CTree
#define REFLEX_OPTION_nodefault           true
#define REFLEX_OPTION_outfile             "code/CBCTreeScanner.cpp"
#define REFLEX_OPTION_params              const JFAID_t fileID, CBTree* tree, JPtrArray<CBClass>* classList
#define REFLEX_OPTION_token_type          bool
#define REFLEX_OPTION_unicode             true

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  SECTION 1: %top user code                                                 //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#line 1 "code/CBCTreeScanner.l"

/*
Copyright © 1998 by Dustin Laurence.
Copyright © 2021 by John Lindal.

This scanner reads a C/C++ file and creates CBClass instances.
*/

#include "CBCClass.h"
#include <JStringIterator.h>
#include <JStack.h>
#include <JFAID.h>
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
namespace CTree {

class Scanner : public reflex::AbstractLexer<reflex::Matcher> {
#line 21 "code/CBCTreeScanner.l"


public:

	virtual	~Scanner();

private:

	JInteger				itsCurlyBraceDepth;
	bool					itsTemplateFlag;
	JInteger				itsTemplateBracketDepth;
	CBClass::InheritType	itsInheritType;
	CBClass*				itsCurrentClass;

	JPtrArray<JString>*					itsUsingList;
	JStack<JInteger, JArray<JInteger>>	itsCurlyBraceStack;

	JString				itsNamespaceStr;
	JPtrArray<JString>*	itsNamespaceStack;

private:

	void ResetState()
	{
		itsTemplateFlag = false;
		itsCurrentClass = nullptr;
	}

 public:
  typedef reflex::AbstractLexer<reflex::Matcher> AbstractBaseLexer;
  Scanner(
      const reflex::Input& input = reflex::Input(),
      std::ostream&        os    = std::cout)
    :
      AbstractBaseLexer(input, os)
  {
#line 50 "code/CBCTreeScanner.l"

	itsUsingList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsUsingList != nullptr );

	itsNamespaceStack = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsNamespaceStack != nullptr );

  }
  static const int INITIAL = 0;
  static const int NAMESPACE_STATE = 1;
  static const int CLASS_NAME_STATE = 2;
  static const int INHERIT_STATE = 3;
  static const int TEMPLATE_PARAM_STATE = 4;
  static const int C_COMMENT_STATE = 5;
  static const int CPP_COMMENT_STATE = 6;
  static const int STRING_STATE = 7;
  virtual bool CreateClasses(const JFAID_t fileID, CBTree* tree, JPtrArray<CBClass>* classList);
  bool CreateClasses(const reflex::Input& input, const JFAID_t fileID, CBTree* tree, JPtrArray<CBClass>* classList)
  {
    in(input);
    return CreateClasses(fileID, tree, classList);
  }
  bool CreateClasses(const reflex::Input& input, std::ostream *os, const JFAID_t fileID, CBTree* tree, JPtrArray<CBClass>* classList)
  {
    in(input);
    if (os)
      out(*os);
    return CreateClasses(fileID, tree, classList);
  }
};

} // namespace CB
} // namespace CTree

#endif
