/******************************************************************************
 CBCShellStyler.h

	Copyright (C) 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBCShellStyler
#define _H_CBCShellStyler

#include "CBStylerBase.h"
#include "CBCShellScanner.h"

class CBCShellStyler : public CBStylerBase, public CBCShellScanner
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();

	virtual ~CBCShellStyler();

protected:

	CBCShellStyler();

	virtual void	Scan(istream& input, const TokenExtra& initData);
	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles);

private:

	static CBCShellStyler*	itsSelf;

private:

	void	ExtendCheckRangeForString(const JIndexRange& tokenRange);
	void	StyleEmbeddedVariables(const Token& token);

	// not allowed

	CBCShellStyler(const CBCShellStyler& source);
	const CBCShellStyler& operator=(const CBCShellStyler& source);
};

#endif
