/******************************************************************************
 CBTCLStyler.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBTCLStyler
#define _H_CBTCLStyler

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

	virtual void	Scan(std::istream& input, const TokenExtra& initData) override;
	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles) override;

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
