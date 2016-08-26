/******************************************************************************
 CBListChooseFileDialog.h

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBListChooseFileDialog
#define _H_CBListChooseFileDialog

#include <JXChooseFileDialog.h>

class JXRadioGroup;

class CBListChooseFileDialog : public JXChooseFileDialog
{
public:

	static CBListChooseFileDialog*
		Create(JXDirector* supervisor, JDirInfo* dirInfo,
			   const JCharacter* fileFilter,
			   const JBoolean allowSelectMultiple,
			   const JCharacter* replaceListStr, const JCharacter* appendToListStr,
			   const JCharacter* origName, const JCharacter* message);

	virtual ~CBListChooseFileDialog();

	JBoolean	ReplaceExisting() const;

protected:

	CBListChooseFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						   const JCharacter* fileFilter,
						   const JBoolean allowSelectMultiple);

private:

// begin JXLayout

	JXRadioGroup* itsAppendReplaceRG;

// end JXLayout

private:

	void	BuildWindow(const JCharacter* replaceListStr,
						const JCharacter* appendToListStr,
						const JCharacter* origName,
						const JCharacter* message);

	// not allowed

	CBListChooseFileDialog(const CBListChooseFileDialog& source);
	const CBListChooseFileDialog& operator=(const CBListChooseFileDialog& source);
};

#endif
