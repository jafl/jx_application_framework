/******************************************************************************
 CBCSharpStyler.h

	Copyright © 2004 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBCSharpStyler
#define _H_CBCSharpStyler

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CBStylerBase.h"
#include "CBCSharpScanner.h"

class CBCSharpStyler : public CBStylerBase, public CBCSharpScanner
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();

	virtual ~CBCSharpStyler();

protected:

	CBCSharpStyler();

	virtual void	Scan(istream& input, const TokenExtra& initData);
	virtual void	PreexpandCheckRange(const JString& text,
										const JRunArray<JFont>& styles,
										const JIndexRange& modifiedRange,
										const JBoolean deletion,
										JIndexRange* checkRange);

	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles);

private:

	static CBCSharpStyler*	itsSelf;

private:

	JBoolean	SlurpPPComment(JIndexRange* totalRange);

	// not allowed

	CBCSharpStyler(const CBCSharpStyler& source);
	const CBCSharpStyler& operator=(const CBCSharpStyler& source);
};

#endif
