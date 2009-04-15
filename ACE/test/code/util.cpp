/******************************************************************************
 util.cpp

	Code used by both client and server.

	Written by John Lindal.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include "util.h"
#include <ace/SOCK_Stream.h>
#include <JString.h>
#include <jTime.h>
#include <jErrno.h>
#include <jAssert.h>

const JCharacter* kMsgToServer = "HI!  NICE DAY!\n";
const int kMsgToServerLength   = strlen(kMsgToServer);

const JCharacter* kMsgToClient =
	"Please get Loud Howard out of here!\n";
const int kMsgToClientLength = strlen(kMsgToClient);

const JCharacter kEndOfMessage = '\n';

/******************************************************************************
 TalkToClient

 ******************************************************************************/

JBoolean
TalkToClient
	(
	ACE_SOCK_Stream& socket
	)
{
	// Wait for message from client.

	const JBoolean ok = ReceiveMessage(socket);
	if (ok)
		{
		// This tests that clients are willing to wait.

		JWait(1.0);

		// Reply to the bothersome twit.

		if (socket.send_n(kMsgToClient, kMsgToClientLength) != kMsgToClientLength)
			{
			cerr << "error trying to send: " << jerrno() << endl;
			}
		}

	return ok;
}

/******************************************************************************
 TalkToServer

 ******************************************************************************/

void
TalkToServer
	(
	ACE_SOCK_Stream& socket
	)
{
	// This tests that the server is willing to wait.

	JWait(1.0);

	// Say hello to the nice server.

	if (socket.send_n(kMsgToServer, kMsgToServerLength) != kMsgToServerLength)
		{
		cerr << "error trying to send: " << jerrno() << endl;
		socket.close();
		exit(1);
		}

	// Wait for the server to reply.

	ReceiveMessage(socket);

	// Close the connection completely.

	if (socket.close() == -1) 
		{
		cerr << "error trying to close connection: " << jerrno() << endl;
		}
}

/******************************************************************************
 ReceiveMessage

	You should also check out recv(iovec*).

 ******************************************************************************/

JBoolean
ReceiveMessage
	(
	ACE_SOCK_Stream& socket
	)
{
	const JSize kBufSize = 50;
	char buffer [ kBufSize ];

	const ACE_Time_Value timeOut(5);	// wait up to 5 seconds

	JString msgFromServer;
	while (1)
		{
		const ssize_t count = socket.recv(buffer, kBufSize, 0, &timeOut);
		if (count == -1)
			{
			cerr << "error while waiting for message: " << jerrno() << endl;
			return kJFalse;
			}
		else if (count > 0)
			{
			cout << "received " << count << " bytes" << endl;
			msgFromServer.Append(buffer, count);

			if (msgFromServer.GetLastCharacter() == kEndOfMessage)
				{
				cout << "received: " << msgFromServer << endl;
				return kJTrue;
				}
			}
		}
}
