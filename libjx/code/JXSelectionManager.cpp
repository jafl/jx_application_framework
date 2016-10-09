/******************************************************************************
 JXSelectionManager.cpp

	Global object to interface with X Selection mechanism.

	Nobody else seems to support MULTIPLE, so I see no reason to support
	it here.  If we ever need to support it, here is the basic idea:

		Pass in filled JArray<Atom>* and empty JArray<char*>*
		We remove the unconverted types from JArray<Atom> and
			fill in the JArray<char*> with the converted data

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXSelectionManager.h>
#include <JXDNDManager.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXWidget.h>
#include <jXGlobals.h>
#include <jXUtil.h>
#include <jTime.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <X11/Xlib.h>
#include <jAssert.h>

#define JXSEL_DEBUG_MSGS		0	// boolean
#define JXSEL_DEBUG_ONLY_RESULT 0	// boolean
#define JXSEL_MICRO_TRANSFER	0	// boolean

const Time kHistoryInterval = 60000;	// 1 minute (milliseconds)

const clock_t kWaitForSelectionTime = 5 * CLOCKS_PER_SEC;
const clock_t kUserBoredWaitingTime = 1 * CLOCKS_PER_SEC;

static const JCharacter* kAtomNames[ JXSelectionManager::kAtomCount ] =
{
	"JXSelectionWindowProperty",	// for receiving selection data
	"INCR",							// for sending data incrementally
	"TARGETS",						// returns type XA_ATOM
	"TIMESTAMP",					// returns type XA_INTEGER
	"TEXT",							//  8-bit characters
	"COMPOUND_TEXT",				// 16-bit characters
	"MULTIPLE",						// several formats at once
	"text/plain",
	"text/uri-list",
	"DELETE",						// returns type "NULL"
	"NULL",
	"CLIPBOARD"
};

/******************************************************************************
 Constructor

 ******************************************************************************/

