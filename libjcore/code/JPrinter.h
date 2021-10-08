/******************************************************************************
 JPrinter.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JPrinter
#define _H_JPrinter

#include "jx-af/jcore/JBroadcaster.h"

class JPrinter : virtual public JBroadcaster
{
public:

	JPrinter();

	~JPrinter() override;

private:

	// not allowed

	JPrinter(const JPrinter&) = delete;
	JPrinter& operator=(const JPrinter&) = delete;

public:

	// JBroadcaster messages

	static const JUtf8Byte* kPageSetupFinished;
	static const JUtf8Byte* kPrintSetupFinished;

	class PageSetupFinished : public JBroadcaster::Message
		{
		public:

			PageSetupFinished(const bool changed)
				:
				JBroadcaster::Message(kPageSetupFinished),
				itsChangedFlag(changed)
				{ };

			bool
			Changed() const
			{
				return itsChangedFlag;
			};

		private:

			const bool	itsChangedFlag;
		};

	class PrintSetupFinished : public JBroadcaster::Message
		{
		public:

			PrintSetupFinished(const bool success, const bool changed)
				:
				JBroadcaster::Message(kPrintSetupFinished),
				itsSuccessFlag(success), itsChangedFlag(changed)
				{ };

			bool
			Successful() const
			{
				return itsSuccessFlag;
			};

			bool
			Changed() const
			{
				return itsChangedFlag;
			};

		private:

			const bool	itsSuccessFlag;
			const bool	itsChangedFlag;
		};
};

#endif
