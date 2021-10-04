/******************************************************************************
 JXDialogDirector.h

	Interface for the JXDialogDirector class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDialogDirector
#define _H_JXDialogDirector

#include "jx-af/jx/JXWindowDirector.h"

class JXButton;

class JXDialogDirector : public JXWindowDirector
{
public:

	JXDialogDirector(JXDirector* supervisor, const bool modal);

	virtual ~JXDialogDirector();

	bool		IsModal() const;
	JXButton*	GetOKButton() const;
	JXButton*	GetCancelButton() const;
	void		SetButtons(JXButton* okButton, JXButton* cancelButton);

	void	BeginDialog();
	void	EndDialog(const bool success);

	void	Activate() override;
	bool	Deactivate() override;

protected:

	void	UseModalPlacement(const bool doIt);

	bool	OKToDeactivate() override;
	bool			Cancelled() const;		// for use in OKToDeactivate()

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	const bool	itsModalFlag;
	bool		itsAutoGeomFlag;
	bool		itsCancelFlag;
	JXButton*	itsOKButton;
	JXButton*	itsCancelButton;

public:

	// JBroadcaster messages

	static const JUtf8Byte* kDeactivated;

	class Deactivated : public JBroadcaster::Message
		{
		public:

			Deactivated(const bool success)
				:
				JBroadcaster::Message(kDeactivated),
				itsSuccessFlag(success)
				{ };

			bool
			Successful() const
			{
				return itsSuccessFlag;
			};

		private:

			const bool	itsSuccessFlag;
		};
};


/******************************************************************************
 IsModal

 ******************************************************************************/

inline bool
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

	If success is true, we close ourselves with success.
	Otherwise, we close as if cancelled.

	We don't bother to return the result of Deactivate() because, if it
	is successful, kJXDialogDeactivated will be broadcast.

 ******************************************************************************/

inline void
JXDialogDirector::EndDialog
	(
	const bool success
	)
{
	itsCancelFlag = !success;
	if (!Deactivate())
		{
		itsCancelFlag = true;		// so WM_DELETE_WINDOW => cancel
		}
}

/******************************************************************************
 UseModalPlacement (protected)

	Modal dialogs are normally centered on the screen so the user will
	instantly notice them.  Call this with false to turn off this behavior.

 ******************************************************************************/

inline void
JXDialogDirector::UseModalPlacement
	(
	const bool doIt
	)
{
	itsAutoGeomFlag = doIt;
}

/******************************************************************************
 Cancelled (protected)

 ******************************************************************************/

inline bool
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
