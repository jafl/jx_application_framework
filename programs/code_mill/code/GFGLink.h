/******************************************************************************
 GFGLink.h

	Interface for the GFGLink class

	Copyright (C) 2002 by Glenn W. Bach.
	
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

	bool	StartCTags();
	void		ParseClass(GFGClass* list, const JString& filename, const JString& classname);
	
private:

	typedef JMessageProtocol<ACE_LSOCK_STREAM>	ProcessLink;

	ProcessLink*	itsLink;

	JProcess*		itsCTagsProcess;			// can be nullptr

	JOutPipeStream*	itsOutputLink;				// nullptr if process not started
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

	static const JUtf8Byte* kFileParsed;

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
