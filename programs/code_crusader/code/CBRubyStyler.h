/******************************************************************************
 CBRubyStyler.h

	Copyright (C) 2003 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBRubyStyler
#define _H_CBRubyStyler

#include "CBStylerBase.h"
#include "CBRubyScanner.h"

class CBRubyStyler : public CBStylerBase, public CBRubyScanner
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();

	virtual ~CBRubyStyler();

protected:

	CBRubyStyler();

	virtual void	Scan(std::istream& input, const TokenExtra& initData);

	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles);

private:

	static CBRubyStyler*	itsSelf;

private:

	void	ExtendCheckRangeForString(const JIndexRange& tokenRange);
	void	StyleEmbeddedVariables(const Token& token);

	// not allowed

	CBRubyStyler(const CBRubyStyler& source);
	const CBRubyStyler& operator=(const CBRubyStyler& source);
};

#endif
