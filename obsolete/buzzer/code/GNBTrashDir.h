/******************************************************************************
 GNBTrashDir.h

	Interface for the GNBTrashDir class

	Copyright (C) 1998 by Glenn W. Bach.
	
	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#ifndef _H_GNBTrashDir
#define _H_GNBTrashDir

//includes

// Superclass Header
#include <JXDocument.h>
#include <JXPM.h>

class GNBTrashWidget;
class JTree;
class JFileArray;
class JTreeNode;
class JXTextButton;
class JXToolBar;

class GNBTrashDir : public JXDocument
{
public:

public:

	GNBTrashDir(JXDirector* supervisor);
	virtual ~GNBTrashDir();

	void	DiscardNode(JTreeNode* node);
	void	FlushDiscards();

	virtual JXPM		GetMenuIcon() const;
	virtual JBoolean	NeedsSave() const;
	virtual void		SafetySave(const JXDocumentManager::SafetySaveReason reason);

	void				DataModified();
	void				DataReverted();

protected:

	virtual void		Receive(JBroadcaster* sender, const Message& message) override;
	virtual JBoolean	OKToClose();
	virtual void		DiscardChanges();

private:

	JTree*			itsTree;
	GNBTrashWidget*	itsWidget;
	JBoolean		itsTreeChanged;
	JXTextButton*	itsCloseButton;
	JXToolBar*		itsToolBar;

private:

	void BuildWindow();

	void ReadPrefs();
	void WritePrefs();

	// not allowed

	GNBTrashDir(const GNBTrashDir& source);
	const GNBTrashDir& operator=(const GNBTrashDir& source);

public:

// Broadcaster message types

// Broadcaster message classes
};


#endif
