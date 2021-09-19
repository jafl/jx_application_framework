/******************************************************************************
 CBBourneShellStyler.h

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBBourneShellStyler
#define _H_CBBourneShellStyler

#include "CBStylerBase.h"
#include "CBBourneShellScanner.h"

class CBBourneShellStyler : public CBStylerBase, public CB::Text::BourneShell::Scanner
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();

	virtual ~CBBourneShellStyler();

protected:

	CBBourneShellStyler();

	virtual void	Scan(const JStyledText::TextIndex& startIndex,
						 std::istream& input, const TokenExtra& initData) override;
	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles) override;

private:

	static CBBourneShellStyler*	itsSelf;

private:

	void	ExtendCheckRangeForString(const JStyledText::TextRange& tokenRange);
	void	StyleEmbeddedVariables(const Token& token);

	// not allowed

	CBBourneShellStyler(const CBBourneShellStyler& source);
	const CBBourneShellStyler& operator=(const CBBourneShellStyler& source);
};

#endif
