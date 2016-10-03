/******************************************************************************
 CBRPChooseFileDialog.h

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBRPChooseFileDialog
#define _H_CBRPChooseFileDialog

#include <JXChooseFileDialog.h>
#include "CBRelPathCSF.h"		// need definition of PathType

class JXRadioGroup;

class CBRPChooseFileDialog : public JXChooseFileDialog
{
public:

	static CBRPChooseFileDialog*
		Create(JXDirector* supervisor, JDirInfo* dirInfo,
			   const JCharacter* fileFilter,
			   const JBoolean allowSelectMultiple,
			   const CBRelPathCSF::PathType pathType,
			   const JCharacter* origName, const JCharacter* message);

	virtual ~CBRPChooseFileDialog();

	CBRelPathCSF::PathType	GetPathType() const;

protected:

	CBRPChooseFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						 const JCharacter* fileFilter,
						 const JBoolean allowSelectMultiple);

private:

// begin JXLayout

	JXRadioGroup* itsPathTypeRG;

// end JXLayout

private:

	void	BuildWindow(const CBRelPathCSF::PathType pathType,
						const JCharacter* origName,
						const JCharacter* message);

	// not allowed

	CBRPChooseFileDialog(const CBRPChooseFileDialog& source);
	const CBRPChooseFileDialog& operator=(const CBRPChooseFileDialog& source);
};

#endif
