/******************************************************************************
 CBRPChoosePathDialog.h

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBRPChoosePathDialog
#define _H_CBRPChoosePathDialog

#include <JXChoosePathDialog.h>
#include "CBRelPathCSF.h"		// need definition of PathType

class JXRadioGroup;

class CBRPChoosePathDialog : public JXChoosePathDialog
{
public:

	static CBRPChoosePathDialog*
		Create(JXDirector* supervisor, JDirInfo* dirInfo,
			   const JCharacter* fileFilter,
			   const JBoolean selectOnlyWritable,
			   const CBRelPathCSF::PathType pathType,
			   const JCharacter* message = NULL);

	virtual ~CBRPChoosePathDialog();

	CBRelPathCSF::PathType	GetPathType() const;

protected:

	CBRPChoosePathDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						 const JCharacter* fileFilter,
						 const JBoolean selectOnlyWritable);

private:

// begin JXLayout

	JXRadioGroup* itsPathTypeRG;

// end JXLayout

private:

	void	BuildWindow(const CBRelPathCSF::PathType pathType,
						const JCharacter* message = NULL);

	// not allowed

	CBRPChoosePathDialog(const CBRPChoosePathDialog& source);
	const CBRPChoosePathDialog& operator=(const CBRPChoosePathDialog& source);
};

#endif
