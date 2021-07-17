/******************************************************************************
 GFGMainDirector.h

	Copyright (C) 2002 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_GFGMainDirector
#define _H_GFGMainDirector

#include <JXWindowDirector.h>
#include <JPrefObject.h>

#include <JPtrArray-JString.h>

class JXInputField;
class JXPathInput;
class JXTextButton;
class JXStaticText;
class JXTextMenu;
class JXToolBar;

class GFGClass;
class GFGFunctionTable;

class GFGMainDirector : public JXWindowDirector, public JPrefObject
{
public:

public:

	GFGMainDirector(JXDirector* supervisor, const JPtrArray<JString>& argList);

	virtual	~GFGMainDirector();

protected:

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXTextMenu*	itsFileMenu;
	JXTextMenu*	itsPrefsMenu;
	JXTextMenu*	itsHelpMenu;

	GFGClass*			itsClass;
	GFGFunctionTable*	itsTable;

// begin JXLayout

	JXInputField* itsClassInput;
	JXTextButton* itsChooseButton;
	JXTextButton* itsGenerateButton;
	JXTextButton* itsHelpButton;
	JXTextButton* itsCancelButton;
	JXPathInput*  itsDirInput;
	JXStaticText* itsBaseClassTxt;
	JXInputField* itsAuthorInput;
	JXTextButton* itsStringsButton;
	JXInputField* itsCopyrightInput;

// end JXLayout

private:

	void 		BuildWindow(const JString& outputPath);
	bool 	Write();

	// not allowed

	GFGMainDirector(const GFGMainDirector& source);
	const GFGMainDirector& operator=(const GFGMainDirector& source);
};

#endif
