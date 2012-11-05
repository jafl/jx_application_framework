/******************************************************************************
 CBEiffelStyler.h

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBEiffelStyler
#define _H_CBEiffelStyler

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CBStylerBase.h"
#include "CBEiffelScanner.h"

class CBEiffelStyler : public CBStylerBase, public CBEiffelScanner
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();

	virtual ~CBEiffelStyler();

protected:

	CBEiffelStyler();

	virtual void	Scan(istream& input, const TokenExtra& initData);

	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles);

private:

	static CBEiffelStyler*	itsSelf;

private:

	// not allowed

	CBEiffelStyler(const CBEiffelStyler& source);
	const CBEiffelStyler& operator=(const CBEiffelStyler& source);
};

#endif
