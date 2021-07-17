/******************************************************************************
 CBStringCompleter.h

	Interface for CBStringCompleter class.

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBStringCompleter
#define _H_CBStringCompleter

#include "CBTextFileType.h"
#include <JPtrArray-JString.h>

class JTextEditor;
class CBStylerBase;
class JXStringCompletionMenu;

class CBStringCompleter : virtual public JBroadcaster
{
public:

	CBStringCompleter(const CBLanguage lang,
					  const JSize keywordCount, const JUtf8Byte** keywordList,
					  const JString::Case caseSensitive);

	virtual ~CBStringCompleter();

	void	Reset();
	void	Add(const JString& str);
	void	RemoveAll();

	bool	Complete(JTextEditor* te, JXStringCompletionMenu* menu);

protected:

	virtual void	UpdateWordList();
	void			CopyWordsFromStyler(CBStylerBase* styler);
	void			CopySymbolsForLanguage(const CBLanguage lang);

	virtual bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const = 0;
	virtual void		MatchCase(const JString& source, JString* target) const;
	virtual void		Receive(JBroadcaster* sender, const Message& message) override;

private:

	const CBLanguage	itsLanguage;
	const JSize			itsPredefKeywordCount;
	const JUtf8Byte**	itsPrefefKeywordList;
	const JString::Case	itsCaseSensitiveFlag;
	JPtrArray<JString>*	itsStringList;	// contents not owned
	JPtrArray<JString>*	itsOwnedList;
	CBStylerBase*		itsStyler;		// can be nullptr; not owned; provides extra words

private:

	void		Add(JString* s);
	bool	Complete(JTextEditor* te, const bool includeNS,
						 JXStringCompletionMenu* menu);
	JSize		Complete(const JString& prefix, JString* maxPrefix,
						 JXStringCompletionMenu* menu) const;

	// not allowed

	CBStringCompleter(const CBStringCompleter& source);
	const CBStringCompleter& operator=(const CBStringCompleter& source);
};

#endif
