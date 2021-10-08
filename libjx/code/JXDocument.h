/******************************************************************************
 JXDocument.h

	Interface for the JXDocument class

	Copyright (C) 1996-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDocument
#define _H_JXDocument

#include "jx-af/jx/JXWindowDirector.h"
#include "jx-af/jx/JXDocumentManager.h"	// need definition of SafetySaveReason

class JXDocument : public JXWindowDirector
{
public:

	JXDocument(JXDirector* supervisor);

	~JXDocument();

	bool	Close() override;
	bool	Deactivate() override;

	bool			GetMenuIcon(const JXImage** icon) const override;
	bool			NeedsSave() const override;
	virtual bool	NeedDocument(JXDocument* doc) const;

	void	RevertToSaved();

	virtual void	SafetySave(const JXDocumentManager::SafetySaveReason reason) = 0;

protected:

	virtual bool	OKToClose()      = 0;
	virtual bool	OKToRevert()     = 0;
	virtual bool	CanRevert()      = 0;
	virtual void	DiscardChanges() = 0;
	bool			OKToDeactivate() override;
};

#endif
