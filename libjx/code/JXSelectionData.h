/******************************************************************************
 JXSelectionData.h

	Copyright © 1996-2005 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXSelectionData
#define _H_JXSelectionData

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPtrArray-JString.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

class JXDisplay;
class JXWindow;
class JXWidget;
class JXSelectionManager;
class JXDNDManager;

class JXSelectionData : public JBroadcaster
{
public:

	JXSelectionData(JXDisplay* display);
	JXSelectionData(JXWidget* widget, const JCharacter* id);

	virtual	~JXSelectionData();

	JXDisplay*			GetDisplay() const;
	JXSelectionManager*	GetSelectionManager() const;
	JXDNDManager*		GetDNDManager() const;
	Atom				GetDNDSelectionName() const;

	Atom		GetSelectionName() const;
	Time		GetStartTime() const;
	JBoolean	IsCurrent() const;
	JBoolean	GetEndTime(Time* t) const;
	void		SetSelectionInfo(const Atom selectionName, const Time startTime);
	void		SetEndTime(const Time endTime);

	const JArray<Atom>&	GetTypeList() const;

	void		Resolve() const;
	JBoolean	Convert(const Atom requestType, Atom* returnType,
						unsigned char** data, JSize* dataLength,
						JSize* bitsPerBlock) const;

protected:

	Atom	AddType(const JCharacter* name);
	void	AddType(const Atom type);
	void	RemoveType(const Atom type);

	virtual void		AddTypes(const Atom selectionName) = 0;
	virtual JBoolean	ConvertData(const Atom requestType, Atom* returnType,
									unsigned char** data, JSize* dataLength,
									JSize* bitsPerBlock) const = 0;

	virtual void	ReceiveGoingAway(JBroadcaster* sender);

private:

	JXDisplay*		itsDisplay;			// not owned
	Atom			itsSelectionName;
	Time			itsStartTime;
	Time			itsEndTime;
	JArray<Atom>*	itsTypeList;		// sorted

	// used for delayed evaluation -- NULL after Resolve()

	JXWidget*	itsDataSource;			// not owned
	JString*	itsDataSourceID;

private:

	void	JXSelectionDataX();

	static JOrderedSetT::CompareResult
		CompareAtoms(const Atom& atom1, const Atom& atom2);

	// not allowed

	JXSelectionData(const JXSelectionData& source);
	const JXSelectionData& operator=(const JXSelectionData& source);
};


/******************************************************************************
 GetDisplay

 ******************************************************************************/

inline JXDisplay*
JXSelectionData::GetDisplay()
	const
{
	return itsDisplay;
}

/******************************************************************************
 GetSelectionName

 ******************************************************************************/

inline Atom
JXSelectionData::GetSelectionName()
	const
{
	return itsSelectionName;
}

/******************************************************************************
 GetStartTime

 ******************************************************************************/

inline Time
JXSelectionData::GetStartTime()
	const
{
	return itsStartTime;
}

/******************************************************************************
 IsCurrent

 ******************************************************************************/

inline JBoolean
JXSelectionData::IsCurrent()
	const
{
	return JI2B( itsEndTime == CurrentTime );
}

/******************************************************************************
 End time

 ******************************************************************************/

inline JBoolean
JXSelectionData::GetEndTime
	(
	Time* t
	)
	const
{
	*t = itsEndTime;
	return JI2B( itsEndTime != CurrentTime );
}

inline void
JXSelectionData::SetEndTime
	(
	const Time endTime
	)
{
	itsEndTime = endTime - 1;	// given time is when it changed
}

/******************************************************************************
 GetTypeList

 ******************************************************************************/

inline const JArray<Atom>&
JXSelectionData::GetTypeList()
	const
{
	return *itsTypeList;
}

/******************************************************************************
 AddType (protected)

	Add the target to the list if it is not already included.

 ******************************************************************************/

inline void
JXSelectionData::AddType
	(
	const Atom type
	)
{
	itsTypeList->InsertSorted(type, kJFalse);
}

#endif
