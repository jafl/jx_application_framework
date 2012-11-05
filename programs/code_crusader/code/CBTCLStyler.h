/******************************************************************************
 CBTCLStyler.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBTCLStyler
#define _H_CBTCLStyler

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CBStylerBase.h"
#include "CBTCLScanner.h"

class CBTCLStyler : public CBStylerBase, public CBTCLScanner
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();

	virtual ~CBTCLStyler();

protected:

	CBTCLStyler();

	virtual void	Scan(istream& input, const TokenExtra& initData);
	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles);

private:

	static CBTCLStyler*	itsSelf;

private:

	void	ExtendCheckRangeForString(const JIndexRange& tokenRange);
	void	StyleEmbeddedVariables(const Token& token);

	// not allowed

	CBTCLStyler(const CBTCLStyler& source);
	const CBTCLStyler& operator=(const CBTCLStyler& source);
};

#endif
