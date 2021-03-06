/******************************************************************************
 GFGLink.h

	Interface for the GFGLink class

	Copyright (C) 2002 by Glenn W. Bach.  All rights reserved.
	
 *****************************************************************************/

#ifndef _H_GFGLink
#define _H_GFGLink

// Superclass Header
#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>
#include <JMessageProtocol.h>

class JOutPipeStream;
class JProcess;

class GFGClass;
class GFGMemberFunction;

class GFGLink : public JBroadcaster
{
public:

	GFGLink();
	virtual ~GFGLink();

	JBoolean	StartCTags();
	void		ParseClass(GFGClass* list, const JCharacter* filename, const JCharacter* classname);
	
private:

	typedef JMessageProtocol<ACE_LSOCK_STREAM>	ProcessLink;

	ProcessLink*	itsLink;

	JProcess*		itsCTagsProcess;			// can be NULL

	JOutPipeStream*	itsOutputLink;				// NULL if process not started
	int				itsInputFD;					// -1 if process not started

	GFGClass*		itsClassList;
	JString			itsCurrentClass;
	JString			itsCurrentFile;

private:

	void	StopCTags();
	void	ParseLine(const JString& data);
	void	ParseInterface(GFGMemberFunction* fn, const JIndex line);

	// not allowed

	GFGLink(const GFGLink& source);
	const GFGLink& operator=(const GFGLink& source);

public:

	static const JCharacter* kFileParsed;

	class FileParsed : public JBroadcaster::Message
		{
		public:

			FileParsed()
				:
				JBroadcaster::Message(kFileParsed)
				{ };
		};

};

#endif
