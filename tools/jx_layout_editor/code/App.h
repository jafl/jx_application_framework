/******************************************************************************
 App.h

	Copyright (C) 2023 by John Lindal.

 *****************************************************************************/

#ifndef _H_App
#define _H_App

#include <jx-af/jx/JXApplication.h>

class JXMenuBar;
class JXTextMenu;

class App : public JXApplication
{
public:

	App(int* argc, char* argv[], bool* displayAbout, JString* prevVersStr);

	~App() override;

	void	DisplayAbout(const bool showLicense = false,
						 const JString& prevVersStr = JString::empty);

	JXTextMenu*	CreateHelpMenu(JXMenuBar* menuBar, const JUtf8Byte* sectionName);

	static const JUtf8Byte*	GetAppSignature();
	static void				InitStrings();

protected:

	void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason) override;

private:

	void	HandleHelpMenu(const JUtf8Byte* windowSectionName, const JIndex index);
};

#endif
