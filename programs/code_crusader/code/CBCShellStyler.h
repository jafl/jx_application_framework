/******************************************************************************
 CBCShellStyler.h

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBCShellStyler
#define _H_CBCShellStyler

#include "CBStylerBase.h"
#include "CBCShellScanner.h"

class CBCShellStyler : public CBStylerBase, public CB::Text::CShell::Scanner
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();

	virtual ~CBCShellStyler();

protected:

	CBCShellStyler();

	virtual void	Scan(const JStyledText::TextIndex& startIndex,
						 std::istream& input, const TokenExtra& initData) override;
	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles) override;

private:

	static CBCShellStyler*	itsSelf;

private:

	void	ExtendCheckRangeForString(const JStyledText::TextRange& tokenRange);
	void	StyleEmbeddedVariables(const Token& token);

	// not allowed

	CBCShellStyler(const CBCShellStyler& source);
	const CBCShellStyler& operator=(const CBCShellStyler& source);
};

#endif
