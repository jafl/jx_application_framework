/******************************************************************************
 test_JMessageProtocol.cpp

	Program to test JMessageProtocol class.

	Written by John Lindal.

 ******************************************************************************/

#include "JTestManager.h"
#include <ace/Reactor.h>
#include <ace/Acceptor.h>
#include <ace/LSOCK_Acceptor.h>
#include <ace/Connector.h>
#include <ace/LSOCK_Connector.h>
#include <ace/UNIX_Addr.h>
#include <ace/LSOCK_Stream.h>
#include "JMessageProtocol.h"
#include "JArray.h"
#include "jTime.h"
#include "jAssert.h"

int main()
{
	return JTestManager::Execute();
}

static int server = 0;

class TestLink : public JMessageProtocol<ACE_LSOCK_STREAM>
{
public:

	TestLink();

	void	SendBytes(const JUtf8Byte* data);	// allow broken up UTF-8 characters
};

using TestAcceptor  = ACE_Acceptor<TestLink, ACE_LSOCK_ACCEPTOR>;
using TestConnector = ACE_Connector<TestLink, ACE_LSOCK_CONNECTOR>;

static const char* socketName = "/tmp/test_JMessageProtocol";

static const JUtf8Byte* serverSend[] =
{
	// force break in middle of UTF-8 character

	"1234567890\xC2\xA9\xC3\x85\xC3", "\xA5\xE2\x9C\x94\n", nullptr,
	"\xC3\xA6\xCF\x86\xCF", "\x83" "me\xC3\x9F\na",

	// test receiving several messages at once

	"bc\nab", "c\ndef\ngih\nke", "r\n", nullptr,
	nullptr
};

static const JUtf8Byte* message[] =
{
	"1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94",
	"\xC3\xA6\xCF\x86\xCF\x83" "me\xC3\x9F",
	"abc",
	"abc",
	"def",
	"gih",
	"ker",
	nullptr
};

class Base : virtual public JBroadcaster
{
public:

	Base(TestLink* link)
		:
		itsLink(link), itsState(0)
	{
		ListenTo(itsLink);
	};

protected:

	virtual void
	Receive
		(
		JBroadcaster*	sender,
		const Message&	message
		)
	{
		if (sender == itsLink && message.Is(JMessageProtocolT::kMessageReady))
		{
			HandleMessage();
		}
		else if (sender == itsLink && message.Is(JMessageProtocolT::kReceivedDisconnect))
		{
			HandleDisconnect();
		}
		else
		{
			JBroadcaster::Receive(sender, message);
		}
	};

protected:

	TestLink*	itsLink;
	int			itsState;

private:

	virtual void	HandleMessage() = 0;

	virtual void
	HandleDisconnect()
	{ };

	// not allowed

	Base(const Base&) = delete;
	Base& operator=(const Base&) = delete;
};

class Server : public Base
{
public:

	Server(TestLink* link)
		:
		Base(link),
		itsSendOffset(0)
	{
		link->ShouldSendSynch();
	};

private:

	int itsSendOffset;

private:

	virtual void
	HandleMessage()
	{
		JAssertTrue(itsLink->HasMessages());
		JAssertFalse(itsLink->ReceivedDisconnect());

		JString msg;
		JAssertFalse(itsLink->PeekPartialMessage(&msg));
		JAssertTrue(itsLink->PeekNextMessage(&msg));
		JAssertTrue(itsLink->GetNextMessage(&msg));		// pull from queue
		std::cout << "server received: " << msg << std::endl;

		JAssertStringsEqual(message[ itsState ], msg);
		itsState++;

		if (serverSend[ itsSendOffset ] == nullptr)
		{
			return;
		}

		while (serverSend[ itsSendOffset ] != nullptr)
		{
			itsLink->SendBytes(serverSend[ itsSendOffset ]);
			std::cout << "server sent: " << serverSend[ itsSendOffset ] << std::endl;
			itsSendOffset++;
			JWait(0.5);
		}

		if (serverSend[ itsSendOffset+1 ] != nullptr)	// prepare to send next batch
		{
			itsSendOffset++;
		}
	};

	virtual void
	HandleDisconnect()
	{
		std::cout << "server disconnect" << std::endl;
		ACE_Reactor::instance()->end_reactor_event_loop();
	};
};

TestLink::TestLink()
{
	std::cout << server << ": create TestLink" << std::endl;
	if (server)
{
		jnew Server(this);
}
};

void
TestLink::SendBytes
	(
	const JUtf8Byte* data
	)
{
	JNetworkProtocolBase::Send(data, strlen(data));
};

void Listen()
{
	ACE_UNIX_Addr addr(socketName);
	auto* acc = jnew TestAcceptor();
	const int result = acc->open(addr);
	JAssertEqual(0, result);
}

class Client : public Base
{
public:

	Client(TestLink* link)
		:
		Base(link)
	{
		link->SendMessage(JString(message[0], JString::kNoCopy));
	};

private:

	void
	HandleMessage()
	{
		JAssertTrue(itsLink->HasMessages());
		JAssertFalse(itsLink->ReceivedDisconnect());

		JString msg;
		JAssertTrue(itsLink->PeekNextMessage(&msg));
		JAssertTrue(itsLink->GetNextMessage(&msg));		// pull from queue
		std::cout << "client received: " << msg << std::endl;

		JAssertStringsEqual(message[ itsState ], msg);
		itsState++;

		if (message[ itsState ] == nullptr)
		{
			itsLink->ShouldSendSynch();
			itsLink->SendDisconnect();
			ACE_Reactor::instance()->end_reactor_event_loop();
			JWait(5);	// wait for server to finish
			return;
		}

		itsLink->SendMessage(JString(message[ itsState ], JString::kNoCopy));
		std::cout << "client sent: " << message[ itsState ] << std::endl;
	};
};

void SendAndRecv()
{
	auto* link = new TestLink;
	assert( link != nullptr );

	ACE_UNIX_Addr addr(socketName);
	TestConnector conn;
	int result = conn.connect(link, addr, ACE_Synch_Options::synch);
	JAssertEqual(0, result);

	jnew Client(link);
}

JTEST(SendRecv)
{
	unlink(socketName);

	int result = ACE_OS::fork();
	if (result == 0)
	{
		server = 1;
		Listen();
	}
	else
	{
		JWait(1);	// wait for child to initialize
		SendAndRecv();
	}

	ACE_Reactor::instance()->run_reactor_event_loop();
	unlink(socketName);
}
