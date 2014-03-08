/*********************************************************************************
 FitModule.h

	Interface for the FitModule class.

	Copyright @ 1997 by Glenn W. Bach. All rights reserved.

 ********************************************************************************/

#ifndef _H_FitModule
#define _H_FitModule

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPtrArray.h>

#include <j_prep_ace.h>
#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>
#include <JMessageProtocol.h>	// template which requires ace includes

class PlotDir;
class JProgressDisplay;
class JProcess;
class JString;
class JPlotDataBase;
class JOutPipeStream;

class FitModule : virtual public JBroadcaster
{
public:

	static JBoolean Create(	FitModule** module,
							PlotDir* dir,
							JPlotDataBase* fitData,
							const JCharacter* sysCmd);
	virtual ~FitModule();

protected:

	FitModule(	PlotDir* dir, JPlotDataBase* fitData, JProcess* process,
				const int fd, JOutPipeStream* output);

	virtual void	Receive(JBroadcaster* sender,
						const JBroadcaster::Message& message);

private:

	typedef JMessageProtocol<ACE_LSOCK_STREAM>	ProcessLink;

private:

	PlotDir* 					itsDir;			// We don't own this.
	JPlotDataBase*				itsData;		// We don't own this.
	JBoolean					itsStatusRead;
	JBoolean					itsHeaderRead;
	JBoolean					itsFunctionRead;
	JBoolean					itsHasErrors;
	JBoolean					itsHasGOF;
	JSize						itsParmsCount;
	JProgressDisplay*			itsPG;
	JProcess*					itsProcess;
	JArray<JFloat>*				itsValues;
	JPtrArray<JString>*			itsNames;
	JString*					itsFunction;
	ProcessLink*				itsLink;

private:

	void		HandleInput(JString& str);
	void		HandleDataRead(JString& str);
	void		HandleFit();

	// not allowed

	FitModule(const FitModule& source);
	const FitModule& operator=(const FitModule& source);


};

#endif

