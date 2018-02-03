/******************************************************************************
 GMAccountList.h

	Interface for the GMAccountList class

	Copyright (C) 1999 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_GMAccountList
#define _H_GMAccountList

#include <JXEditTable.h>

class JXInputField;
class GMAccountDialog;
class GMAccount;

class GMAccountList : public JXEditTable
{
public:

	static GMAccountList*
	Create(GMAccountDialog* dialog, JPtrArray<GMAccount>* accounts,
					JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);
	virtual ~GMAccountList();

	JIndex			GetCurrentIndex();
	void			SetCurrentAccount(const JIndex index);
	void			SetCurrentAsDefault();

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers) override;

protected:

	GMAccountList(GMAccountDialog* dialog, JPtrArray<GMAccount>* accounts,
					JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h);
	virtual void			PrepareDeleteXInputField();
	virtual JBoolean		ExtractInputData(const JPoint& cell);

private:

	JCoordinate			itsMinColWidth;
	JXInputField*		itsInput;
	GMAccountDialog*	itsDialog;
	JSize				itsLineHeight;

	JPtrArray<GMAccount>*	itsAccountInfo;

private:

	void	GMAccountListX();
	void	AdjustColWidth();

	// not allowed

	GMAccountList(const GMAccountList& source);
	const GMAccountList& operator=(const GMAccountList& source);

public:

	static const JCharacter* kNameSelected;

	class NameSelected : public JBroadcaster::Message
	{
	public:

		NameSelected(const JIndex index)
			:
			JBroadcaster::Message(kNameSelected),
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
