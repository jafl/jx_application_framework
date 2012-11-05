/******************************************************************************
 CBPerlStyler.h

	Copyright © 2003 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBPerlStyler
#define _H_CBPerlStyler

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CBStylerBase.h"
#include "CBPerlScanner.h"

class CBPerlStyler : public CBStylerBase, public CBPerlScanner
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();

	virtual ~CBPerlStyler();

protected:

	CBPerlStyler();

	virtual void	Scan(istream& input, const TokenExtra& initData);
	virtual void	PreexpandCheckRange(const JString& text,
										const JRunArray<JTextEditor::Font>& styles,
										const JIndexRange& modifiedRange,
										const JBoolean deletion,
										JIndexRange* checkRange);

	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles);

private:

	static CBPerlStyler*	itsSelf;

private:

	void	ExtendCheckRangeForString(const JIndexRange& tokenRange);

	// not allowed

	CBPerlStyler(const CBPerlStyler& source);
	const CBPerlStyler& operator=(const CBPerlStyler& source);
};

#endif
