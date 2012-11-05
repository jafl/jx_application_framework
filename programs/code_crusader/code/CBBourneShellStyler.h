/******************************************************************************
 CBBourneShellStyler.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBBourneShellStyler
#define _H_CBBourneShellStyler

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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

	virtual void	Scan(istream& input, const TokenExtra& initData);
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
