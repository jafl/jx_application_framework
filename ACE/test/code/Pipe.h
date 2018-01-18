/******************************************************************************
 Pipe.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_Pipe
#define _H_Pipe

#include <JBroadcaster.h>
#include <JString.h>
#include <ace/Svc_Handler.h>
#include <ace/Synch_Traits.h>

template <ACE_PEER_STREAM_1>
class Pipe : public ACE_Svc_Handler<ACE_PEER_STREAM_2, ACE_SYNCH>
{
public:

	Pipe();
	Pipe(const ACE_HANDLE fd);

	virtual ~Pipe();

	void	Dump();

	// ACE_Svc_Handler functions

	virtual int	handle_input(ACE_HANDLE) override;

private:

	JString		itsData;

	JCharacter*	itsBuffer;				// buffer to receive raw bytes
	JSize		itsBufferSize;

private:

	void	PipeX();

	// not allowed

	Pipe(const Pipe& source);
	Pipe& operator=(const Pipe& source);
};

#include "Pipe.tmpl"

#endif
