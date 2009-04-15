/*********************************************************************************
 DataModule.h

	Interface for the DataModule class.
 
	Copyright @ 1997 by Glenn W. Bach. All rights reserved.

 ********************************************************************************/
 
#ifndef _H_DataModule
#define _H_DataModule

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JBroadcaster.h>
#include <JArray.h>

#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>
#include <JMessageProtocol.h>	// template which requires ace includes

class GXRaggedFloatTable;
class GRaggedFloatTableData;
class JProcess;
class JOutPipeStream;
//class JIPCIOStream;
class JProgressDisplay;
class JString;

class DataModule : virtual public JBroadcaster
{
public:

	static JBoolean Create(	DataModule** module,
							GXRaggedFloatTable* table, 
							GRaggedFloatTableData* data,
							const JCharacter* sysCmd);
	virtual ~DataModule();

protected:

	DataModule(	GXRaggedFloatTable* table, GRaggedFloatTableData* data,
				JProcess* process, const int fd, JOutPipeStream* output);

	virtual void	Receive(JBroadcaster* sender,
						const JBroadcaster::Message& message);

private:

	typedef JMessageProtocol<ACE_LSOCK_STREAM>	ProcessLink;

private:

	GXRaggedFloatTable* 		itsTable;
	GRaggedFloatTableData*		itsData;
	JOutPipeStream*				itsOutput;
	JBoolean					itsHeaderRead;
	JBoolean					itsSentData;
	JBoolean					itsDataIsDump;
	JSize						itsColNum;
	JIndex						itsColStart;
	JProgressDisplay*			itsPG;
	JProcess*					itsProcess;
	JArray<JIndex>*				itsCols;
	ProcessLink*				itsLink;

private:

	void		HandleInput(const JString& str);
	void		HandleDataRead(const JString& str);
	void		HandlePrepareCols(const JSize cols);
	
	// not allowed

	DataModule(const DataModule& source);
	const DataModule& operator=(const DataModule& source);


};

#endif
