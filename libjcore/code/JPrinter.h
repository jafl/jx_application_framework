/******************************************************************************
 JPrinter.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JPrinter
#define _H_JPrinter

#include <JBroadcaster.h>

class JPrinter : virtual public JBroadcaster
{
public:

	JPrinter();

	virtual ~JPrinter();

private:

	// not allowed

	JPrinter(const JPrinter& source);
	const JPrinter& operator=(const JPrinter& source);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kPageSetupFinished;
	static const JUtf8Byte* kPrintSetupFinished;

	class PageSetupFinished : public JBroadcaster::Message
		{
		public:

			PageSetupFinished(const JBoolean changed)
				:
				JBroadcaster::Message(kPageSetupFinished),
				itsChangedFlag(changed)
				{ };

			JBoolean
			Changed() const
			{
				return itsChangedFlag;
			};

		private:

			const JBoolean	itsChangedFlag;
		};

	class PrintSetupFinished : public JBroadcaster::Message
		{
		public:

			PrintSetupFinished(const JBoolean success, const JBoolean changed)
				:
				JBroadcaster::Message(kPrintSetupFinished),
				itsSuccessFlag(success), itsChangedFlag(changed)
				{ };

			JBoolean
			Successful() const
			{
				return itsSuccessFlag;
			};

			JBoolean
			Changed() const
			{
				return itsChangedFlag;
			};

		private:

			const JBoolean	itsSuccessFlag;
			const JBoolean	itsChangedFlag;
		};
};

#endif
