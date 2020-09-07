/******************************************************************************
 JXFSDirMenu.h

	Copyright (C) 2000 by Glenn W. Bach.

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

	JXFSDirMenu(const JString& path,
				const JString& title, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	JXFSDirMenu(const JString& path,
				JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	JXFSDirMenu(const JPtrArray<JString>& fileNameList,
				const JString& title, JXContainer* enclosure,
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

	void	SetEmptyMessage(const JString& msg);

protected:

	JXFSDirMenu(const JString& path,
				JXFSDirMenu* parent, const JIndex itemIndex);

	void	ClearMenu();

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	void			BroadcastIfTopLevel(const JString& filename);

private:

	JString					itsPath;		// used to construct itsDirInfo
	JDirInfo*				itsDirInfo;		// can be nullptr
	JPtrArray<JDirEntry>*	itsEntries;		// can be nullptr
	JXFSDirMenu*			itsParent;		// can be nullptr; owns us
	JString					itsEmptyMsg;
	JBoolean				itsShowPathFlag;
	JBoolean				itsDereferenceLinksFlag;
	JBoolean				itsDeleteBrokenLinksFlag;

	JBoolean	itsOwnsFileIcon;
	JXImage*	itsFileIcon;
	JXImage*	itsFolderIcon;		// not owned
	JBoolean	itsOwnsExecIcon;
	JXImage*	itsExecIcon;
	JXImage*	itsUnknownIcon;		// not owned

private:

	void	JXFSDirMenuX();
	void	JXFSDirMenuX1();
	void	UpdateSelf();
	void	AppendEntry(const JDirEntry& entry);

	// not allowed

	JXFSDirMenu(const JXFSDirMenu& source);
	const JXFSDirMenu& operator=(const JXFSDirMenu& source);

public:

	static const JUtf8Byte* kFileSelected;

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
