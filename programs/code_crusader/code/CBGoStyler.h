/******************************************************************************
 CBGoStyler.h

	Copyright © 2021 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBGoStyler
#define _H_CBGoStyler

#include "CBStylerBase.h"
#include "CBGoScanner.h"

class CBGoStyler : public CBStylerBase, public CB::Text::Go::Scanner
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();

	virtual ~CBGoStyler();

protected:

	CBGoStyler();

	virtual void	Scan(const JStyledText::TextIndex& startIndex,
						 std::istream& input, const TokenExtra& initData) override;
	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	static CBGoStyler*	itsSelf;

private:

	// not allowed

	CBGoStyler(const CBGoStyler& source);
	const CBGoStyler& operator=(const CBGoStyler& source);
};

#endif
