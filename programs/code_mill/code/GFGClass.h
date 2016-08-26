/******************************************************************************
 GFGClass.h

	Interface for the GFGClass class

	Copyright © 2002 by Glenn W. Bach.  All rights reserved.
	
 *****************************************************************************/

#ifndef _H_GFGClass
#define _H_GFGClass

// Superclass Header
#include <JPtrArray.h>

#include <GFGMemberFunction.h>

class GFGLink;

class GFGClass : public JPtrArray<GFGMemberFunction>
{
public:

	GFGClass();
	virtual ~GFGClass();

	JSize			GetBaseClassCount() const;
	void			GetBaseClass(const JIndex index, JString* classname, JString* filename) const;
	void			AddBaseClass(const JCharacter* classname, const JCharacter* filename);

	const JString&	GetClassName() const;
	void			SetClassName(const JCharacter* classname);

	const JString&	GetFileName() const;
	void			SetFileName(const JCharacter* filename);

	JSize	GetAncestorCount() const;
	void	GetAncestor(const JIndex index, JString* classname, JString* filename) const;
	void	AddAncestor(const JCharacter* classname, const JCharacter* filename);

	void	Populate();

	void	WritePublicInterface(ostream& os);
	void	WriteProtectedInterface(ostream& os);
	void	WritePublic(ostream& os, const JBoolean interface = kJFalse);
	void	WriteProtected(ostream& os, const JBoolean interface = kJFalse);
	
protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	
private:

	GFGLink*	itsLink;

	JPtrArray<JString>*		itsBaseClasses;
	JPtrArray<JString>*		itsBaseClassFiles;

	JPtrArray<JString>*		itsAncestors;
	JPtrArray<JString>*		itsAncestorFiles;

	JString 	itsFileName;
	JString		itsClassName;

private:

	void WriteFunction(ostream& os, GFGMemberFunction* fn, const JBoolean interface);

	// not allowed

	GFGClass(const GFGClass& source);
	const GFGClass& operator=(const GFGClass& source);

};


/******************************************************************************
 GetClassName (public)

 ******************************************************************************/

inline const JString&
GFGClass::GetClassName()
	const
{
	return itsClassName;
}

/******************************************************************************
 SetClassName (public)

 ******************************************************************************/

inline void
GFGClass::SetClassName
	(
	const JCharacter* classname
	)
{
	itsClassName	= classname;
}

/******************************************************************************
 GetFileName (public)

 ******************************************************************************/

inline const JString&
GFGClass::GetFileName()
	const
{
	return itsFileName;
}

/******************************************************************************
 SetFileName (public)

 ******************************************************************************/

inline void
GFGClass::SetFileName
	(
	const JCharacter* filename
	)
{
	itsFileName	= filename;
}

#endif
