/******************************************************************************
 CBJavaScriptStyler.h

	Copyright (C) 2006 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBJavaScriptStyler
#define _H_CBJavaScriptStyler

#include "CBStylerBase.h"
#include "CBJavaScriptScanner.h"

class CBJavaScriptStyler : public CBStylerBase, public CBJavaScriptScanner
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();

	virtual ~CBJavaScriptStyler();

protected:

	CBJavaScriptStyler();

	virtual void	Scan(std::istream& input, const TokenExtra& initData);
	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles);

private:

	static CBJavaScriptStyler*	itsSelf;

private:

	void	StyleEmbeddedVariables(const Token& token);

	// not allowed

	CBJavaScriptStyler(const CBJavaScriptStyler& source);
	const CBJavaScriptStyler& operator=(const CBJavaScriptStyler& source);
};

#endif
