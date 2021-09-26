/******************************************************************************
 JXSelectionData.h

	Copyright (C) 1996-2005 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXSelectionData
#define _H_JXSelectionData

#include <jx-af/jcore/JPtrArray-JString.h>
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
	JXSelectionData(JXWidget* widget, const JUtf8Byte* id);

	virtual	~JXSelectionData();

	JXDisplay*			GetDisplay() const;
	JXSelectionManager*	GetSelectionManager() const;
	JXDNDManager*		GetDNDManager() const;
	Atom				GetDNDSelectionName() const;

	Atom		GetSelectionName() const;
	Time		GetStartTime() const;
	bool	IsCurrent() const;
	bool	GetEndTime(Time* t) const;
	void		SetSelectionInfo(const Atom selectionName, const Time startTime);
	void		SetEndTime(const Time endTime);

	const JArray<Atom>&	GetTypeList() const;

	void		Resolve() const;
	bool	Convert(const Atom requestType, Atom* returnType,
						unsigned char** data, JSize* dataLength,
						JSize* bitsPerBlock) const;

protected:

	Atom	AddType(const JUtf8Byte* name);
	void	AddType(const Atom type);
	void	RemoveType(const Atom type);

	virtual void		AddTypes(const Atom selectionName) = 0;
	virtual bool	ConvertData(const Atom requestType, Atom* returnType,
									unsigned char** data, JSize* dataLength,
									JSize* bitsPerBlock) const = 0;

	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	JXDisplay*		itsDisplay;			// not owned
	Atom			itsSelectionName;
	Time			itsStartTime;
	Time			itsEndTime;
	JArray<Atom>*	itsTypeList;		// sorted

	// used for delayed evaluation -- nullptr after Resolve()

	JXWidget*	itsDataSource;			// not owned
	JString*	itsDataSourceID;		// may be nullptr

private:

	void	JXSelectionDataX();

	static JListT::CompareResult
		CompareAtoms(const Atom& atom1, const Atom& atom2);

	// not allowed

	JXSelectionData(const JXSelectionData&) = delete;
	JXSelectionData& operator=(const JXSelectionData&) = delete;
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

inline bool
JXSelectionData::IsCurrent()
	const
{
	return itsEndTime == CurrentTime;
}

/******************************************************************************
 End time

 ******************************************************************************/

inline bool
JXSelectionData::GetEndTime
	(
	Time* t
	)
	const
{
	*t = itsEndTime;
	return itsEndTime != CurrentTime;
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
	itsTypeList->InsertSorted(type, false);
}

#endif
