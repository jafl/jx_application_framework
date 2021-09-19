/******************************************************************************
 CBINIStyler.h

	Copyright © 2013 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBINIStyler
#define _H_CBINIStyler

#include "CBStylerBase.h"
#include "CBINIScanner.h"

class CBINIStyler : public CBStylerBase, public CB::Text::INI::Scanner
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();

	virtual ~CBINIStyler();

protected:

	CBINIStyler();

	virtual void	Scan(const JStyledText::TextIndex& startIndex,
						 std::istream& input, const TokenExtra& initData) override;
	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles) override;

private:

	static CBINIStyler*	itsSelf;

private:

	void	ExtendCheckRangeForString(const JStyledText::TextRange& tokenRange);

	// not allowed

	CBINIStyler(const CBINIStyler& source);
	const CBINIStyler& operator=(const CBINIStyler& source);
};

#endif
