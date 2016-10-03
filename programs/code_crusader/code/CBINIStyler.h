/******************************************************************************
 CBINIStyler.h

	Copyright (C) 2013 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBINIStyler
#define _H_CBINIStyler

#include "CBStylerBase.h"
#include "CBINIScanner.h"

class CBINIStyler : public CBStylerBase, public CBINIScanner
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();

	virtual ~CBINIStyler();

protected:

	CBINIStyler();

	virtual void	Scan(istream& input, const TokenExtra& initData);

	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles);

private:

	static CBINIStyler*	itsSelf;

private:

	void	ExtendCheckRangeForString(const JIndexRange& tokenRange);

	// not allowed

	CBINIStyler(const CBINIStyler& source);
	const CBINIStyler& operator=(const CBINIStyler& source);
};

#endif
