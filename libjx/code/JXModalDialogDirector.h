/******************************************************************************
 JXModalDialogDirector.h

	Interface for the JXModalDialogDirector class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDialogDirector
#define _H_JXDialogDirector

#include "JXWindowDirector.h"
#include <boost/fiber/condition_variable.hpp>

class JXButton;

class JXModalDialogDirector : public JXWindowDirector
{
public:

	JXModalDialogDirector(const bool allowResizing = false);

	~JXModalDialogDirector() override;

	JXButton*	GetOKButton() const;
	JXButton*	GetCancelButton() const;
	void		SetButtons(JXButton* okButton, JXButton* cancelButton);

	bool	DoDialog();
	void	EndDialog(const bool success);

	void	Activate() override;
	bool	Deactivate() override;
	bool	Close() override;

protected:

	bool	OKToDeactivate() override;
	bool	Cancelled() const;		// for use in OKToDeactivate()

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	const bool	itsAllowResizeFlag;
	bool		itsCancelFlag;
	JXButton*	itsOKButton;
	JXButton*	itsCancelButton;

	boost::fibers::condition_variable	itsCondition;
	boost::fibers::mutex				itsMutex;
	bool								itsDoneFlag;
	bool*								itsDeletedFlag;
};


/******************************************************************************
 Cancelled (protected)

 ******************************************************************************/

inline bool
JXModalDialogDirector::Cancelled()
	const
{
	return itsCancelFlag;
}

/******************************************************************************
 Buttons

 ******************************************************************************/

inline JXButton*
JXModalDialogDirector::GetOKButton()
	const
{
	return itsOKButton;
}

inline JXButton*
JXModalDialogDirector::GetCancelButton()
	const
{
	return itsCancelButton;
}

#endif
