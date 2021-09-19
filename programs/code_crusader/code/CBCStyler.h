/******************************************************************************
 CBCStyler.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBCStyler
#define _H_CBCStyler

#include "CBStylerBase.h"
#include "CBCScanner.h"

class CBCStyler : public CBStylerBase, public CB::Text::C::Scanner
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();

	virtual ~CBCStyler();

protected:

	CBCStyler();

	virtual void	Scan(const JStyledText::TextIndex& startIndex,
						 std::istream& input, const TokenExtra& initData) override;
	virtual void	PreexpandCheckRange(const JString& text,
										const JRunArray<JFont>& styles,
										const JCharacterRange& modifiedRange,
										const bool deletion,
										JStyledText::TextRange* checkRange) override;

	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	static CBCStyler*	itsSelf;

private:

	bool	SlurpPPComment(JStyledText::TextRange* totalRange);

	// not allowed

	CBCStyler(const CBCStyler& source);
	const CBCStyler& operator=(const CBCStyler& source);
};

#endif
