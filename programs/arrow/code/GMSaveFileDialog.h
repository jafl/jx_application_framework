/******************************************************************************
 GMSaveFileDialog.h

	Interface for the GMSaveFileDialog class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_GMSaveFileDialog
#define _H_GMSaveFileDialog

#include <JXSaveFileDialog.h>

class JXTextCheckbox;

class GMSaveFileDialog : public JXSaveFileDialog
{
public:

	GMSaveFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						const JCharacter* fileFilter,
						const JCharacter* origName,
						const JCharacter* prompt,
						const JCharacter* message = NULL);

	virtual ~GMSaveFileDialog();

	void		ShouldSaveHeaders(const JBoolean save);
	JBoolean	IsSavingHeaders() const;

private:

// begin JXLayout

    JXTextCheckbox* itsShowHeadersCB;

// end JXLayout

private:

	void	BuildWindow(const JCharacter* origName, const JCharacter* prompt,
						const JCharacter* message = NULL);

	// not allowed

	GMSaveFileDialog(const GMSaveFileDialog& source);
	const GMSaveFileDialog& operator=(const GMSaveFileDialog& source);
};

#endif
