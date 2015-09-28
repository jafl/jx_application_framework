/******************************************************************************
 CBPropertiesStyler.h

	Copyright © 2015 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBPropertiesStyler
#define _H_CBPropertiesStyler

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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

	virtual void	Scan(istream& input, const TokenExtra& initData);

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
