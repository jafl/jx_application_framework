/******************************************************************************
 CBRelPathCSF.h

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBRelPathCSF
#define _H_CBRelPathCSF

#include <JXChooseSaveFile.h>

class CBProjectDocument;
class CBRPChooseFileDialog;
class CBRPChoosePathDialog;

class CBRelPathCSF : public JXChooseSaveFile
{
public:

	// Do not change these values once they are assigned because
	// they are use by CBProjectTable in the prefs file.

	// If you add values, be sure to keep CBProjectTable::DropFileAction up to date.

	enum PathType
	{
		kAbsolutePath = 1,	// avoid conflict with X atom None when calling ChooseDropAction()
		kProjectRelative,
		kHomeRelative
	};

public:

	CBRelPathCSF(CBProjectDocument* doc);

	virtual ~CBRelPathCSF();

	PathType	GetPathType() const;
	void		SetPathType(const PathType type);

	const JString&	GetProjectPath() const;

	JString			ConvertToRelativePath(const JCharacter* fullPath);
	static JString	ConvertToRelativePath(const JCharacter* fullPath,
										  const JCharacter* projPath,
										  const PathType pathType);
	static PathType	CalcPathType(const JCharacter* path);

	JBoolean ChooseRelFile(const JCharacter* prompt,
						   const JCharacter* instructions,		// can be NULL
						   const JCharacter* origName,
						   JString* name);						// relative

	JBoolean ChooseRelRPath(const JCharacter* prompt,
							const JCharacter* instructions,		// can be NULL
							const JCharacter* origPath,			// can be NULL
							JString* newPath);					// relative
	JBoolean ChooseRelRWPath(const JCharacter* prompt,
							 const JCharacter* instructions,	// can be NULL
							 const JCharacter* origPath,		// can be NULL
							 JString* newPath);					// relative

protected:

	virtual JXChooseFileDialog*
	CreateChooseFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						   const JCharacter* fileFilter,
						   const JBoolean allowSelectMultiple,
						   const JCharacter* origName, const JCharacter* message);

	virtual JXChoosePathDialog*
	CreateChoosePathDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						   const JCharacter* fileFilter,
						   const JBoolean selectOnlyWritable, const JCharacter* message);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	CBRPChooseFileDialog*	itsFileDialog;
	CBRPChoosePathDialog*	itsPathDialog;
	PathType				itsPathType;
	CBProjectDocument*		itsDoc;

private:

	JString	PrepareForChoose(const JCharacter* origName);
	void	CalcPathType(const JCharacter* path, PathType* type) const;

	// not allowed

	CBRelPathCSF(const CBRelPathCSF& source);
	const CBRelPathCSF& operator=(const CBRelPathCSF& source);
};


/******************************************************************************
 Path type

 ******************************************************************************/

inline CBRelPathCSF::PathType
CBRelPathCSF::GetPathType()
	const
{
	return itsPathType;
}

inline void
CBRelPathCSF::SetPathType
	(
	const PathType type
	)
{
	itsPathType = type;
}

#endif
