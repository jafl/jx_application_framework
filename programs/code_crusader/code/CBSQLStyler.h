/******************************************************************************
 CBSQLStyler.h

	Copyright (C) 2017 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBSQLStyler
#define _H_CBSQLStyler

#include "CBStylerBase.h"
#include "CBSQLScanner.h"

class CBSQLStyler : public CBStylerBase, public CBSQLScanner
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();

	virtual ~CBSQLStyler();

protected:

	CBSQLStyler();

	virtual void	Scan(std::istream& input, const TokenExtra& initData);
	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles);

private:

	static CBSQLStyler*	itsSelf;

private:

	// not allowed

	CBSQLStyler(const CBSQLStyler& source);
	const CBSQLStyler& operator=(const CBSQLStyler& source);
};

#endif
