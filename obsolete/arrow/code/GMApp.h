/******************************************************************************
 GMApp.h

	Interface for the GMApp class

	Copyright (C) 1997 by Glenn W. Bach.  All rights reserved.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#ifndef _H_GMApp
#define _H_GMApp

#include <JXApplication.h>
#include <JString.h>

class GMessageTableDir;
class GMessageEditDir;
class GLockFileTask;
class GMailboxTreeDir;
class GAddressBookTreeDir;
class GMessageHeader;
class GMAboutDialog;

class GMApp : public JXApplication
{
public:

	GMApp(int* argc, char* argv[], JBoolean* displayAbout, JString* prevVersStr);
	virtual ~GMApp();

	virtual void	Suspend() override;
	virtual void	Resume() override;

	void	OpenMailbox();
	void	OpenMailbox(const JCharacter* filename, const JBoolean beep = kJFalse, const JBoolean iconify = kJFalse);
	void	OpenIconifiedMailbox(const JCharacter* filename,
									const JBoolean beep = kJTrue);
	JBoolean	NewMailbox(const JCharacter* filename, const JBoolean openFile = kJTrue);
	void		OpenSystemMailbox();
	JBoolean	MailboxOpen(const JCharacter* filename, const JBoolean activate);

	void	AppendNewMessage(GMessageEditDir* dir);

	void	DisplayAbout(const JCharacter* prevVersStr = NULL);
	virtual JBoolean Close();

	void	CheckMail();

	JBoolean	OpenSession();

	void		NewMessage(const JCharacter* to);

	JBoolean	HasFileDirectory() const;
	JBoolean	GetFileDirectory(JString* dir);

	void		HeaderStateChanged(GMessageHeader* header);

	void		BroadcastMailboxClosed(const JCharacter* mbox);

	void		OpenMailboxWindowIfNeeded();

	static const JCharacter*	GetAppSignature();
	static void					InitStrings();

protected:

	virtual void	Receive(JBroadcaster* sender,
								const JBroadcaster::Message& message);
	virtual void	DirectorClosed(JXDirector* theDirector);
	virtual void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

private:

	JPtrArray<GMessageTableDir>*	itsTableDirs;
	JPtrArray<GMessageEditDir>*		itsEditDirs;
	JPtrArray<GLockFileTask>*		itsLockTasks;

	JString			itsFileDirectory;
	JBoolean		itsHasFileDirectory;
	JBoolean		itsOpenPrefsAfterAbout;
	JBoolean		itsPrefsNew;
	GMAboutDialog*	itsAboutDialog;

private:

	void		SaveState();
	JBoolean	FileLocked(const JCharacter* filename, const JBoolean iconify);

	// not allowed

	GMApp(const GMApp& source);
	const GMApp& operator=(const GMApp& source);

public:

	static const JCharacter* kMailboxOpened;
	static const JCharacter* kMailboxClosed;

	class MailboxOpened : public JBroadcaster::Message
		{
		public:
			MailboxOpened(const JString& filename)
			:
			JBroadcaster::Message(kMailboxOpened),
			itsMBox(filename)
			{ };

			const JString&
			GetMailbox() const
			{
				return itsMBox;
			};

		private:

			const JString& itsMBox;
		};

	class MailboxClosed : public JBroadcaster::Message
		{
		public:
			MailboxClosed(const JString& filename)
			:
			JBroadcaster::Message(kMailboxClosed),
			itsMBox(filename)
			{ };

			const JString&
			GetMailbox() const
			{
				return itsMBox;
			};

		private:

			const JString& itsMBox;
		};
};

/******************************************************************************
 HasFileDirectory

 ******************************************************************************/

inline JBoolean
GMApp::HasFileDirectory()
	const
{
	return itsHasFileDirectory;
}

#endif
