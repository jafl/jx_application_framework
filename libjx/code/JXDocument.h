/******************************************************************************
 JXDocument.h

	Interface for the JXDocument class

	Copyright (C) 1996-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDocument
#define _H_JXDocument

#include <JXWindowDirector.h>
#include <JXDocumentManager.h>	// need definition of SafetySaveReason

class JXFileDocument;

class JXDocument : public JXWindowDirector
{
public:

	JXDocument(JXDirector* supervisor);

	virtual ~JXDocument();

	virtual JBoolean	Close();
	virtual JBoolean	Deactivate();

	virtual JBoolean	GetMenuIcon(const JXImage** icon) const;
	virtual JBoolean	NeedsSave() const;
	virtual JBoolean	NeedDocument(JXDocument* doc) const;

	void	RevertToSaved();

	virtual void		SafetySave(const JXDocumentManager::SafetySaveReason reason) = 0;

protected:

	virtual JBoolean	OKToClose()      = 0;
	virtual JBoolean	OKToRevert()     = 0;
	virtual JBoolean	CanRevert()      = 0;
	virtual void		DiscardChanges() = 0;
	virtual JBoolean	OKToDeactivate();

private:

	// not allowed

	JXDocument(const JXDocument& source);
	const JXDocument& operator=(const JXDocument& source);
};

#endif
