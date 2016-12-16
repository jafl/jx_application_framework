/******************************************************************************
 CBPerlStyler.h

	Copyright (C) 2003 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBPerlStyler
#define _H_CBPerlStyler

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

	virtual void	Scan(std::istream& input, const TokenExtra& initData);
	virtual void	PreexpandCheckRange(const JString& text,
										const JRunArray<JFont>& styles,
										const JIndexRange& modifiedRange,
										const JBoolean deletion,
										JIndexRange* checkRange);

	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles);

private:

	static CBPerlStyler*	itsSelf;

private:

	// not allowed

	CBPerlStyler(const CBPerlStyler& source);
	const CBPerlStyler& operator=(const CBPerlStyler& source);
};

#endif
