/******************************************************************************
 GLFitDescriptionList.h

	Copyright © 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GLFitDescriptionList
#define _H_GLFitDescriptionList

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXEditTable.h>
#include <J2DPlotWidget.h>

class JXInputField;
class JXImage;

class GLFitDescriptionList : public JXEditTable
{
public:

	GLFitDescriptionList(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~GLFitDescriptionList();

	JBoolean		GetCurrentFitIndex(JIndex* index);
	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h);
	virtual void			PrepareDeleteXInputField();
	virtual JBoolean		ExtractInputData(const JPoint& cell);

	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh);

private:

	JPtrArray<JString>*	itsNameList;	// we don't own the strings
	JCoordinate			itsMinColWidth;
	JXInputField*		itsInput;
	JXImage*			itsBuiltInIcon;
	JXImage*			itsNonLinearIcon;
	JXImage*			itsPolyIcon;
	JXImage*			itsExecutableIcon;

private:

	void	SyncWithManager();
	void	AdjustColWidth();

	// not allowed

	GLFitDescriptionList(const GLFitDescriptionList& source);
	const GLFitDescriptionList& operator=(const GLFitDescriptionList& source);

public:

	static const JCharacter* kFitSelected;
	static const JCharacter* kFitInitiated;

	class FitSelected : public JBroadcaster::Message
		{
		public:

			FitSelected(const JIndex index)
				:
				JBroadcaster::Message(kFitSelected),
				itsIndex(index)
				{ };

			JIndex
			GetIndex() const
			{
				return itsIndex;
			};

		private:

			JIndex itsIndex;
		};

	class FitInitiated : public JBroadcaster::Message
		{
		public:

			FitInitiated(const JIndex index)
				:
				JBroadcaster::Message(kFitInitiated),
				itsIndex(index)
				{ };

		JIndex
		GetIndex() const
		{
			return itsIndex;
		};

	private:

		JIndex itsIndex;
		};
};

#endif
