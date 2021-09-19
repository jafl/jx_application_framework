/******************************************************************************
 CBDStyler.h

	Copyright © 2021 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBDStyler
#define _H_CBDStyler

#include "CBStylerBase.h"
#include "CBDScanner.h"

class CBDStyler : public CBStylerBase, public CB::Text::D::Scanner
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();

	virtual ~CBDStyler();

protected:

	CBDStyler();

	virtual void	Scan(const JStyledText::TextIndex& startIndex,
						 std::istream& input, const TokenExtra& initData) override;

	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	static CBDStyler*	itsSelf;

private:

	// not allowed

	CBDStyler(const CBDStyler& source);
	const CBDStyler& operator=(const CBDStyler& source);
};

#endif
