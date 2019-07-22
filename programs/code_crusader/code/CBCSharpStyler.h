/******************************************************************************
 CBCSharpStyler.h

	Copyright (C) 2004 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBCSharpStyler
#define _H_CBCSharpStyler

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

	virtual void	Scan(std::istream& input, const TokenExtra& initData) override;
	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles) override;

private:

	static CBCSharpStyler*	itsSelf;

private:

	JBoolean	SlurpPPComment(JUtf8ByteRange* totalRange);

	// not allowed

	CBCSharpStyler(const CBCSharpStyler& source);
	const CBCSharpStyler& operator=(const CBCSharpStyler& source);
};

#endif
