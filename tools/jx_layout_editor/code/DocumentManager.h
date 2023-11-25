/******************************************************************************
 DocumentManager.h

	Copyright © 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_DocumentManager
#define _H_DocumentManager

#include <jx-af/jx/JXDocumentManager.h>
#include <jx-af/jcore/JPrefObject.h>

class JXWindow;
class FileHistoryMenu;

class DocumentManager : public JXDocumentManager, public JPrefObject
{
public:

	DocumentManager();

	~DocumentManager() override;

	void	AddToFileHistoryMenu(const JString& fullName);

	// called by CreateGlobals()

	void	CreateFileHistoryMenu(JXWindow* window);

	// called by FileHistoryMenu

	FileHistoryMenu*	GetFileHistoryMenu() const;

protected:

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

private:

	FileHistoryMenu*	itsRecentFileMenu;

public:

	// JBroadcaster messages

	static const JUtf8Byte* kAddFileToHistory;

	class AddFileToHistory : public JBroadcaster::Message
	{
	public:

		AddFileToHistory(const JString& fullName)
			:
			JBroadcaster::Message(kAddFileToHistory),
			itsFullName(fullName)
			{ };

		const JString&
		GetFullName() const
		{
			return itsFullName;
		}

	private:

		const JString&	itsFullName;
	};
};


/******************************************************************************
 GetFileHistoryMenu

	Called by FileHistoryMenu.

 ******************************************************************************/

inline FileHistoryMenu*
DocumentManager::GetFileHistoryMenu()
	const
{
	return itsRecentFileMenu;
}

#endif
