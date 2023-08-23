#include <string.h>
#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm> 
#include <functional> 

#include "IQDataStore.h"

using namespace std;


// Another Process
int Send()
{
	IQDataStore iQDataStore;
	iQDataStore.MakeSharedMemory();

	pMsgBlock->read_byte = 0;
	pMsgBlock->write_byte = 0;

	while (1)
	{

		int readed = srcFileIO.Read(SharedPtr, 1024 * 1024 * 1024, 1024 * 1024 * 1024);
		pMsgBlock->read_byte = readed;

		sem_post(&pMsgBlock->sem_Wait);

		sem_wait(&pMsgBlock->sem_Done);


		if (readed <= 0) {
			break;
		}

	}

	iQDataStore.ReleaseSharedMemory();


	return 0;
}


// Another Process
int Recv()
{

	IQDataStore iQDataStore;
	iQDataStore.MakeSharedMemory();
	iQDataStore.PrepareSemaphore();

	SharedMsgBlock* pMsgBlock = (SharedMsgBlock*)iQDataStore.m_shmaddr;
	char* SharedPtr = iQDataStore.m_shmaddr + sizeof(SharedMsgBlock);

	while (1)
	{
		sem_wait(&pMsgBlock->sem_Wait);

		printf("Recv %d (Shared Memory)\n", pMsgBlock->read_byte);

		if (pMsgBlock->read_byte > 0)
		{
			fwrite(SharedPtr, 1, pMsgBlock->read_byte, fp);
			printf("Write SSD %d\n", pMsgBlock->read_byte);
			pMsgBlock->read_byte = 0;
		}
		else
		{
			sem_post(&pMsgBlock->sem_Done);
			break;
		}

		sem_post(&pMsgBlock->sem_Done);

		usleep(1);
	}

	printf("fclose ok \n");

	fclose(fp);

	iQDataStore.ReleaseSharedMemory();

	return 0;
}

