/******************************************************************************
 test_JMessageProtocol.cc

	Program to test JMessageProtocol class.

	Written by John Lindal.

 ******************************************************************************/

#include <JUnitTestManager.h>
#include <ace/reactor.h>
#include <ace/Acceptor.h>
#include <ace/LSOCK_Acceptor.h>
#include <ace/Connector.h>
#include <ace/LSOCK_Connector.h>
#include <ace/UNIX_Addr.h>
#include <ace/LSOCK_Stream.h>
#include <JMessageProtocol.h>
#include <JArray.h>
#include <jassert_simple.h>

int main()
{
	return JUnitTestManager::Execute();
}

static int server = 0;

class TestLink : public JMessageProtocol<ACE_LSOCK_STREAM>
{
public:

	TestLink();
};

typedef ACE_Acceptor<TestLink, ACE_LSOCK_ACCEPTOR>		TestAcceptor;
typedef ACE_Connector<TestLink, ACE_LSOCK_CONNECTOR>	TestConnector;
static const char* socketName = "/tmp/test_JMessageProtocol";

static const JUtf8Byte* message1 = "1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94";
static const JUtf8Byte* message2 = "\xC3\xA6\xCF\x86\xCF\x83" "me\xC3\x9F";

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

	Base(const Base& source);
	const Base& operator=(const Base& source);
};

class Server : public Base
{
public:

	Server(TestLink* link)
		:
		Base(link)
	{
		link->ShouldSendSynch();
	};

private:

	virtual void
	HandleMessage()
	{
		std::cout << "server state: " << itsState << std::endl;

		JString msg;
		if (itsState == 0)
			{
			JAssertTrue(itsLink->GetNextMessage(&msg));
			JAssertStringsEqual(message1, msg);
			JAssertFalse(itsLink->ReceivedDisconnect());

			itsLink->SendData(JString(message1, 13));	// force break in middle of UTF-8 character
			itsLink->SendMessage(JString(message1 + 13, 0));
			itsState++;
			}
		else if (itsState == 1)
			{
			JAssertTrue(itsLink->GetNextMessage(&msg));
			JAssertStringsEqual(message2, msg);
			JAssertFalse(itsLink->ReceivedDisconnect());

			itsLink->SendData(JString(message1, 13));	// force break in middle of UTF-8 character
			itsLink->SendMessage(JString(message1 + 13, 0));
			itsState++;
			}
	};

	virtual void
	HandleDisconnect()
	{
		std::cout << "server disconnect" << std::endl;
		ACE_Reactor::instance()->end_reactor_event_loop();
	};

	// not allowed

	Server(const Server& source);
	const Server& operator=(const Server& source);
};

TestLink::TestLink()
{
	std::cout << server << ": create TestLink" << std::endl;
	if (server)
	{
		Server* svr = jnew Server(this);
		assert( svr != NULL );
	}
};

void Listen()
{
	ACE_UNIX_Addr addr(socketName);
	TestAcceptor* acc = jnew TestAcceptor();
	assert( acc != NULL );
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
		link->SendMessage(JString(message1, 0, kJFalse));
	};

private:

	void
	HandleMessage()
	{
		std::cout << "client state: " << itsState << std::endl;

		JString msg;
		if (itsState == 0)
			{
			JAssertTrue(itsLink->GetNextMessage(&msg));
			JAssertStringsEqual(message1, msg);
			JAssertFalse(itsLink->ReceivedDisconnect());

			itsLink->SendMessage(JString(message2, 0, kJFalse));
			itsState++;
			}
		else if (itsState == 1)
			{
			JAssertTrue(itsLink->GetNextMessage(&msg));
			JAssertStringsEqual(message2, msg);
			JAssertFalse(itsLink->ReceivedDisconnect());

			itsLink->ShouldSendSynch();
			itsLink->SendDisconnect();
			ACE_Reactor::instance()->end_reactor_event_loop();
			itsState++;
			}
	};

	// not allowed

	Client(const Client& source);
	const Client& operator=(const Client& source);
};

void SendAndRecv()
{
	TestLink* link = new TestLink;
	assert( link != NULL );

	ACE_UNIX_Addr addr(socketName);
	TestConnector conn;
	int result = conn.connect(link, addr, ACE_Synch_Options::synch);
	JAssertEqual(0, result);

	Client* a = jnew Client(link);
	assert( a != NULL );
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
		sleep(1);	// wait for child to initialize
		SendAndRecv();
		}

	ACE_Reactor::instance()->run_reactor_event_loop();
}
