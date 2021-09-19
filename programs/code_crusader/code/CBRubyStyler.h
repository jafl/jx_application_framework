/******************************************************************************
 CBRubyStyler.h

	Copyright © 2003 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBRubyStyler
#define _H_CBRubyStyler

#include "CBStylerBase.h"
#include "CBRubyScanner.h"

class CBRubyStyler : public CBStylerBase, public CB::Text::Ruby::Scanner
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();

	virtual ~CBRubyStyler();

protected:

	CBRubyStyler();

	virtual void	Scan(const JStyledText::TextIndex& startIndex,
						 std::istream& input, const TokenExtra& initData) override;
	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles) override;

private:

	static CBRubyStyler*	itsSelf;

private:

	void	ExtendCheckRangeForString(const JStyledText::TextRange& tokenRange);
	void	StyleEmbeddedVariables(const Token& token);

	// not allowed

	CBRubyStyler(const CBRubyStyler& source);
	const CBRubyStyler& operator=(const CBRubyStyler& source);
};

#endif
