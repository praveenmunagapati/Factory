#include "C:\Factory\Common\Options\SockServerTh.h"
#include "C:\Factory\Common\Options\SockStream.h"
#include "C:\Factory\Common\Options\CRRandom.h"

#define EV_STOP "{49e9f81c-dae4-464f-a209-301eed85b011}"

static uint64 KeepDiskFree = 2500000000ui64; // 2.5 GB
static char *RootDir = "C:\\appdata\\FilingCase3";
static char *DataDir;
static char *TempDir;
static uint EvStop;
static critical_t CritFileIO;

static int RecvPrmData(SockStream_t *ss, char *dataFile, uint64 dataSize) // ret: ? ����
{
	uint64 count;

	enterCritical(&CritFileIO);
	{
		FILE *fp = fileOpen(dataFile, "wb");

		for(count = 0; count < dataSize; count++)
		{
			int chr = SockRecvChar(ss);

			if(chr == EOF)
				break;

			writeChar(fp, chr);
		}
		fileClose(fp);
	}
	leaveCritical(&CritFileIO);

	return count == dataSize;
}
static void PerformTh(int sock, char *strip)
{
	SockStream_t *ss = CreateSockStream2(sock, 0, 30, 0);

	for(; ; )
	{
		char *command;
		char *path;
		char *sDataSize;
		uint64 dataSize;
		char *dataFile;

		command   = SockRecvLine(ss, 30);
		path      = SockRecvLine(ss, 1000);
		sDataSize = SockRecvLine(ss, 30);

		coutJLine_x(xcout("command: %s\n", command));
		coutJLine_x(xcout("path: %s\n", path));
		coutJLine_x(xcout("dataSize: %s\n", sDataSize));

		dataSize = toValue64(sDataSize);

		if(!isFairRelPath(path, strlen(RootDir)))
		{
			cout("�s���ȃp�X�ł��B\n");
			goto fault;
		}
		updateDiskSpace(RootDir[0]);

		if(lastDiskFree < KeepDiskFree)
		{
			cout("���s�ɕK�v�ȃf�B�X�N�̋󂫗̈悪�s�����Ă��܂��B\n");
			goto fault;
		}
		if(lastDiskFree - KeepDiskFree < dataSize)
		{
			cout("�f�B�X�N�̋󂫗̈悪�v���f�[�^�T�C�Y�ɑ΂��ĕs�����Ă��܂��B\n");
			goto fault;
		}
		dataFile = combine_cx(TempDir, MakeUUID(1));

		if(!RecvPrmData(ss, dataFile, dataSize))
		{
			cout("�f�[�^�̎�M�Ɏ��s���܂����B\n");
			goto fault2;
		}

		{
			char *ender = SockRecvLine(ss, 30);

			if(_stricmp(ender, "/e"))
			{
				cout("�s���ȏI�[�ł��B\n");
				memFree(ender);
				goto fault2;
			}
			memFree(ender);
		}

		if(!_stricmp(command, "GET"))
		{
			error(); // TODO
		}

		error(); // TODO

	fault2:
		removeFile(dataFile);
		memFree(dataFile);

	fault:
		memFree(command);
		memFree(path);
		memFree(sDataSize);
	}
	ReleaseSockStream(ss);
}
static int IdleTh(void)
{
	static int keep = 1;

	if(handleWaitForMillis(EvStop, 0))
		keep = 0;

	while(hasKey())
		if(getKey() == 0x1b)
			keep = 0;

	if(!keep)
		LOGPOS();

	return keep;
}
int main(int argc, char **argv)
{
	uint portNo = 65123;
	uint connectMax = 20;

readArgs:
	if(argIs("/P"))
	{
		portNo = toValue(nextArg());
		goto readArgs;
	}
	if(argIs("/C"))
	{
		connectMax = toValue(nextArg());
		goto readArgs;
	}
	if(argIs("/R"))
	{
		RootDir = nextArg();
		goto readArgs;
	}
	if(argIs("/D"))
	{
		KeepDiskFree = toValue64(nextArg());
		goto readArgs;
	}

	if(argIs("/S"))
	{
		LOGPOS();
		eventWakeup(EV_STOP);
		return;
	}

	cout("�|�[�g�ԍ�: %u\n", portNo);
	cout("�ő哯���ڑ���: %u\n", connectMax);
	cout("�m�ۂ���f�B�X�N�̋󂫗̈�: %I64u\n", KeepDiskFree);
	cout("RootDir.0: %s\n", RootDir);

	errorCase(!m_isRange(portNo, 1, 65535));
	errorCase(!m_isRange(connectMax, 1, IMAX));

	RootDir = makeFullPath(RootDir);
	RootDir = toFairFullPathFltr_x(RootDir);

	cout("RootDir.1: %s\n", RootDir);

	DataDir = combine(RootDir, "d");
	TempDir = combine(RootDir, "e");

	cout("DataDir: %s\n", DataDir);
	cout("TempDir: %s\n", TempDir);

	createPath(DataDir, 'd');
	createPath(TempDir, 'd');

	recurClearDir(TempDir);

	EvStop = eventOpen(EV_STOP);
	initCritical(&CritFileIO);

	sockServerTh(PerformTh, portNo, connectMax, IdleTh);

	memFree(RootDir);
	memFree(DataDir);
	memFree(TempDir);
	handleClose(EvStop);
	fnlzCritical(&CritFileIO);
}
