/******************************************************************************
 JXSelectionManager.h

	Copyright © 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXSelectionManager
#define _H_JXSelectionManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXSelectionData.h>

const Atom kJXClipboardName = XA_PRIMARY;

class JXSelectionManager : virtual public JBroadcaster
{
	friend class JXDNDManager;

public:

	enum DeleteMethod
	{
		kArrayDelete,
		kXFree,
		kCFree
	};

public:

	JXSelectionManager(JXDisplay* display);

	virtual ~JXSelectionManager();

	JBoolean	GetAvailableTypes(const Atom selectionName, const Time time,
								  JArray<Atom>* typeList);
	JBoolean	GetData(const Atom selectionName, const Time time,
						const Atom requestType, Atom* returnType,
						unsigned char** data, JSize* dataLength,
						DeleteMethod* delMethod);
	void		DeleteData(unsigned char** data, const DeleteMethod delMethod);

	void	SendDeleteRequest(const Atom selectionName, const Time time);

	JBoolean	OwnedSelection(const Atom selectionName, const Time time);
	JBoolean	OwnsSelection(const Atom selectionName);
	JBoolean	GetData(const Atom selectionName, const Time time,
						const JXSelectionData** data);
	JBoolean	SetData(const Atom selectionName, JXSelectionData* data);
	void		ClearData(const Atom selectionName, const Time endTime);

	Atom	GetTargetsXAtom() const;
	Atom	GetTimeStampXAtom() const;
	Atom	GetTextXAtom() const;
	Atom	GetCompoundTextXAtom() const;
	Atom	GetMultipleXAtom() const;
	Atom	GetMimePlainTextXAtom() const;
	Atom	GetURLXAtom() const;

	Atom	GetDeleteSelectionXAtom() const;
	Atom	GetNULLXAtom() const;

	// called by JXDisplay

	void	HandleSelectionRequest(const XSelectionRequestEvent& selReqEvent);

protected:

	virtual void	ReceiveWithFeedback(JBroadcaster* sender, Message* message);

private:

	JXDisplay*					itsDisplay;		// owns us
	Window						itsDataWindow;
	JPtrArray<JXSelectionData>*	itsDataList;	// current + recent

	JSize		itsMaxDataChunkSize;	// max # of 4-byte blocks that we can send
	JBoolean	itsReceivedAllocErrorFlag;
	Window		itsTargetWindow;
	JBoolean	itsTargetWindowDeletedFlag;

	Atom itsSelectionWindPropXAtom;		// for receiving selection data
	Atom itsIncrementalSendXAtom;		// for sending data incrementally

	Atom itsTargetsXAtom;				// returns type XA_ATOM
	Atom itsTimeStampXAtom;				// returns type XA_INTEGER
	Atom itsTextXAtom;					//  8-bit characters
	Atom itsCompoundTextXAtom;			// 16-bit characters
	Atom itsMultipleXAtom;				// several formats at once
	Atom itsMimePlainTextXAtom;			// "text/plain"
	Atom itsURLXAtom;					// "text/uri-list"
	Atom itsDeleteSelectionXAtom;		// returns type "NULL"
	Atom itsNULLXAtom;					// "NULL"
	Atom itsGnomeClipboardName;			// "CLIPBOARD"

private:

	JBoolean	RequestData(const Atom selectionName, const Time time,
							const Atom type, XSelectionEvent* selEvent);

	void		SendData(const Window requestor, const Atom property,
						 const Atom type, unsigned char* data,
						 const JSize dataLength, const JSize bitsPerBlock,
						 XEvent* returnEvent);
	JBoolean	SendData1(const Window requestor, const Atom property,
						  const Atom type, unsigned char* data,
						  const JSize dataLength, const JSize bitsPerBlock);
	JBoolean	WaitForPropertyDeleted(const Window xWindow, const Atom property);
	static Bool	GetNextPropDeletedEvent(Display* display, XEvent* event, char* arg);

	JBoolean	ReceiveDataIncr(const Atom selectionName,
								Atom* returnType, unsigned char** data,
								JSize* dataLength, DeleteMethod* delMethod);
	JBoolean	WaitForPropertyCreated(const Window xWindow, const Atom property,
									   const Window sender);
	static Bool	GetNextNewPropertyEvent(Display* display, XEvent* event, char* arg);

	JBoolean	GetData(const Atom selectionName, const Time time,
						JXSelectionData** data, JIndex* index = NULL);
	void		DeleteOutdatedData();

	// called by JXDNDManager

	Window	GetDataTransferWindow();

	// not allowed

	JXSelectionManager(const JXSelectionManager& source);
	const JXSelectionManager& operator=(const JXSelectionManager& source);
};


/******************************************************************************
 Get atom

 ******************************************************************************/

inline Atom
JXSelectionManager::GetTargetsXAtom()
	const
{
	return itsTargetsXAtom;
}

inline Atom
JXSelectionManager::GetTimeStampXAtom()
	const
{
	return itsTimeStampXAtom;
}

inline Atom
JXSelectionManager::GetTextXAtom()
	const
{
	return itsTextXAtom;
}

inline Atom
JXSelectionManager::GetCompoundTextXAtom()
	const
{
	return itsCompoundTextXAtom;
}

inline Atom
JXSelectionManager::GetMultipleXAtom()
	const
{
	return itsMultipleXAtom;
}

inline Atom
JXSelectionManager::GetMimePlainTextXAtom()
	const
{
	return itsMimePlainTextXAtom;
}

inline Atom
JXSelectionManager::GetURLXAtom()
	const
{
	return itsURLXAtom;
}

inline Atom
JXSelectionManager::GetDeleteSelectionXAtom()
	const
{
	return itsDeleteSelectionXAtom;
}

inline Atom
JXSelectionManager::GetNULLXAtom()
	const
{
	return itsNULLXAtom;
}

/******************************************************************************
 Selection ownership

 ******************************************************************************/

inline JBoolean
JXSelectionManager::OwnedSelection
	(
	const Atom	selectionName,
	const Time	time
	)
{
	const JXSelectionData* data;
	return GetData(selectionName, time, &data);
}

inline JBoolean
JXSelectionManager::OwnsSelection
	(
	const Atom selectionName
	)
{
	const JXSelectionData* data;
	return GetData(selectionName, CurrentTime, &data);
}

/******************************************************************************
 GetDataTransferWindow (private)

 ******************************************************************************/

inline Window
JXSelectionManager::GetDataTransferWindow()
{
	return itsDataWindow;
}

#endif
