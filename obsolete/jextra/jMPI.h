#ifdef HAS_MPI

/******************************************************************************
 jMPI.h

	Interface for jMPI.cc

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jMPI
#define _H_jMPI

#include <mpi.h>

typedef int	MPIProcessID;

#define MPI_INDEX			MPI_UNSIGNED_LONG
#define MPI_SIZE			MPI_UNSIGNED_LONG
#define MPI_NUMERICVALUE	MPI_DOUBLE
#define MPI_PROCID			MPI_INT

#include <jTypes.h>

class std::fstream;
class JString;
class JStaticBuffer;

int MPI_Send_JBoolean(const JBoolean& jbool, int dest, int tag, MPI_Comm comm);
int MPI_Recv_JBoolean(JBoolean* jbool, int source, int tag, MPI_Comm comm, MPI_Status* status);
int MPI_Bcast_JBoolean(JBoolean* jbool, int root, MPI_Comm comm);

int MPI_Send_JString(const JString& str, int dest, int tag, MPI_Comm comm);
int MPI_Recv_JString(JString* str, int source, int tag, MPI_Comm comm, MPI_Status* status);

int MPI_Send_Buffer(const JStaticBuffer& buffer, int dest, int tag, MPI_Comm comm);
int MPI_Recv_Buffer(JStaticBuffer* buffer, int source, int tag, MPI_Comm comm, MPI_Status* status);

int MPI_Send_File(std::fstream& file, int dest, int tag, MPI_Comm comm);
int MPI_Recv_File(std::fstream& file, int source, int tag, MPI_Comm comm, MPI_Status* status);

#endif

#endif
