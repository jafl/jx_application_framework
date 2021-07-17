/******************************************************************************
 GLFitDescriptionList.h

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_GLFitDescriptionList
#define _H_GLFitDescriptionList

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

	bool		GetCurrentFitIndex(JIndex* index);
	virtual void	HandleKeyPress(const JUtf8Character& c,
								   const int keySym, const JXKeyModifiers& modifiers) override;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h) override;
	virtual void			PrepareDeleteXInputField() override;
	virtual bool		ExtractInputData(const JPoint& cell) override;

	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;

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

	static const JUtf8Byte* kFitSelected;
	static const JUtf8Byte* kFitInitiated;

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
