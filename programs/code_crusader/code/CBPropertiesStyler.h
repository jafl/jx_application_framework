/******************************************************************************
 CBPropertiesStyler.h

	Copyright © 2015 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBPropertiesStyler
#define _H_CBPropertiesStyler

#include "CBStylerBase.h"
#include "CBPropertiesScanner.h"

class CBPropertiesStyler : public CBStylerBase, public CB::Text::Properties::Scanner
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();

	virtual ~CBPropertiesStyler();

protected:

	CBPropertiesStyler();

	virtual void	Scan(const JStyledText::TextIndex& startIndex,
						 std::istream& input, const TokenExtra& initData) override;
	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles) override;

private:

	static CBPropertiesStyler*	itsSelf;

private:

	// not allowed

	CBPropertiesStyler(const CBPropertiesStyler& source);
	const CBPropertiesStyler& operator=(const CBPropertiesStyler& source);
};

#endif
