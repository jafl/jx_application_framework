/******************************************************************************
 JXFSDirMenu.h

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_JXFSDirMenu
#define _H_JXFSDirMenu

#include <jx-af/jx/JXTextMenu.h>

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

	~JXFSDirMenu();

	bool	GetDirInfo(JDirInfo** info);
	void	SetFileList(const JPtrArray<JString>& fullNameList);

	bool	WillShowPath() const;
	void	ShouldShowPath(const bool show);
	bool	WillDereferenceLinks() const;
	void	ShouldDereferenceLinks(const bool dereference);
	bool	WillDeleteBrokenLinks() const;
	void	ShouldDeleteBrokenLinks(const bool deleteBroken);

	void	SetFileIcon(const JXImage& image);
	void	SetFileIcon(const JXPM& data);

	void	SetExecIcon(const JXImage& image);
	void	SetExecIcon(const JXPM& data);

	void	SetEmptyMessage(const JString& msg);

protected:

	JXFSDirMenu(const JString& path,
				JXFSDirMenu* parent, const JIndex itemIndex);

	void	ClearMenu();

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	BroadcastIfTopLevel(const JString& filename);

private:

	JString					itsPath;		// used to construct itsDirInfo
	JDirInfo*				itsDirInfo;		// can be nullptr
	JPtrArray<JDirEntry>*	itsEntries;		// can be nullptr
	JXFSDirMenu*			itsParent;		// can be nullptr; owns us
	JString					itsEmptyMsg;
	bool					itsShowPathFlag;
	bool					itsDereferenceLinksFlag;
	bool					itsDeleteBrokenLinksFlag;

	bool		itsOwnsFileIcon;
	JXImage*	itsFileIcon;
	JXImage*	itsFolderIcon;		// not owned
	bool		itsOwnsExecIcon;
	JXImage*	itsExecIcon;
	JXImage*	itsUnknownIcon;		// not owned

private:

	void	JXFSDirMenuX();
	void	JXFSDirMenuX1();
	void	UpdateSelf();
	void	AppendEntry(const JDirEntry& entry);

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

inline bool
JXFSDirMenu::WillShowPath()
	const
{
	return itsShowPathFlag;
}

inline void
JXFSDirMenu::ShouldShowPath
	(
	const bool show
	)
{
	itsShowPathFlag = show;
}

/******************************************************************************
 Dereference links

 ******************************************************************************/

inline bool
JXFSDirMenu::WillDereferenceLinks()
	const
{
	return itsDereferenceLinksFlag;
}

inline void
JXFSDirMenu::ShouldDereferenceLinks
	(
	const bool dereference
	)
{
	itsDereferenceLinksFlag = dereference;
}

/******************************************************************************
 Delete broken links

 ******************************************************************************/

inline bool
JXFSDirMenu::WillDeleteBrokenLinks()
	const
{
	return itsDeleteBrokenLinksFlag;
}

inline void
JXFSDirMenu::ShouldDeleteBrokenLinks
	(
	const bool deleteBroken
	)
{
	itsDeleteBrokenLinksFlag = deleteBroken;
}

#endif
