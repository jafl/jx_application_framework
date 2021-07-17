/*********************************************************************************
 GLDataModule.h

	Interface for the GLDataModule class.
 
	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/
 
#ifndef _H_GLDataModule
#define _H_GLDataModule

#include <JArray.h>

#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>
#include <JMessageProtocol.h>	// template which requires ace includes

class GLRaggedFloatTable;
class GLRaggedFloatTableData;
class JProcess;
class JOutPipeStream;
//class JIPCIOStream;
class JProgressDisplay;
class JString;

class GLDataModule : virtual public JBroadcaster
{
public:

	static bool Create(	GLDataModule** module,
							GLRaggedFloatTable* table, 
							GLRaggedFloatTableData* data,
							const JString& sysCmd);
	virtual ~GLDataModule();

protected:

	GLDataModule(	GLRaggedFloatTable* table, GLRaggedFloatTableData* data,
				JProcess* process, const int fd, JOutPipeStream* output);

	virtual void	Receive(JBroadcaster* sender,
						const JBroadcaster::Message& message);

private:

	typedef JMessageProtocol<ACE_LSOCK_STREAM>	ProcessLink;

private:

	GLRaggedFloatTable* 		itsTable;
	GLRaggedFloatTableData*		itsData;
	JOutPipeStream*				itsOutput;
	bool					itsHeaderRead;
	bool					itsSentData;
	bool					itsDataIsDump;
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

	GLDataModule(const GLDataModule& source);
	const GLDataModule& operator=(const GLDataModule& source);


};

#endif
