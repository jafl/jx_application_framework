/******************************************************************************
 CMVarCommand.h

	Copyright � 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMVarCommand
#define _H_CMVarCommand

#include "CMCommand.h"
#include <JBroadcaster.h>

class CMVarNode;

class CMVarCommand : public CMCommand, virtual public JBroadcaster
{
public:

	CMVarCommand();

	virtual	~CMVarCommand();

	const JString&	GetData() const;

protected:

	void	SetData(const JString& data);

	virtual void	HandleFailure();

private:

	JString	itsData;

private:

	// not allowed

	CMVarCommand(const CMVarCommand& source);
	const CMVarCommand& operator=(const CMVarCommand& source);

public:

	// JBroadcaster messages

	static const JCharacter* kValueUpdated;
	static const JCharacter* kValueFailed;

	class ValueMessage : public JBroadcaster::Message
		{
		public:

			ValueMessage(const JCharacter* type, CMVarNode* root)
				:
				JBroadcaster::Message(type),
				itsRootNode(root)
				{ };

		public:

			CMVarNode*
			GetRootNode() const
			{
				return itsRootNode;
			}

		private:

			CMVarNode*	itsRootNode;
		};
};


/******************************************************************************
 GetData

 ******************************************************************************/

inline const JString&
CMVarCommand::GetData()
	const
{
	return itsData;
}

/******************************************************************************
 SetData (protected)

 ******************************************************************************/

inline void
CMVarCommand::SetData
	(
	const JString& data
	)
{
	itsData = data;
}

#endif
