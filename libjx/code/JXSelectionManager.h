/******************************************************************************
 JXSelectionManager.h

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXSelectionManager
#define _H_JXSelectionManager

#include "JXSelectionData.h"

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

	JXDisplay*	GetDisplay();

	Atom	GetTargetsXAtom() const;
	Atom	GetTimeStampXAtom() const;
	Atom	GetUtf8StringXAtom() const;
	Atom	GetMultipleXAtom() const;
	Atom	GetMimePlainTextXAtom() const;
	Atom	GetMimePlainTextUTF8XAtom() const;
	Atom	GetURLXAtom() const;

	Atom	GetDeleteSelectionXAtom() const;
	Atom	GetNULLXAtom() const;

	// called by JXDisplay

	void	HandleSelectionRequest(const XSelectionRequestEvent& selReqEvent);

protected:

	virtual void	ReceiveWithFeedback(JBroadcaster* sender, Message* message) override;

public:		// kAtomCount required at global scope

	enum
	{
		kSelectionWindPropAtomIndex,
		kIncrementalSendAtomIndex,
		kTargetsAtomIndex,
		kTimeStampAtomIndex,
		kUtf8StringAtomIndex,
		kMultipleAtomIndex,
		kMimePlainTextAtomIndex,
		kMimePlainTextUTF8AtomIndex,
		kURLAtomIndex,
		kDeleteSelectionAtomIndex,
		kNULLAtomIndex,
		kGnomeClipboardAtomIndex,

		kAtomCount
	};

private:

	JXDisplay*					itsDisplay;		// owns us
	Window						itsDataWindow;
	JPtrArray<JXSelectionData>*	itsDataList;	// current + recent

	JSize		itsMaxDataChunkSize;	// max # of 4-byte blocks that we can send
	JBoolean	itsReceivedAllocErrorFlag;
	Window		itsTargetWindow;
	JBoolean	itsTargetWindowDeletedFlag;
	Atom		itsAtoms[ kAtomCount ];

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
						JXSelectionData** data, JIndex* index = nullptr);
	void		DeleteOutdatedData();

	// called by JXDNDManager

	Window	GetDataTransferWindow();

	// not allowed

	JXSelectionManager(const JXSelectionManager& source);
	const JXSelectionManager& operator=(const JXSelectionManager& source);
};


/******************************************************************************
 GetDisplay

 ******************************************************************************/

inline JXDisplay*
JXSelectionManager::GetDisplay()
{
	return itsDisplay;
}

/******************************************************************************
 Get atom

 ******************************************************************************/

inline Atom
JXSelectionManager::GetTargetsXAtom()
	const
{
	return itsAtoms[ kTargetsAtomIndex ];
}

inline Atom
JXSelectionManager::GetTimeStampXAtom()
	const
{
	return itsAtoms[ kTimeStampAtomIndex ];
}

inline Atom
JXSelectionManager::GetUtf8StringXAtom()
	const
{
	return itsAtoms [ kUtf8StringAtomIndex ];
}

inline Atom
JXSelectionManager::GetMultipleXAtom()
	const
{
	return itsAtoms[ kMultipleAtomIndex ];
}

inline Atom
JXSelectionManager::GetMimePlainTextXAtom()
	const
{
	return itsAtoms[ kMimePlainTextAtomIndex ];
}

inline Atom
JXSelectionManager::GetMimePlainTextUTF8XAtom()
	const
{
	return itsAtoms[ kMimePlainTextUTF8AtomIndex ];
}

inline Atom
JXSelectionManager::GetURLXAtom()
	const
{
	return itsAtoms[ kURLAtomIndex ];
}

inline Atom
JXSelectionManager::GetDeleteSelectionXAtom()
	const
{
	return itsAtoms[ kDeleteSelectionAtomIndex ];
}

inline Atom
JXSelectionManager::GetNULLXAtom()
	const
{
	return itsAtoms[ kNULLAtomIndex ];
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