JXSelectionManager::JXSelectionManager
	(
	JXDisplay* display
	)
{
	itsDisplay = display;

	itsDataList = jnew JPtrArray<JXSelectionData>(JPtrArrayT::kDeleteAll);
	assert( itsDataList != NULL );

	itsMaxDataChunkSize = XMaxRequestSize(*display) * 4/5;

	itsReceivedAllocErrorFlag  = kJFalse;
	itsTargetWindow            = None;
	itsTargetWindowDeletedFlag = kJFalse;

	// create data transfer window

	const unsigned long valueMask = CWOverrideRedirect | CWEventMask;

	XSetWindowAttributes attr;
	attr.override_redirect = kJTrue;
	attr.event_mask        = PropertyChangeMask;

	itsDataWindow = XCreateSimpleWindow(*itsDisplay, itsDisplay->GetRootWindow(),
										0,0,10,10, 0, 0,0);
	XChangeWindowAttributes(*itsDisplay, itsDataWindow, valueMask, &attr);

	// create required X atoms

	JString mimePlainTextXAtomName = kAtomNames[ kMimePlainTextAtomIndex ];
	JIndex charSetIndex;
	if (JXGetLatinCharacterSetIndex(&charSetIndex) && charSetIndex != 1)
		{
		mimePlainTextXAtomName += ";charset=iso8859-";
		mimePlainTextXAtomName += JString(charSetIndex, JString::kBase10);
		}

	const JCharacter* atomNames[ JXSelectionManager::kAtomCount ];
	memcpy(atomNames, kAtomNames, sizeof(kAtomNames));
	atomNames[ kMimePlainTextAtomIndex ] = mimePlainTextXAtomName;

	itsDisplay->RegisterXAtoms(kAtomCount, atomNames, itsAtoms);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXSelectionManager::~JXSelectionManager()
{
	jdelete itsDataList;

	XDestroyWindow(*itsDisplay, itsDataWindow);
}

/******************************************************************************
 GetAvailableTypes

	time can be CurrentTime.

 ******************************************************************************/

JBoolean
JXSelectionManager::GetAvailableTypes
	(
	const Atom		selectionName,
	const Time		time,
	JArray<Atom>*	typeList
	)
{
	// Check if this application owns the selection.

	JXSelectionData* data = NULL;
	if (GetData(selectionName, time, &data))
		{
		*typeList = data->GetTypeList();
		return kJTrue;
		}

	// We have to go via the X server.

	typeList->RemoveAll();

	XSelectionEvent selEvent;
	if (RequestData(selectionName, time, itsAtoms[ kTargetsAtomIndex ], &selEvent))
		{
		Atom actualType;
		int actualFormat;
		unsigned long itemCount, remainingBytes;
		unsigned char* data = NULL;
		XGetWindowProperty(*itsDisplay, itsDataWindow, itsAtoms[ kSelectionWindPropAtomIndex ],
						   0, LONG_MAX, True, XA_ATOM,
						   &actualType, &actualFormat,
						   &itemCount, &remainingBytes, &data);

		if (actualType == XA_ATOM &&
			actualFormat/8 == sizeof(Atom) && remainingBytes == 0)
			{
			Atom* atomData = reinterpret_cast<Atom*>(data);
			for (JIndex i=1; i<=itemCount; i++)
				{
				typeList->AppendElement(atomData[i-1]);
				}

			XFree(data);
			return kJTrue;
			}
		else
			{
//			cout << "TARGETS returned " << XGetAtomName(*itsDisplay, actualType) << endl;
//			cout << "  format:    " << actualFormat/8 << endl;
//			cout << "  count:     " << itemCount << endl;
//			cout << "  remaining: " << remainingBytes << endl;

			XFree(data);
			// fall through
			}
		}

	if (selectionName == kJXClipboardName &&
		XGetSelectionOwner(*itsDisplay, kJXClipboardName) != None)
		{
		// compensate for brain damage in rxvt, etc.

//		cout << "XA_PRIMARY owner: " << XGetSelectionOwner(*itsDisplay, XA_PRIMARY) << endl;
//		cout << "XA_SECONDARY owner: " << XGetSelectionOwner(*itsDisplay, XA_SECONDARY) << endl;
//		cout << "CLIPBOARD owner: " << XGetSelectionOwner(*itsDisplay, XInternAtom(*itsDisplay, "CLIPBOARD", False)) << endl;

		typeList->AppendElement(XA_STRING);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 GetData

	time can be CurrentTime.

	*** Caller is responsible for calling DeleteData() on the data
		that is returned.

 ******************************************************************************/

JBoolean
JXSelectionManager::GetData
	(
	const Atom		selectionName,
	const Time		time,
	const Atom		requestType,
	Atom*			returnType,
	unsigned char**	data,
	JSize*			dataLength,
	DeleteMethod*	delMethod
	)
{
	// Check if this application owns the selection.

	JXSelectionData* localData = NULL;
	JSize bitsPerBlock;
	if (GetData(selectionName, time, &localData))
		{
		if (localData->Convert(requestType, returnType,
							   data, dataLength, &bitsPerBlock))
			{
			*delMethod = kArrayDelete;
			return kJTrue;
			}
		else
			{
			*returnType = None;
			*data       = NULL;
			*dataLength = 0;
			return kJFalse;
			}
		}

	// We have to go via the X server.

	*returnType = None;
	*data       = NULL;
	*dataLength = 0;
	*delMethod  = kXFree;

	JBoolean success = kJFalse;

	XSelectionEvent selEvent;
	if (RequestData(selectionName, time, requestType, &selEvent))
		{
		#if JXSEL_DEBUG_MSGS && ! JXSEL_DEBUG_ONLY_RESULT
		cout << "Received SelectionNotify" << endl;
		#endif

		// We need to discard all existing PropertyNotify events
		// before initiating the incremental transfer.
		{
		XEvent xEvent;
		XID checkIfEventData[] = { itsDataWindow, itsAtoms[ kSelectionWindPropAtomIndex ] };
		while (XCheckIfEvent(*itsDisplay, &xEvent, GetNextNewPropertyEvent,
							 reinterpret_cast<char*>(checkIfEventData)))
			{
			// ignore the event
			}
		}

		// Initiate incremental transfer by deleting the property.

		int actualFormat;
		unsigned long itemCount, remainingBytes;
		XGetWindowProperty(*itsDisplay, itsDataWindow, itsAtoms[ kSelectionWindPropAtomIndex ],
						   0, LONG_MAX, True, AnyPropertyType,
						   returnType, &actualFormat,
						   &itemCount, &remainingBytes, data);

		if (*returnType == itsAtoms[ kIncrementalSendAtomIndex ])
			{
			XFree(*data);
			success = ReceiveDataIncr(selectionName, returnType,
									  data, dataLength, delMethod);
			}
		else if (*returnType != None && remainingBytes == 0)
			{
			*dataLength = itemCount * actualFormat/8;
			success = kJTrue;
			}
		else
			{
			XFree(*data);
			*data = NULL;
			}
		}

	if (success && *returnType == itsAtoms[ kURLAtomIndex ])
		{
		const Window srcWindow = XGetSelectionOwner(*itsDisplay, selectionName);
		JString newData;
		if (JXFixBrokenURLs((char*) *data, *dataLength, itsDisplay, srcWindow, &newData))
			{
			XFree(*data);
			*data = (unsigned char*) malloc(newData.GetLength()+1);
			memcpy(*data, newData.GetCString(), newData.GetLength()+1);
			*dataLength = newData.GetLength();
			}
		}

	return success;
}

/******************************************************************************
 DeleteData

	This must be called with all data returned by GetData().
	The DeleteMethod must be the method returned by GetData().

 ******************************************************************************/

void
JXSelectionManager::DeleteData
	(
	unsigned char**		data,
	const DeleteMethod	delMethod
	)
{
	if (delMethod == kXFree)
		{
		XFree(*data);
		}
	else if (delMethod == kCFree)
		{
		free(*data);
		}
	else
		{
		assert( delMethod == kArrayDelete );
		jdelete [] *data;
		}

	*data = NULL;
}

/******************************************************************************
 SendDeleteRequest

	Implements the DELETE selection protocol.

	window should be any one that X can attach the data to.
	Widgets can simply pass in the result from GetWindow().

	time can be CurrentTime.

 ******************************************************************************/

void
JXSelectionManager::SendDeleteRequest
	(
	const Atom	selectionName,
	const Time	time
	)
{
	Atom returnType;
	unsigned char* data = NULL;
	JSize dataLength;
	JXSelectionManager::DeleteMethod delMethod;

	GetData(selectionName, time, itsAtoms[ kDeleteSelectionAtomIndex ],
			&returnType, &data, &dataLength, &delMethod);
	DeleteData(&data, delMethod);
}

/******************************************************************************
 RequestData (private)

 ******************************************************************************/

JBoolean
JXSelectionManager::RequestData
	(
	const Atom			selectionName,
	const Time			origTime,
	const Atom			type,
	XSelectionEvent*	selEvent
	)
{
	assert( type != None );

	JXDNDManager* dndMgr = itsDisplay->GetDNDManager();

	Time time = origTime;
	if (time == CurrentTime)
		{
		time = itsDisplay->GetLastEventTime();
		}

	XConvertSelection(*itsDisplay, selectionName, type,
					  itsAtoms[ kSelectionWindPropAtomIndex ], itsDataWindow, time);

	Bool receivedEvent = False;
	XEvent xEvent;
	const clock_t startTime = clock();
	const clock_t endTime   = startTime + kWaitForSelectionTime;
	JBoolean userBored      = kJFalse;
	while (!receivedEvent && clock() < endTime)
		{
		receivedEvent =
			XCheckTypedWindowEvent(*itsDisplay, itsDataWindow,
								   SelectionNotify, &xEvent);

		if (!userBored && clock() > startTime + kUserBoredWaitingTime)
			{
			userBored = kJTrue;
			(JXGetApplication())->DisplayBusyCursor();
			}
		}

	if (receivedEvent)
		{
		assert( xEvent.type == SelectionNotify );
		*selEvent = xEvent.xselection;
		return JI2B(selEvent->requestor == itsDataWindow &&
					selEvent->selection == selectionName &&
					selEvent->target    == type &&
					selEvent->property  == itsAtoms[ kSelectionWindPropAtomIndex ] );
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 HandleSelectionRequest

 ******************************************************************************/

void
JXSelectionManager::HandleSelectionRequest
	(
	const XSelectionRequestEvent& selReqEvent
	)
{
	XEvent xEvent;
	XSelectionEvent& returnEvent = xEvent.xselection;

	returnEvent.type      = SelectionNotify;
	returnEvent.display   = selReqEvent.display;
	returnEvent.requestor = selReqEvent.requestor;
	returnEvent.selection = selReqEvent.selection;
	returnEvent.target    = selReqEvent.target;
	returnEvent.property  = selReqEvent.property;
	returnEvent.time      = selReqEvent.time;

	if (returnEvent.property == None)
		{
		returnEvent.property = returnEvent.target;
		}

	Atom selectionName   = selReqEvent.selection;
	JXDNDManager* dndMgr = itsDisplay->GetDNDManager();
	if (selectionName == kJXClipboardName &&
		dndMgr->IsLastFakePasteTime(selReqEvent.time))
		{
		selectionName = dndMgr->GetDNDSelectionName();
		}

	// allow Gnome apps to paste via menu item

	if (selectionName == itsAtoms[ kGnomeClipboardAtomIndex ])
		{
		selectionName = kJXClipboardName;
		}

	JXSelectionData* selData = NULL;
	if (GetData(selectionName, selReqEvent.time, &selData))
		{
		Atom returnType;
		unsigned char* data;
		JSize dataLength;
		JSize bitsPerBlock;
		if (selData->Convert(selReqEvent.target,
							 &returnType, &data, &dataLength, &bitsPerBlock))
			{
			#if JXSEL_DEBUG_MSGS && ! JXSEL_DEBUG_ONLY_RESULT
			cout << "Accepted selection request: ";
			cout << XGetAtomName(*itsDisplay, selReqEvent.target);
			cout << ", time=" << selReqEvent.time << endl;
			#endif

			#if JXSEL_MICRO_TRANSFER
			const JSize savedMaxSize = itsMaxDataChunkSize;
			if (selReqEvent.target != itsTargetsXAtom)
				{
				itsMaxDataChunkSize = 1;
				}
			#endif

			SendData(selReqEvent.requestor, returnEvent.property, returnType,
					 data, dataLength, bitsPerBlock, &xEvent);
			jdelete [] data;

			#if JXSEL_MICRO_TRANSFER
			itsMaxDataChunkSize = savedMaxSize;
			#endif

			return;
			}
		else
			{
			#if JXSEL_DEBUG_MSGS
			cout << "Rejected selection request: can't convert to ";
			cout << XGetAtomName(*itsDisplay, selReqEvent.target);
			cout << ", time=" << selReqEvent.time << endl;
			#endif
			}
		}
	else
		{
		#if JXSEL_DEBUG_MSGS
		cout << "Rejected selection request: don't own ";
		cout << XGetAtomName(*itsDisplay, selReqEvent.selection);
		cout << ", time=" << selReqEvent.time << endl;
		#endif
		}

	returnEvent.property = None;
	itsDisplay->SendXEvent(selReqEvent.requestor, &xEvent);
}

/******************************************************************************
 SendData (private)

	Sends the given data either as one chunk or incrementally.

 ******************************************************************************/

void
JXSelectionManager::SendData
	(
	const Window	requestor,
	const Atom		property,
	const Atom		type,
	unsigned char*	data,
	const JSize		dataLength,
	const JSize		bitsPerBlock,
	XEvent*			returnEvent
	)
{
	JSize chunkSize = 4*itsMaxDataChunkSize;

	// if small enough, send it one chunk

	#if JXSEL_DEBUG_MSGS && ! JXSEL_DEBUG_ONLY_RESULT
	cout << "Selection is " << dataLength << " bytes" << endl;
	if (dataLength <= chunkSize)
		{
		cout << "Attempting to send entire selection" << endl;
		}
	#endif

	if (dataLength <= chunkSize &&
		SendData1(requestor, property, type,
				  data, dataLength, bitsPerBlock))
		{
		#if JXSEL_DEBUG_MSGS
		cout << "Transfer complete" << endl;
		#endif

		itsDisplay->SendXEvent(requestor, returnEvent);
		return;
		}

	// we need to hear when the property or the window is deleted

	XSelectInput(*itsDisplay, requestor, PropertyChangeMask | StructureNotifyMask);

	// initiate transfer by sending INCR

	#if JXSEL_DEBUG_MSGS && ! JXSEL_DEBUG_ONLY_RESULT
	cout << "Initiating incremental transfer" << endl;
	#endif

	(JXGetApplication())->DisplayBusyCursor();

	XID remainingLength = dataLength;		// must be 32 bits
	XChangeProperty(*itsDisplay, requestor, property, itsAtoms[ kIncrementalSendAtomIndex ],
					32, PropModeReplace,
					reinterpret_cast<unsigned char*>(&remainingLength), 1);
	itsDisplay->SendXEvent(requestor, returnEvent);
	if (!WaitForPropertyDeleted(requestor, property))
		{
		#if JXSEL_DEBUG_MSGS
		cout << "No response from requestor (data length)" << endl;
		#endif

		return;
		}

	// send a chunk and wait for it to be deleted

	#if JXSEL_DEBUG_MSGS
	JIndex chunkIndex = 0;
	#endif

	while (remainingLength > 0)
		{
		#if JXSEL_DEBUG_MSGS
		chunkIndex++;
		#endif

		unsigned char* dataStart = data + dataLength - remainingLength;
		if (chunkSize > remainingLength)
			{
			chunkSize = remainingLength;
			}

		#if JXSEL_DEBUG_MSGS && ! JXSEL_DEBUG_ONLY_RESULT
		cout << "Sending " << chunkSize << " bytes" << endl;
		#endif

		SendData1(requestor, property, type,
				  dataStart, chunkSize, bitsPerBlock);
		if (itsTargetWindowDeletedFlag)
			{
			#if JXSEL_DEBUG_MSGS
			cout << "Requestor crashed on iteration ";
			cout << chunkIndex << endl;
			#endif
			return;
			}
		else if (itsReceivedAllocErrorFlag && itsMaxDataChunkSize > 1)
			{
			itsMaxDataChunkSize /= 2;
			chunkSize            = 4*itsMaxDataChunkSize;

			#if JXSEL_DEBUG_MSGS && ! JXSEL_DEBUG_ONLY_RESULT
			cout << "Reducing chunk size to " << chunkSize << " bytes" << endl;
			#endif
			}
		else if (itsReceivedAllocErrorFlag)
			{
			#if JXSEL_DEBUG_MSGS
			cout << "X server is out of memory!" << endl;
			#endif

			XSelectInput(*itsDisplay, requestor, NoEventMask);
			return;
			}

		if (!WaitForPropertyDeleted(requestor, property))
			{
			#if JXSEL_DEBUG_MSGS
			cout << "No response from requestor on iteration ";
			cout << chunkIndex << ", " << dataLength - remainingLength;
			cout << " bytes sent, " << remainingLength;
			cout << " bytes remaining, chunk size " << chunkSize << endl;
			#endif

			return;
			}

		remainingLength -= chunkSize;
		}

	// send zero-length property to signal that we are done

	SendData1(requestor, property, type, data, 0, 8);

	// we are done interacting with the requestor

	if (!itsTargetWindowDeletedFlag)
		{
		XSelectInput(*itsDisplay, requestor, NoEventMask);
		}

	#if JXSEL_DEBUG_MSGS
	cout << "Transfer complete" << endl;
	#endif
}

/******************************************************************************
 SendData1 (private)

	Put the data into the window property and check for BadAlloc and
	BadWindow errors.

 ******************************************************************************/

JBoolean
JXSelectionManager::SendData1
	(
	const Window	requestor,
	const Atom		property,
	const Atom		type,
	unsigned char*	data,
	const JSize		dataLength,
	const JSize		bitsPerBlock
	)
{
	XChangeProperty(*itsDisplay, requestor, property, type,
					bitsPerBlock, PropModeReplace, data,
					dataLength/(bitsPerBlock/8));

	itsReceivedAllocErrorFlag  = kJFalse;
	itsTargetWindow            = requestor;
	itsTargetWindowDeletedFlag = kJFalse;

	itsDisplay->Synchronize();

	ListenTo(itsDisplay);
	itsDisplay->CheckForXErrors();
	StopListening(itsDisplay);

	itsTargetWindow = None;

	return JNegate(itsReceivedAllocErrorFlag || itsTargetWindowDeletedFlag);
}

/******************************************************************************
 WaitForPropertyDeleted (private)

	Wait for the receiver to jdelete the window property.
	Returns kJFalse if we time out or the window is deleted (receiver crash).

 ******************************************************************************/

JBoolean
JXSelectionManager::WaitForPropertyDeleted
	(
	const Window	xWindow,
	const Atom		property
	)
{
	itsDisplay->Synchronize();

	XEvent xEvent;
	XID checkIfEventData[] = { xWindow, property };
	const clock_t endTime = clock() + kWaitForSelectionTime;
	while (clock() < endTime)
		{
		const Bool receivedEvent =
			XCheckIfEvent(*itsDisplay, &xEvent, GetNextPropDeletedEvent,
						  reinterpret_cast<char*>(&checkIfEventData));

		if (receivedEvent && xEvent.type == PropertyNotify)
			{
			return kJTrue;
			}
		else if (receivedEvent && xEvent.type == DestroyNotify)
			{
			return kJFalse;
			}
		}

	#if JXSEL_DEBUG_MSGS

	Atom actualType;
	int actualFormat;
	unsigned long itemCount, remainingBytes;
	unsigned char* data;
	XGetWindowProperty(*itsDisplay, xWindow, property,
					   0, LONG_MAX, False, AnyPropertyType,
					   &actualType, &actualFormat,
					   &itemCount, &remainingBytes, &data);

	if (actualType == None && actualFormat == 0 && remainingBytes == 0)
		{
		cout << "Window property was deleted, but source was not notified!" << endl;
		}
	else
		{
		cout << "Window property not deleted, type " << XGetAtomName(*itsDisplay, actualType);
		cout << ", " << itemCount * actualFormat/8 << " bytes" << endl;
		}
	XFree(data);

	#endif

	XSelectInput(*itsDisplay, xWindow, NoEventMask);
	return kJFalse;
}

// static

Bool
JXSelectionManager::GetNextPropDeletedEvent
	(
	Display*	display,
	XEvent*		event,
	char*		arg
	)
{
	XID* data = reinterpret_cast<XID*>(arg);

	if (event->type             == PropertyNotify &&
		event->xproperty.window == data[0] &&
		event->xproperty.atom   == data[1] &&
		event->xproperty.state  == PropertyDelete)
		{
		return True;
		}
	else if (event->type                  == DestroyNotify &&
			 event->xdestroywindow.window == data[0])
		{
		return True;
		}
	else
		{
		return False;
		}
}

/******************************************************************************
 ReceiveDataIncr (private)

	Receives the current selection data incrementally.

 ******************************************************************************/

JBoolean
JXSelectionManager::ReceiveDataIncr
	(
	const Atom		selectionName,
	Atom*			returnType,
	unsigned char**	data,
	JSize*			dataLength,
	DeleteMethod*	delMethod
	)
{
	*returnType = None;
	*data       = NULL;
	*dataLength = 0;
	*delMethod  = kCFree;

	#if JXSEL_DEBUG_MSGS && ! JXSEL_DEBUG_ONLY_RESULT
	cout << "Initiating incremental receive" << endl;
	#endif

	(JXGetApplication())->DisplayBusyCursor();

	// we need to hear when the sender crashes

	const Window sender = XGetSelectionOwner(*itsDisplay, selectionName);
	if (sender == None)
		{
		return kJFalse;
		}
	XSelectInput(*itsDisplay, sender, StructureNotifyMask);

	// The transfer has already been initiated by deleting the
	// INCR property when it was retrieved.

	// wait for a chunk, retrieve it, and jdelete it

	#if JXSEL_DEBUG_MSGS
	JIndex chunkIndex = 0;
	#endif

	JBoolean ok = kJTrue;
	while (1)
		{
		#if JXSEL_DEBUG_MSGS
		chunkIndex++;
		#endif

		if (!WaitForPropertyCreated(itsDataWindow, itsAtoms[ kSelectionWindPropAtomIndex ], sender))
			{
			#if JXSEL_DEBUG_MSGS
			cout << "No response from selection owner on iteration ";
			cout << chunkIndex << ", " << *dataLength << " bytes received" << endl;
			#endif

			ok = kJFalse;
			break;
			}

		Atom actualType;
		int actualFormat;
		unsigned long itemCount, remainingBytes;
		unsigned char* chunk;
		XGetWindowProperty(*itsDisplay, itsDataWindow, itsAtoms[ kSelectionWindPropAtomIndex ],
						   0, LONG_MAX, True, AnyPropertyType,
						   &actualType, &actualFormat,
						   &itemCount, &remainingBytes, &chunk);

		if (actualType == None)
			{
			#if JXSEL_DEBUG_MSGS
			cout << "Received data of type None on iteration ";
			cout << chunkIndex << endl;
			#endif

			ok = kJFalse;
			break;
			}

		// an empty property means that we are done

		if (itemCount == 0)
			{
			#if JXSEL_DEBUG_MSGS
			if (*data == NULL)
				{
				cout << "Didn't receive any data on iteration ";
				cout << chunkIndex << endl;
				}
			#endif

			XFree(chunk);
			ok = JConvertToBoolean( *data != NULL );
			break;
			}

		// otherwise, append it to *data

		else
			{
			assert( remainingBytes == 0 );

			const JSize chunkSize = itemCount * actualFormat/8;
			if (*data == NULL)
				{
				// the first chunk determines the format
				*returnType = actualType;

				*data = static_cast<unsigned char*>(malloc(chunkSize));
				assert( *data != NULL );
				memcpy(*data, chunk, chunkSize);

				#if JXSEL_DEBUG_MSGS && ! JXSEL_DEBUG_ONLY_RESULT
				cout << "Data format: " << XGetAtomName(*itsDisplay, actualType) << endl;
				#endif
				}
			else
				{
				*data = static_cast<unsigned char*>(realloc(*data, *dataLength + chunkSize));
				memcpy(*data + *dataLength, chunk, chunkSize);
				}

			*dataLength += chunkSize;
			XFree(chunk);

			#if JXSEL_DEBUG_MSGS && ! JXSEL_DEBUG_ONLY_RESULT
			cout << "Received " << chunkSize << " bytes" << endl;
			#endif
			}
		}

	// we are done interacting with the sender

	XSelectInput(*itsDisplay, sender, NoEventMask);

	// clean up

	if (!ok && *data != NULL)
		{
		free(*data);
		*data       = NULL;
		*dataLength = 0;
		*returnType = None;
		}

	#if JXSEL_DEBUG_MSGS
	if (ok)
		{
		cout << "Transfer successful" << endl;
		}
	else
		{
		cout << "Transfer failed" << endl;
		}
	#endif

	return ok;
}

/******************************************************************************
 WaitForPropertyCreated (private)

	Wait for the receiver to create the window property.
	Returns kJFalse if we time out or the sender crashes.

 ******************************************************************************/

JBoolean
JXSelectionManager::WaitForPropertyCreated
	(
	const Window	xWindow,
	const Atom		property,
	const Window	sender
	)
{
	itsDisplay->Synchronize();

	XEvent xEvent;
	XID checkIfEventData[] = { xWindow, property };
	const clock_t endTime = clock() + kWaitForSelectionTime;
	while (clock() < endTime)
		{
		if (XCheckTypedWindowEvent(*itsDisplay, sender, DestroyNotify, &xEvent))
			{
			return kJFalse;
			}

		if (XCheckIfEvent(*itsDisplay, &xEvent, GetNextNewPropertyEvent,
						  reinterpret_cast<char*>(checkIfEventData)))
			{
			return kJTrue;
			}
		}

	return kJFalse;
}

// static

Bool
JXSelectionManager::GetNextNewPropertyEvent
	(
	Display*	display,
	XEvent*		event,
	char*		arg
	)
{
	XID* data = reinterpret_cast<XID*>(arg);

	if (event->type             == PropertyNotify &&
		event->xproperty.window == data[0] &&
		event->xproperty.atom   == data[1] &&
		event->xproperty.state  == PropertyNewValue)
		{
		return True;
		}
	else
		{
		return False;
		}
}

/******************************************************************************
 ReceiveWithFeedback (virtual protected)

	This catches errors while sending and receiving data.

 ******************************************************************************/

void
JXSelectionManager::ReceiveWithFeedback
	(
	JBroadcaster*	sender,
	Message*		message
	)
{
	if (sender == itsDisplay && message->Is(JXDisplay::kXError))
		{
		JXDisplay::XError* err = dynamic_cast<JXDisplay::XError*>(message);
		assert( err != NULL );

		if (err->GetType() == BadAlloc)
			{
			itsReceivedAllocErrorFlag = kJTrue;
			err->SetCaught();
			}

		else if (err->GetType() == BadWindow &&
				 err->GetXID()  == itsTargetWindow)
			{
			itsTargetWindowDeletedFlag = kJTrue;
			err->SetCaught();
			}
		}

	else
		{
		JBroadcaster::ReceiveWithFeedback(sender, message);
		}
}

/******************************************************************************
 SetData

	Set the data for the given selection.

	*** 'data' must be allocated on the heap.
		We take ownership of 'data' even if the function returns kJFalse.

 ******************************************************************************/

JBoolean
JXSelectionManager::SetData
	(
	const Atom			selectionName,
	JXSelectionData*	data
	)
{
	DeleteOutdatedData();

	const Time lastEventTime = itsDisplay->GetLastEventTime();

	// check if it already owns the selection

	JXSelectionData* origData = NULL;
	const JBoolean found = GetData(selectionName, CurrentTime, &origData);
	if (found && origData != data)
		{
		origData->SetEndTime(lastEventTime);
		}
	else if (found)
		{
		itsDataList->Remove(data);
		}

	// we are required to check XGetSelectionOwner()

	XSetSelectionOwner(*itsDisplay, selectionName, itsDataWindow, lastEventTime);
	if (XGetSelectionOwner(*itsDisplay, selectionName) == itsDataWindow)
		{
		#if JXSEL_DEBUG_MSGS
		cout << "Got selection ownership: ";
		cout << XGetAtomName(*itsDisplay, selectionName);
		cout << ", time=" << lastEventTime << endl;
		#endif

		// allow Gnome apps to paste via menu item

		if (selectionName == kJXClipboardName)
			{
			XSetSelectionOwner(*itsDisplay, itsAtoms[ kGnomeClipboardAtomIndex ], itsDataWindow, lastEventTime);
			}

		data->SetSelectionInfo(selectionName, lastEventTime);
		itsDataList->Append(data);
		return kJTrue;
		}
	else
		{
		jdelete data;
		return kJFalse;
		}
}

/******************************************************************************
 ClearData

	time can be CurrentTime.

	We have to pass endTime to GetData() because sometimes we have to
	rewrite the history book:  We called SetData() while we thought we owned
	the selection, but there was a SelectionClear event on its way, which
	means that, in reality, we lost ownership for a short while.

 ******************************************************************************/

void
JXSelectionManager::ClearData
	(
	const Atom	selectionName,
	const Time	endTime
	)
{
	JXSelectionData* data = NULL;
	if (GetData(selectionName, endTime, &data))
		{
		const Time t = (endTime == CurrentTime ? itsDisplay->GetLastEventTime() : endTime);
		data->SetEndTime(t);

		if (XGetSelectionOwner(*itsDisplay, selectionName) == itsDataWindow)
			{
			XSetSelectionOwner(*itsDisplay, selectionName, None, t);
			}
		}
}

/******************************************************************************
 GetData

	time can be CurrentTime.

	In the private version, if index != NULL, it contains the index into itsDataList.

 ******************************************************************************/

JBoolean
JXSelectionManager::GetData
	(
	const Atom				selectionName,
	const Time				time,
	const JXSelectionData**	data
	)
{
	return GetData(selectionName, time, const_cast<JXSelectionData**>(data));
}

// private

JBoolean
JXSelectionManager::GetData
	(
	const Atom			selectionName,
	const Time			time,
	JXSelectionData**	data,
	JIndex*				index
	)
{
	const JSize count = itsDataList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		*data         = itsDataList->NthElement(i);
		const Time t1 = (**data).GetStartTime();

		Time t2;
		const JBoolean finished = (**data).GetEndTime(&t2);

		if (selectionName == (**data).GetSelectionName() &&
			((time == CurrentTime && !finished) ||
			 (time != CurrentTime && t1 != CurrentTime &&
			  t1 <= time && (!finished || time <= t2))))
			{
			if (index != NULL)
				{
				*index = i;
				}
			return kJTrue;
			}
		}

	*data = NULL;
	if (index != NULL)
		{
		*index = 0;
		}
	return kJFalse;
}

/******************************************************************************
 DeleteOutdatedData (private)

 ******************************************************************************/

void
JXSelectionManager::DeleteOutdatedData()
{
	const Time currTime = itsDisplay->GetLastEventTime();
	const Time oldTime  = currTime - kHistoryInterval;

	const JSize count = itsDataList->GetElementCount();
	for (JIndex i=count; i>=1; i--)
		{
		JXSelectionData* data = itsDataList->NthElement(i);

		// toss if outdated or clock has wrapped

		Time endTime;
		if ((data->GetEndTime(&endTime) && endTime < oldTime) ||
			data->GetStartTime() > currTime)
			{
			itsDataList->DeleteElement(i);
			}
		}
}
