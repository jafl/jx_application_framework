/******************************************************************************
 CBPropertiesStyler.h

	Copyright (C) 2015 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBPropertiesStyler
#define _H_CBPropertiesStyler

#include "CBStylerBase.h"
#include "CBPropertiesScanner.h"

class CBPropertiesStyler : public CBStylerBase, public CBPropertiesScanner
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();

	virtual ~CBPropertiesStyler();

protected:

	CBPropertiesStyler();

	virtual void	Scan(std::istream& input, const TokenExtra& initData);

	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles);

private:

	static CBPropertiesStyler*	itsSelf;

private:

	// not allowed

	CBPropertiesStyler(const CBPropertiesStyler& source);
	const CBPropertiesStyler& operator=(const CBPropertiesStyler& source);
};

#endif
