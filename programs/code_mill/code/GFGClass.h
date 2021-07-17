/******************************************************************************
 GFGClass.h

	Interface for the GFGClass class

	Copyright (C) 2002 by Glenn W. Bach.
	
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
	void			AddBaseClass(const JString& classname, const JString& filename);

	const JString&	GetClassName() const;
	void			SetClassName(const JString& classname);

	const JString&	GetFileName() const;
	void			SetFileName(const JString& filename);

	JSize	GetAncestorCount() const;
	void	GetAncestor(const JIndex index, JString* classname, JString* filename) const;
	void	AddAncestor(const JString& classname, const JString& filename);

	void	Populate();

	void	WritePublicInterface(std::ostream& os);
	void	WriteProtectedInterface(std::ostream& os);
	void	WritePublic(std::ostream& os, const bool interface = false);
	void	WriteProtected(std::ostream& os, const bool interface = false);
	
protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	
private:

	GFGLink*	itsLink;

	JPtrArray<JString>*		itsBaseClasses;
	JPtrArray<JString>*		itsBaseClassFiles;

	JPtrArray<JString>*		itsAncestors;
	JPtrArray<JString>*		itsAncestorFiles;

	JString 	itsFileName;
	JString		itsClassName;

private:

	void WriteFunction(std::ostream& os, GFGMemberFunction* fn, const bool interface);

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
	const JString& classname
	)
{
	itsClassName = classname;
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
	const JString& filename
	)
{
	itsFileName = filename;
}

#endif
