/******************************************************************************
 CBStringCompleter.h

	Interface for CBStringCompleter class.

	Copyright (C) 1998 by John Lindal.

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
					  const JSize keywordCount, const JCharacter** keywordList,
					  const JBoolean caseSensitive);

	virtual ~CBStringCompleter();

	void	Reset();
	void	Add(const JCharacter* str);
//	void	Remove(const JCharacter* str);
	void	RemoveAll();

	JBoolean	Complete(JTextEditor* te, JXStringCompletionMenu* menu);

protected:

	virtual void	UpdateWordList();
	void			CopyWordsFromStyler(CBStylerBase* styler);
	void			CopySymbolsForLanguage(const CBLanguage lang);

	virtual JBoolean	IsWordCharacter(const JString& s, const JIndex index,
										const JBoolean includeNS) const = 0;
	virtual void		MatchCase(const JString& source, JString* target) const;
	virtual void		Receive(JBroadcaster* sender, const Message& message) override;

private:

	const CBLanguage	itsLanguage;
	const JSize			itsPredefKeywordCount;
	const JCharacter**	itsPrefefKeywordList;
	const JBoolean		itsCaseSensitiveFlag;
	JPtrArray<JString>*	itsStringList;	// contents not owned
	JPtrArray<JString>*	itsOwnedList;
	CBStylerBase*		itsStyler;		// can be nullptr; not owned; provides extra words

private:

	void		Add(JString* s);
	JBoolean	Complete(JTextEditor* te, const JBoolean includeNS,
						 JXStringCompletionMenu* menu);
	JSize		Complete(const JString& prefix, JString* maxPrefix,
						 JXStringCompletionMenu* menu) const;

	// not allowed

	CBStringCompleter(const CBStringCompleter& source);
	const CBStringCompleter& operator=(const CBStringCompleter& source);
};

#endif
