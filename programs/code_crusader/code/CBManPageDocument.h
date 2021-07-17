/******************************************************************************
 CBManPageDocument.h

	Interface for the CBManPageDocument class

	Copyright © 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBManPageDocument
#define _H_CBManPageDocument

#include "CBTextDocument.h"

class CBManPageDocument : public CBTextDocument
{
public:

	static bool	Create(CBManPageDocument** doc, const JString& pageName,
						   const JString& pageIndex = JString::empty,
						   const bool apropos = false);

	virtual ~CBManPageDocument();

protected:

	CBManPageDocument(const JString& pageName, const JString& pageIndex,
					  const bool apropos, CBManPageDocument** trueDoc);

	static JString	GetCmd1(const JString& pageName, const JString& pageIndex);
	static JString	GetCmd2(const JString& pageName);

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	bool	itsIgnoreNameChangedFlag;

	static JPtrArray<JString>			theManCmdList;
	static JPtrArray<CBTextDocument>	theManDocList;

private:

	static void	RemoveFromManPageList(CBManPageDocument* doc);

	// not allowed

	CBManPageDocument(const CBManPageDocument& source);
	const CBManPageDocument& operator=(const CBManPageDocument& source);
};

#endif
