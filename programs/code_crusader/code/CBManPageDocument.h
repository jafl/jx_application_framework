/******************************************************************************
 CBManPageDocument.h

	Interface for the CBManPageDocument class

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBManPageDocument
#define _H_CBManPageDocument

#include "CBTextDocument.h"

class CBManPageDocument : public CBTextDocument
{
public:

	static JBoolean	Create(CBManPageDocument** doc, const JCharacter* pageName,
						   const JCharacter pageIndex = ' ', const JBoolean apropos = kJFalse);

	virtual ~CBManPageDocument();

protected:

	CBManPageDocument(const JCharacter* pageName, const JCharacter pageIndex,
					  const JBoolean apropos, CBManPageDocument** trueDoc);

	static JString	GetCmd1(const JCharacter* pageName, const JCharacter pageIndex);
	static JString	GetCmd2(const JCharacter* pageName);

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JBoolean	itsIgnoreNameChangedFlag;

	static JPtrArray<JString>			theManCmdList;
	static JPtrArray<CBTextDocument>	theManDocList;

private:

	static void	RemoveFromManPageList(CBManPageDocument* doc);

	// not allowed

	CBManPageDocument(const CBManPageDocument& source);
	const CBManPageDocument& operator=(const CBManPageDocument& source);
};

#endif
