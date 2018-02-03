/******************************************************************************
 CBBourneShellStyler.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBBourneShellStyler
#define _H_CBBourneShellStyler

#include "CBStylerBase.h"
#include "CBBourneShellScanner.h"

class CBBourneShellStyler : public CBStylerBase, public CBBourneShellScanner
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();

	virtual ~CBBourneShellStyler();

protected:

	CBBourneShellStyler();

	virtual void	Scan(std::istream& input, const TokenExtra& initData);
	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles);

private:

	static CBBourneShellStyler*	itsSelf;

private:

	void	ExtendCheckRangeForString(const JIndexRange& tokenRange);
	void	StyleEmbeddedVariables(const Token& token);

	// not allowed

	CBBourneShellStyler(const CBBourneShellStyler& source);
	const CBBourneShellStyler& operator=(const CBBourneShellStyler& source);
};

#endif
