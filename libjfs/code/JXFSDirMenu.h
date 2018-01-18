/******************************************************************************
 JXFSDirMenu.h

	Copyright (C) 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_JXFSDirMenu
#define _H_JXFSDirMenu

#include <JXTextMenu.h>

class JDirInfo;
class JDirEntry;
class JXImage;

class JXFSDirMenu : public JXTextMenu
{
public:

	JXFSDirMenu(const JCharacter* path,
				const JCharacter* title, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	JXFSDirMenu(const JCharacter* path,
				JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	JXFSDirMenu(const JPtrArray<JString>& fileNameList,
				const JCharacter* title, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	JXFSDirMenu(const JPtrArray<JString>& fileNameList,
				JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	virtual	~JXFSDirMenu();

	JBoolean	GetDirInfo(JDirInfo** info);
	void		SetFileList(const JPtrArray<JString>& fullNameList);

	JBoolean	WillShowPath() const;
	void		ShouldShowPath(const JBoolean show);
	JBoolean	WillDereferenceLinks() const;
	void		ShouldDereferenceLinks(const JBoolean dereference);
	JBoolean	WillDeleteBrokenLinks() const;
	void		ShouldDeleteBrokenLinks(const JBoolean deleteBroken);

	void	SetFileIcon(const JXImage& image);
	void	SetFileIcon(const JXPM& data);

	void	SetExecIcon(const JXImage& image);
	void	SetExecIcon(const JXPM& data);

	void	SetEmptyMessage(const JCharacter* msg);

protected:

	JXFSDirMenu(const JCharacter* path,
				JXFSDirMenu* parent, const JIndex itemIndex);

	void	ClearMenu();

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	void			BroadcastIfTopLevel(const JString& filename);

private:

	JString					itsPath;		// used to construct itsDirInfo
	JDirInfo*				itsDirInfo;		// can be NULL
	JPtrArray<JDirEntry>*	itsEntries;		// can be NULL
	JXFSDirMenu*			itsParent;		// can be NULL; owns us
	JString					itsEmptyMsg;
	JBoolean				itsShowPathFlag;
	JBoolean				itsDereferenceLinksFlag;
	JBoolean				itsDeleteBrokenLinksFlag;

	JXImage*	itsFileIcon;
	JXImage*	itsFolderIcon;
	JXImage*	itsExecIcon;
	JXImage*	itsUnknownIcon;

private:

	void	JXFSDirMenuX();
	void	JXFSDirMenuX1();
	void	UpdateSelf();
	void	AppendEntry(const JDirEntry& entry);

	// not allowed

	JXFSDirMenu(const JXFSDirMenu& source);
	const JXFSDirMenu& operator=(const JXFSDirMenu& source);

public:

	static const JCharacter* kFileSelected;

	class FileSelected : public JBroadcaster::Message
		{
		public:

			FileSelected(const JString& filename)
				:
				JBroadcaster::Message(kFileSelected),
				itsFile(filename)
				{ };

			const JString&
			GetFileName() const
				{
				return itsFile;
				};

		private:

			const JString& itsFile;
		};
};


/******************************************************************************
 Show path (in shortcut column)

 ******************************************************************************/

inline JBoolean
JXFSDirMenu::WillShowPath()
	const
{
	return itsShowPathFlag;
}

inline void
JXFSDirMenu::ShouldShowPath
	(
	const JBoolean show
	)
{
	itsShowPathFlag = show;
}

/******************************************************************************
 Dereference links

 ******************************************************************************/

inline JBoolean
JXFSDirMenu::WillDereferenceLinks()
	const
{
	return itsDereferenceLinksFlag;
}

inline void
JXFSDirMenu::ShouldDereferenceLinks
	(
	const JBoolean dereference
	)
{
	itsDereferenceLinksFlag = dereference;
}

/******************************************************************************
 Delete broken links

 ******************************************************************************/

inline JBoolean
JXFSDirMenu::WillDeleteBrokenLinks()
	const
{
	return itsDeleteBrokenLinksFlag;
}

inline void
JXFSDirMenu::ShouldDeleteBrokenLinks
	(
	const JBoolean deleteBroken
	)
{
	itsDeleteBrokenLinksFlag = deleteBroken;
}

#endif
