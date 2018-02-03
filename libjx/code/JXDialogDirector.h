/******************************************************************************
 JXDialogDirector.h

	Interface for the JXDialogDirector class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDialogDirector
#define _H_JXDialogDirector

#include <JXWindowDirector.h>

class JXButton;

class JXDialogDirector : public JXWindowDirector
{
public:

	JXDialogDirector(JXDirector* supervisor, const JBoolean modal);

	virtual ~JXDialogDirector();

	JBoolean	IsModal() const;
	JXButton*	GetOKButton() const;
	JXButton*	GetCancelButton() const;
	void		SetButtons(JXButton* okButton, JXButton* cancelButton);

	void	BeginDialog();
	void	EndDialog(const JBoolean success);

	virtual void		Activate();
	virtual JBoolean	Deactivate();

protected:

	void	UseModalPlacement(const JBoolean doIt);

	virtual JBoolean	OKToDeactivate();
	JBoolean			Cancelled() const;		// for use in OKToDeactivate()

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	const JBoolean	itsModalFlag;
	JBoolean		itsAutoGeomFlag;
	JBoolean		itsCancelFlag;
	JXButton*		itsOKButton;
	JXButton*		itsCancelButton;

private:

	// not allowed

	JXDialogDirector(const JXDialogDirector& source);
	const JXDialogDirector& operator=(const JXDialogDirector& source);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kDeactivated;

	class Deactivated : public JBroadcaster::Message
		{
		public:

			Deactivated(const JBoolean success)
				:
				JBroadcaster::Message(kDeactivated),
				itsSuccessFlag(success)
				{ };

			JBoolean
			Successful() const
			{
				return itsSuccessFlag;
			};

		private:

			const JBoolean	itsSuccessFlag;
		};
};


/******************************************************************************
 IsModal

 ******************************************************************************/

inline JBoolean
JXDialogDirector::IsModal()
	const
{
	return itsModalFlag;
}

/******************************************************************************
 BeginDialog

 ******************************************************************************/

inline void
JXDialogDirector::BeginDialog()
{
	Activate();
}

/******************************************************************************
 EndDialog

	If success is kJTrue, we close ourselves with success.
	Otherwise, we close as if cancelled.

	We don't bother to return the result of Deactivate() because, if it
	is successful, kJXDialogDeactivated will be broadcast.

 ******************************************************************************/

inline void
JXDialogDirector::EndDialog
	(
	const JBoolean success
	)
{
	itsCancelFlag = !success;
	if (!Deactivate())
		{
		itsCancelFlag = kJTrue;		// so WM_DELETE_WINDOW => cancel
		}
}

/******************************************************************************
 UseModalPlacement (protected)

	Modal dialogs are normally centered on the screen so the user will
	instantly notice them.  Call this with kJFalse to turn off this behavior.

 ******************************************************************************/

inline void
JXDialogDirector::UseModalPlacement
	(
	const JBoolean doIt
	)
{
	itsAutoGeomFlag = doIt;
}

/******************************************************************************
 Cancelled (protected)

 ******************************************************************************/

inline JBoolean
JXDialogDirector::Cancelled()
	const
{
	return itsCancelFlag;
}

/******************************************************************************
 Buttons

 ******************************************************************************/

inline JXButton*
JXDialogDirector::GetOKButton()
	const
{
	return itsOKButton;
}

inline JXButton*
JXDialogDirector::GetCancelButton()
	const
{
	return itsCancelButton;
}

#endif
