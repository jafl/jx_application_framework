#ifdef HAS_MPI

/******************************************************************************
 jMPI.cpp

								Extensions to MPI

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include <jMPI.h>
#include <JString.h>
#include <JStaticBuffer.h>
#include <jFStreamUtil.h>
#include <jMemory.h>
#include <string.h>
#include <limits.h>
#include <jAssert.h>

// Private routines

int MPI_Send_Buffer(const JCharacter* buf, const JSize length, int dest, int tag, MPI_Comm comm);
int MPI_Recv_Buffer(JCharacter** buf, int source, int tag, MPI_Comm comm, MPI_Status* status);

/******************************************************************************
 MPI_Send_JBoolean

	We send a JBoolean as an int.

 ******************************************************************************/

int
MPI_Send_JBoolean
	(
	const JBoolean&	jbool,
	int				dest,
	int				tag,
	MPI_Comm		comm
	)
{
	int data = jbool;
	return MPI_Send(&data, 1, MPI_INT, dest, tag, comm);
}

/******************************************************************************
 MPI_Recv_JBoolean

	We can receive any int as a JBoolean.

 ******************************************************************************/

int
MPI_Recv_JBoolean
	(
	JBoolean*		jbool,
	int				source,
	int				tag,
	MPI_Comm		comm,
	MPI_Status*		status
	)
{
	int data;
	int result = MPI_Recv(&data, 1, MPI_INT, source, tag, comm, status);
	*jbool = ConvertToBoolean(data);
	return result;
}

/******************************************************************************
 MPI_Bcast_JBoolean

 ******************************************************************************/

int
MPI_Bcast_JBoolean
	(
	JBoolean*	jbool,
	int			root,
	MPI_Comm	comm
	)
{
	int data = *jbool;
	int result = MPI_Bcast(&data, 1, MPI_INT, root, comm);
	*jbool = ConvertToBoolean(data);
	return result;
}

/******************************************************************************
 MPI_Send_JString

	We send a JString as a string of characters.

 ******************************************************************************/

int
MPI_Send_JString
	(
	const JString&	str,
	int				dest,
	int				tag,
	MPI_Comm		comm
	)
{
	return MPI_Send_Buffer(str, str.GetLength(), dest, tag, comm);
}

/******************************************************************************
 MPI_Recv_JString

	We can receive any string of characters as a JString.

 ******************************************************************************/

int
MPI_Recv_JString
	(
	JString*		str,
	int				source,
	int				tag,
	MPI_Comm		comm,
	MPI_Status*		status
	)
{
	JCharacter* buf = NULL;
	int result = MPI_Recv_Buffer(&buf, source, tag, comm, status);
	*str = JString(buf);
	delete [] buf;
	return result;
}

/******************************************************************************
 MPI_Send_Buffer

 ******************************************************************************/

int
MPI_Send_Buffer
	(
	const JStaticBuffer&	buffer,
	int						dest,
	int						tag,
	MPI_Comm				comm
	)
{
	return MPI_Send_Buffer(buffer.GetData(), buffer.GetLength(), dest, tag, comm);
}

/******************************************************************************
 MPI_Recv_Buffer

 ******************************************************************************/

int
MPI_Recv_Buffer
	(
	JStaticBuffer*	buffer,
	int				source,
	int				tag,
	MPI_Comm		comm,
	MPI_Status*		status
	)
{
	JCharacter* buf = NULL;
	int result = MPI_Recv_Buffer(&buf, source, tag, comm, status);
	buffer->SetData(buf);
	return result;
}

/******************************************************************************
 MPI_Send_File

 ******************************************************************************/

int
MPI_Send_File
	(
	std::fstream&	file,
	int			dest,
	int			tag,
	MPI_Comm	comm
	)
{
	const std::streampos origPos = file.tellg();

	const JSize fileLength = GetFStreamLength(file);
	int result = MPI_Send(&fileLength, 1, MPI_SIZE, dest, tag, comm);

	JSize bufferSize = 10000;
	if (bufferSize > fileLength)
		{
		bufferSize = fileLength;
		}
	JCharacter* buf = CreateBuffer(&bufferSize);
	result = MPI_Send(&bufferSize, 1, MPI_SIZE, dest, tag, comm);

	file.seekg(0);
	JSize bytesRead = 0;
	while (bytesRead < fileLength)
		{
		if (bytesRead + bufferSize > fileLength)
			{
			bufferSize = fileLength - bytesRead;
			}
		file.read(buf, bufferSize);
		bytesRead += bufferSize;
		result = MPI_Send(buf, bufferSize, MPI_CHAR, dest, tag, comm);
		}

	file.seekg(origPos);

	delete [] buf;
	return result;
}

/******************************************************************************
 MPI_Recv_File

 ******************************************************************************/

int
MPI_Recv_File
	(
	std::fstream&	file,
	int			source,
	int			tag,
	MPI_Comm	comm,
	MPI_Status*	status
	)
{
	int result;
	JSize fileLength, bufferSize;
	result = MPI_Recv(&fileLength, 1, MPI_SIZE, source, tag, comm, status);
	result = MPI_Recv(&bufferSize, 1, MPI_SIZE, source, tag, comm, status);

	JCharacter* buf = new JCharacter[ bufferSize ];
	assert( buf != NULL );

	file.seekg(0);
	JSize bytesReceived = 0;
	while (bytesReceived < fileLength)
		{
		if (bytesReceived + bufferSize > fileLength)
			{
			bufferSize = fileLength - bytesReceived;
			}
		result = MPI_Recv(buf, bufferSize, MPI_CHAR, source, tag, comm, status);
		bytesReceived += bufferSize;
		file.write(buf, bufferSize);
		}

	file.seekg(0);

	delete [] buf;
	return result;
}

/******************************************************************************
 MPI_Send_Buffer (private)

 ******************************************************************************/

int
MPI_Send_Buffer
	(
	const JCharacter*	buf,
	const JSize			length,
	int					dest,
	int					tag,
	MPI_Comm			comm
	)
{
	assert( length <= INT_MAX );
	return MPI_Send((void*) buf, (int) length, MPI_CHAR, dest, tag, comm);
}

/******************************************************************************
 MPI_Recv_Buffer (private)

	buf is allocated with new[] and is zero terminated.

 ******************************************************************************/

int
MPI_Recv_Buffer
	(
	JCharacter**	buf,
	int				source,
	int				tag,
	MPI_Comm		comm,
	MPI_Status*		status
	)
{
	int length;
	MPI_Probe(source, tag, comm, status);
	MPI_Get_count(status, MPI_CHAR, &length);
	*buf = new JCharacter[ length+1 ];
	assert( *buf != NULL );
	int result = MPI_Recv(*buf, length, MPI_CHAR, source, tag, comm, status);
	(*buf)[ length ] = '\0';
	return result;
}

#endif
