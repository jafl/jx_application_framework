/******************************************************************************
 GMChooseFileDialog.h

	Copyright © 1999 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GMChooseFileDialog
#define _H_GMChooseFileDialog

#include <JXChooseFileDialog.h>

class JXTextCheckbox;

class GMChooseFileDialog : public JXChooseFileDialog
{
public:

	GMChooseFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						   const JCharacter* fileFilter,
						   const JBoolean allowSelectMultiple);

	virtual ~GMChooseFileDialog();

	void	BuildWindow(const JCharacter* origName, const JCharacter* message = NULL);

	void		ShouldSaveHeaders(const JBoolean save);
	JBoolean	IsSavingHeaders() const;

private:

// begin JXLayout

    JXTextCheckbox* itsSaveHeadersCB;

// end JXLayout

private:

	// not allowed

	GMChooseFileDialog(const GMChooseFileDialog& source);
	const GMChooseFileDialog& operator=(const GMChooseFileDialog& source);
};

#endif
