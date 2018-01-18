/******************************************************************************
 GNBNoteDir.h

	Interface for the GNBNoteDir class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_GNBNoteDir
#define _H_GNBNoteDir

#include <JXDocument.h>

#include <JFileArray.h>
#include <JString.h>
#include <JXPM.h>
#include <iostream.h>

class GNBTreeDir;
class GNBNoteEditor;
class JXTextMenu;
class JXMenuBar;
class JXToolBar;
//class std::ostream;
//class std::istream;
class JXTextButton;

class GNBNoteDir : public JXDocument
{
public:

	GNBNoteDir(GNBTreeDir* supervisor, const JFAID_t id);
	GNBNoteDir(GNBTreeDir* supervisor);

	virtual ~GNBNoteDir();
	
	void				Print();
	void				Export();

	virtual JBoolean	NeedsSave() const;
	virtual void		SafetySave(const JXDocumentManager::SafetySaveReason reason);
	virtual JXPM		GetMenuIcon() const;
	JBoolean			TextChanged() const;
	JFAID_t				GetID() const;
	void				Append(const JCharacter* text);
	void				Append(const JFAID_t id);
	void				PrintFinished();
	void				MakeTempForPrinting();

protected:

	virtual void		Receive(JBroadcaster* sender, const Message& message) override;
	virtual JBoolean	OKToClose();
	virtual JBoolean	OKToRevert();
	virtual JBoolean	CanRevert();
	virtual void		DiscardChanges();

private:

	GNBNoteEditor*	 		itsEditor;
	JXTextMenu*				itsFileMenu;
	JXMenuBar*				itsMenuBar;
	JBoolean				itsTextChanged;
	JFAID_t					itsID;
	JString					itsTitle;
	JXTextButton*			itsCloseButton;
	JXToolBar*				itsToolBar;
	JBoolean				itsIsTempForPrinting;
	GNBTreeDir*				itsDir;

private:

	void Save(const JBoolean tellApp = kJFalse);

	void BuildWindow();
	void UpdateFileMenu();
	void HandleFileMenu(const JIndex index);

	// not allowed

	GNBNoteDir(const GNBNoteDir& source);
	const GNBNoteDir& operator=(const GNBNoteDir& source);
	
};

/******************************************************************************
 TextChanged (public)

 ******************************************************************************/

inline JBoolean
GNBNoteDir::TextChanged()
	const
{
	return itsTextChanged;
}

/******************************************************************************
 TextChanged (public)

 ******************************************************************************/

inline JFAID_t
GNBNoteDir::GetID()
	const
{
	return itsID;
}

#endif
