/******************************************************************************
 CBSymbolSRDirector.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBSymbolSRDirector
#define _H_CBSymbolSRDirector

#include <JXWindowDirector.h>

class JError;
class JXTextButton;
class JXTextMenu;
class CBSymbolList;
class CBSymbolTable;
class CBProjectDocument;
class CBSymbolDirector;
class CBCommandMenu;

class CBSymbolSRDirector : public JXWindowDirector
{
public:

	CBSymbolSRDirector(CBSymbolDirector* mainSym, CBProjectDocument* projDoc,
					   CBSymbolList* symbolList,
					   const JString& filterStr, const JBoolean isRegex,
					   JError* err);
	CBSymbolSRDirector(CBSymbolDirector* mainSym, CBProjectDocument* projDoc,
					   CBSymbolList* symbolList,
					   const JArray<JIndex>& displayList, const JString& searchStr);

	virtual ~CBSymbolSRDirector();

	CBSymbolList*	GetSymbolList() const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveWithFeedback(JBroadcaster* sender, Message* message) override;

private:

	CBSymbolDirector*	itsMainSymDir;		// owns us
	CBProjectDocument*	itsProjDoc;			// not owned
	CBSymbolList*		itsSymbolList;		// not owned
	CBSymbolTable*		itsSymbolTable;
	JXTextMenu*			itsActionsMenu;
	CBCommandMenu*		itsCmdMenu;

// begin JXLayout


// end JXLayout

private:

	void	CBSymbolSRDirectorX(CBSymbolDirector* owner,
								CBProjectDocument* projDoc,
								CBSymbolList* symbolList,
								const JBoolean focus);
	void	BuildWindow(CBSymbolList* symbolList, const JBoolean focus);

	void	UpdateActionsMenu();
	void	HandleActionsMenu(const JIndex index);

	// not allowed

	CBSymbolSRDirector(const CBSymbolSRDirector& source);
	const CBSymbolSRDirector& operator=(const CBSymbolSRDirector& source);
};


/******************************************************************************
 GetSymbolList

 ******************************************************************************/

inline CBSymbolList*
CBSymbolSRDirector::GetSymbolList()
	const
{
	return itsSymbolList;
}

#endif
