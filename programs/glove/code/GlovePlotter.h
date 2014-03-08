/******************************************************************************
 GlovePlotter.h

	Interface for the GlovePlotter class

	Copyright © 1997 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GlovePlotter
#define _H_GlovePlotter

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JX2DPlotWidget.h>
#include <JArray.h>

#include <j_prep_ace.h>
#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>
#include <JMessageProtocol.h>	// template which requires ace includes

class JXTextMenu;
class GloveHistoryDir;
class JProcess;
class JOutPipeStream;

class GlovePlotter : public JX2DPlotWidget
{
public:

	GlovePlotter(GloveHistoryDir* sessionDir,
						JXMenuBar* menuBar, JXContainer* enclosure,
						const HSizingOption hSizing, const VSizingOption vSizing,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h);

	virtual ~GlovePlotter();
	
protected:

	virtual void	Receive(JBroadcaster* sender,
								const JBroadcaster::Message& message);

private:

	typedef JMessageProtocol<ACE_LSOCK_STREAM>	ProcessLink;

private:

	JXTextMenu*			itsModuleMenu;
	GloveHistoryDir*	itsSessionDir;
	JProcess*			itsCursorProcess;
	ProcessLink*		itsLink;
	JBoolean			itsIsProcessingCursor;
	JBoolean			itsCursorFirstPass;
	
	
private:

	void			UpdateModuleMenu();
	void			HandleModuleMenu(const JIndex index);

	// not allowed

	GlovePlotter(const GlovePlotter& source);
	const GlovePlotter& operator=(const GlovePlotter& source);
	
public:

};


#endif
