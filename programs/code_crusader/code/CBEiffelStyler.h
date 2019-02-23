/******************************************************************************
 CBEiffelStyler.h

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBEiffelStyler
#define _H_CBEiffelStyler

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

	virtual void	Scan(std::istream& input, const TokenExtra& initData) override;
	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles) override;

private:

	static CBEiffelStyler*	itsSelf;

private:

	// not allowed

	CBEiffelStyler(const CBEiffelStyler& source);
	const CBEiffelStyler& operator=(const CBEiffelStyler& source);
};

#endif
