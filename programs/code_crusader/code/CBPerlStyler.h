/******************************************************************************
 CBPerlStyler.h

	Copyright © 2003 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBPerlStyler
#define _H_CBPerlStyler

#include "CBStylerBase.h"
#include "CBPerlScanner.h"

class CBPerlStyler : public CBStylerBase, public CB::Text::Perl::Scanner
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();

	virtual ~CBPerlStyler();

protected:

	CBPerlStyler();

	virtual void	Scan(const JStyledText::TextIndex& startIndex,
						 std::istream& input, const TokenExtra& initData) override;
	virtual void	PreexpandCheckRange(const JString& text,
										const JRunArray<JFont>& styles,
										const JCharacterRange& modifiedRange,
										const bool deletion,
										JStyledText::TextRange* checkRange) override;

	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles) override;

private:

	static CBPerlStyler*	itsSelf;

private:

	// not allowed

	CBPerlStyler(const CBPerlStyler& source);
	const CBPerlStyler& operator=(const CBPerlStyler& source);
};

#endif
