/******************************************************************************
 GLPlotter.h

	Interface for the GLPlotter class

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_GLPlotter
#define _H_GLPlotter

#include <JX2DPlotWidget.h>
#include <JArray.h>

#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>
#include <JMessageProtocol.h>	// template which requires ace includes

class JXTextMenu;
class GLHistoryDir;
class JProcess;
class JOutPipeStream;

class GLPlotter : public JX2DPlotWidget
{
public:

	GLPlotter(GLHistoryDir* sessionDir,
						JXMenuBar* menuBar, JXContainer* enclosure,
						const HSizingOption hSizing, const VSizingOption vSizing,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h);

	virtual ~GLPlotter();
	
protected:

	virtual void	Receive(JBroadcaster* sender,
								const JBroadcaster::Message& message);

private:

	typedef JMessageProtocol<ACE_LSOCK_STREAM>	ProcessLink;

private:

	JXTextMenu*			itsModuleMenu;
	GLHistoryDir*	itsSessionDir;
	JProcess*			itsCursorProcess;
	ProcessLink*		itsLink;
	bool			itsIsProcessingCursor;
	bool			itsCursorFirstPass;
	
	
private:

	void			UpdateModuleMenu();
	void			HandleModuleMenu(const JIndex index);

	// not allowed

	GLPlotter(const GLPlotter& source);
	const GLPlotter& operator=(const GLPlotter& source);
	
public:

};


#endif
