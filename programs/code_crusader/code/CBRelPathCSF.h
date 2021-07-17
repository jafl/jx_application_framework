/******************************************************************************
 CBRelPathCSF.h

	Copyright © 1999 by John Lindal.

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

	JString			ConvertToRelativePath(const JString& fullPath);
	static JString	ConvertToRelativePath(const JString& fullPath,
										  const JString& projPath,
										  const PathType pathType);
	static PathType	CalcPathType(const JString& path);

	bool ChooseRelFile(const JString& prompt,
						   const JString& instructions,
						   const JString& origName,
						   JString* name);					// relative

	bool ChooseRelRPath(const JString& prompt,
							const JString& instructions,
							const JString& origPath,
							JString* newPath);				// relative
	bool ChooseRelRWPath(const JString& prompt,
							 const JString& instructions,
							 const JString& origPath,
							 JString* newPath);				// relative

protected:

	virtual JXChooseFileDialog*
	CreateChooseFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						   const JString& fileFilter,
						   const bool allowSelectMultiple,
						   const JString& origName, const JString& message) override;

	virtual JXChoosePathDialog*
	CreateChoosePathDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						   const JString& fileFilter,
						   const bool selectOnlyWritable, const JString& message) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CBRPChooseFileDialog*	itsFileDialog;
	CBRPChoosePathDialog*	itsPathDialog;
	PathType				itsPathType;
	CBProjectDocument*		itsDoc;

private:

	JString	PrepareForChoose(const JString& origName);
	void	CalcPathType(const JString& path, PathType* type) const;

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
