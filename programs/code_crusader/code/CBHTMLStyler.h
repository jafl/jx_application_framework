/******************************************************************************
 CBHTMLStyler.h

	Copyright © 2001-2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBHTMLStyler
#define _H_CBHTMLStyler

#include "CBStylerBase.h"
#include "CBHTMLScanner.h"

class CBHTMLStyler : public CBStylerBase, public CB::Text::HTML::Scanner
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();

	virtual ~CBHTMLStyler();

protected:

	CBHTMLStyler();

	virtual void		Scan(const JStyledText::TextIndex& startIndex,
							 std::istream& input, const TokenExtra& initData) override;
	virtual void		UpgradeTypeList(const JFileVersion vers,
										JArray<JFontStyle>* typeStyles) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

	virtual const JString&	GetScannedText() const override;

private:

	JString	itsLatestTagName;

	static CBHTMLStyler*	itsSelf;

private:

	void		InitMustacheTypeStyles();
	void		InitPHPTypeStyles();
	void		InitJSPTypeStyles();
	void		InitJavaScriptTypeStyles();
	void		ExtendCheckRangeForString(const JCharacterRange& tokenRange);
	void		ExtendCheckRangeForLanguageStartEnd(const TokenType tokenType, const JCharacterRange& tokenRange);
	JFontStyle	GetTagStyle(const JUtf8ByteRange& tokenRange, const JIndex typeIndex);
	bool		GetXMLStyle(const JString& tagName, JFontStyle* style);
	void		StyleEmbeddedPHPVariables(const Token& token);
	void		StyleEmbeddedJSVariables(const Token& token);

	static JCharacterRange	MatchAt(const Token& token, JStringIterator& iter,
									const JRegex& pattern);

	// not allowed

	CBHTMLStyler(const CBHTMLStyler& source);
	const CBHTMLStyler& operator=(const CBHTMLStyler& source);
};

#endif
