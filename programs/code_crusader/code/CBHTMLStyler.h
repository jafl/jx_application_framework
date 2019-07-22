/******************************************************************************
 CBHTMLStyler.h

	Copyright (C) 2001-2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBHTMLStyler
#define _H_CBHTMLStyler

#include "CBStylerBase.h"
#include "CBHTMLScanner.h"

class CBHTMLStyler : public CBStylerBase, public CBHTMLScanner
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();

	virtual ~CBHTMLStyler();

protected:

	CBHTMLStyler();

	virtual void		Scan(std::istream& input, const TokenExtra& initData) override;
	virtual TokenExtra	GetFirstTokenExtraData() const override;
	virtual void		UpgradeTypeList(const JFileVersion vers,
										JArray<JFontStyle>* typeStyles) override;

	virtual const JString&	GetScannedText() const override;

private:

	JString	itsLatestTagName;

	static CBHTMLStyler*	itsSelf;

private:

	void		InitMustacheTypeStyles();
	void		InitPHPTypeStyles();
	void		InitJSPTypeStyles();
	void		InitJavaScriptTypeStyles();
	void		ExtendCheckRangeForString(const JUtf8ByteRange& tokenRange);
	void		ExtendCheckRangeForPHPStartEnd(const JUtf8ByteRange& tokenRange);
	void		ExtendCheckRangeForJSPStartEnd(const JUtf8ByteRange& tokenRange);
	JFontStyle	GetTagStyle(const JUtf8ByteRange& tokenRange, const JIndex typeIndex);
	JBoolean	GetXMLStyle(const JString& tagName, JFontStyle* style);
	void		StyleEmbeddedPHPVariables(const Token& token);
	void		StyleEmbeddedJSVariables(const Token& token);

	// not allowed

	CBHTMLStyler(const CBHTMLStyler& source);
	const CBHTMLStyler& operator=(const CBHTMLStyler& source);
};

#endif
