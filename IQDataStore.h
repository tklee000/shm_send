#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "define.h"
#include <semaphore.h>


using namespace std;

typedef struct _SharedMsgBlock {
	sem_t sem_Wait;
	sem_t sem_Done;
	uint64_t read_byte;
	uint64_t write_byte;
}SharedMsgBlock;


class IQDataStore
{
public:
	IQDataStore();
	~IQDataStore();

	int MakeSharedMemory();
	int ReleaseSharedMemory();
	int PrepareSemaphore();

	char *m_shmaddr;

private:
	int m_shmid;
	
};
