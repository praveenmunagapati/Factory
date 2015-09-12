/*
	ncp.exe [/S SERVER-DOMAIN] [/P SERVER-PORT] [/F] ...

		SERVER-DOMAIN ... �T�[�o�[�h���C���A�f�t�H���g�� localhost
		SERVER-PORT   ... �T�[�o�[�|�[�g�ԍ��A�f�t�H���g�� 60022
		/F            ... (�T�[�o�[����)�����㏑�����[�h�A/UP, /MV �̂Ƃ���p����B

	ncp.exe ... /UP LOCAL-PATH SERVER-PATH

		�t�@�C���E�f�B���N�g���̃A�b�v���[�h

	ncp.exe ... /DL LOCAL-PATH SERVER-PATH

		�t�@�C���E�f�B���N�g���̃_�E�����[�h

	ncp.exe ... /SV SERVER-PATH

		�t�@�C���E�f�B���N�g���̃T�C�Y�𓾂�B

	ncp.exe ... /MV SERVER-PATH-1 SERVER-PATH-2

		�t�@�C���E�f�B���N�g�����ړ�����B

	ncp.exe ... /RM SERVER-PATH

		�t�@�C���E�f�B���N�g�����폜����B
*/

#include "C:\Factory\Common\Options\SockClient.h"
#include "C:\Factory\Common\Options\DirToStream.h"
#include "C:\Factory\Common\Options\PadFile.h"

static uint MD5Counter;

static void MD5Interrupt(void)
{
	MD5Counter++;
	execute_x(xcout("TITLE ncp - MD5 %u", MD5Counter));
}

static char *ServerDomain = "localhost";
static uint ServerPort = 60022;
static uint RetryCount = 2;
static char *PrmFile;
static FILE *PrmFp;
static char *AnsFile;
static FILE *AnsFp;
static int ForceOverwriteMode;
static int RequestAborted;

#define GetSockFileCounter(sf) \
	((sf)->Counter + (uint64)((sf)->Block ? (sf)->Block->Counter : 0))

#define GetSockFileCntrMax(sf) \
	(m_max((sf)->FileSize, 1ui64))

static int Idle(void)
{
	if(pulseSec(3, NULL))
	{
		uint64 prmcnt = GetSockFileCounter(sockClientStatus.PrmFile);
		uint64 prmmax = GetSockFileCntrMax(sockClientStatus.PrmFile);
		uint64 anscnt = GetSockFileCounter(sockClientStatus.AnsFile);
		uint64 ansmax = GetSockFileCntrMax(sockClientStatus.AnsFile);

		execute_x(xcout("TITLE ncp - %I64u / %I64u (%u) %I64u / %I64u (%u)"
			,prmcnt
			,prmmax
			,(uint)((prmcnt * 100ui64) / prmmax)
			,anscnt
			,ansmax
			,(uint)((anscnt * 100ui64) / ansmax)
			));
	}

	while(hasKey())
	{
		if(getKey() == 0x1b)
		{
			cout("ABORTED!\n");
			RequestAborted = 1;
			return 0;
		}
	}
	return 1;
}

static void CR_Init(void)
{
	PrmFile = makeTempFile("ncp-prm");
	PrmFp = fileOpen(PrmFile, "wb");
}
static void ClientRequest(void)
{
	uchar ip[4];

	errorCase(!PrmFp); // ? CR_Init() ���Y��

	fileClose(PrmFp);
	PrmFp = NULL;
	PadFile2(PrmFile, "NCP_Prm");

	memset(ip, 0, 4);
	SockStartup();

	for(; ; )
	{
		AnsFile = sockClient(ip, ServerDomain, ServerPort, PrmFile, Idle);

		if(AnsFile && UnpadFile2(AnsFile, "NCP_Ans"))
			break;

		if(RequestAborted || !RetryCount)
		{
			cout("CREATE-ANS-DUMMY\n");
			AnsFile = makeTempFile("ncp-ans-dummy");
			RequestAborted = 1;
			break;
		}
		cout("RETRY %u TIMES\n", RetryCount);
		RetryCount--;
	}
	SockCleanup();
	AnsFp = fileOpen(AnsFile, "rb");
}
static void CR_Fnlz(void)
{
	if(PrmFp) fileClose(PrmFp);
	if(AnsFp) fileClose(AnsFp);

	if(PrmFile) removeFile(PrmFile);
	if(AnsFile) removeFile(AnsFile);

	execute("TITLE ncp");

	if(RequestAborted)
	{
		cout("+---------------------------+\n");
		cout("| �G���[�܂��͒��f���܂����B|\n");
		cout("+---------------------------+\n");
	}
}

static uint64 IOCounter;

static void WriteToPrmFp(uchar *buffer, uint size)
{
	autoBlock_t gab;

	IOCounter += (uint64)size;

	if(2 <= size)
	{
		execute_x(xcout("TITLE ncp - %I64u bytes wrote", IOCounter));
	}
	writeBinaryBlock(PrmFp, gndBlockVar(buffer, size, gab));
}
static void ReadFromAnsFp(uchar *buffer, uint size)
{
	autoBlock_t *block = readBinaryBlock(AnsFp, size);

	if(size == getSize(block))
	{
		IOCounter += (uint64)size;

		if(2 <= size)
		{
			execute_x(xcout("TITLE ncp - %I64u bytes read", IOCounter));
		}
		memcpy(buffer, directGetBuffer(block), size);
	}
	else
	{
		STD_ReadStop = 1;
	}
}
static void ReadEndToStream(FILE *rfp, FILE *wfp)
{
	autoBlock_t *buffer;

	while(buffer = readBinaryStream(rfp, 128 * 1024 * 1024))
	{
		IOCounter += (uint64)getSize(buffer);
		execute_x(xcout("TITLE ncp - %I64u bytes copied", IOCounter));
		writeBinaryBlock_x(wfp, buffer);
	}
}

int main(int argc, char **argv)
{
	sockClientAnswerFileSizeMax = UINT64MAX;

	md5_interrupt = MD5Interrupt;

readArgs:
	if(argIs("/S"))
	{
		ServerDomain = nextArg();
		goto readArgs;
	}
	if(argIs("/P"))
	{
		ServerPort = toValue(nextArg());
		goto readArgs;
	}
	if(argIs("/F"))
	{
		ForceOverwriteMode = 1;
		goto readArgs;
	}

	CR_Init();

	if(argIs("/UP")) // Upload
	{
		char *localPath;
		char *serverPath;

		cout("UPLOAD\n");

		localPath = nextArg();
		serverPath = nextArg();

		writeLine(PrmFp, serverPath);
		writeLine(PrmFp, "Dummy");
		writeChar(PrmFp, 'U');
		writeChar(PrmFp, ForceOverwriteMode ? 'F' : '-');

		if(existDir(localPath)) // Directory
		{
			writeChar(PrmFp, 'D');
			DirToStream(localPath, WriteToPrmFp);
		}
		else if(existFile(localPath)) // File
		{
			FILE *fp;

			writeChar(PrmFp, 'F');

			fp = fileOpen(localPath, "rb");
			ReadEndToStream(fp, PrmFp);
			fileClose(fp);
		}
		else
		{
			cout("+--------+\n");
			cout("| �ˁ[�� |\n");
			cout("+--------+\n");

			goto cr_fnlz;
		}
		cout("SEND...\n");
		ClientRequest();
		cout("SEND-END\n");
	}
	else if(argIs("/DL")) // Download
	{
		char *localPath;
		char *serverPath;
		int type;

		cout("DOWNLOAD\n");

		localPath = nextArg();
		serverPath = nextArg();

		errorCase(existPath(localPath));

		writeLine(PrmFp, serverPath);
		writeLine(PrmFp, "Dummy");
		writeChar(PrmFp, 'D');
		writeChar(PrmFp, '-');

		cout("RECV...\n");
		ClientRequest();
		cout("RECV-END\n");

		if(RequestAborted)
			goto cr_fnlz;

		type = readChar(AnsFp);

		if(type == 'D') // Directory
		{
			createDir(localPath);

//			STD_TrustMode = 1;
			StreamToDir(localPath, ReadFromAnsFp);
//			STD_TrustMode = 0;
		}
		else if(type == 'F') // File
		{
			FILE *fp;

			fp = fileOpen(localPath, "wb");
			ReadEndToStream(AnsFp, fp);
			fileClose(fp);
		}
		else
		{
			cout("+----------+\n");
			cout("| �����[�� |\n");
			cout("+----------+\n");
		}
	}
	else if(argIs("/SZ")) // Size
	{
		char *serverPath = nextArg();
		int type;

		writeLine(PrmFp, serverPath);
		writeLine(PrmFp, "Dummy");
		writeChar(PrmFp, 'S');
		writeChar(PrmFp, '-');

		ClientRequest();

		if(RequestAborted)
			goto cr_fnlz;

		type = readChar(AnsFp);

		if(type == 'D')
		{
			cout("%s byte(s) directory exists.\n", c_thousandComma(xcout("%I64u", readValue64(AnsFp))));
		}
		else if(type == 'F')
		{
			cout("%s byte(s) file exists.\n", c_thousandComma(xcout("%I64u", readValue64(AnsFp))));
		}
		else if(type == 'N')
		{
			cout("not exists.\n");
		}
		else
		{
			cout("+--------------+\n");
			cout("| �킩��ˁ[�� |\n");
			cout("+--------------+\n");
		}
	}
	else if(argIs("/MV")) // Move
	{
		char *serverPath1;
		char *serverPath2;

		serverPath1 = nextArg();
		serverPath2 = nextArg();

		cout("MOVE\n");

		writeLine(PrmFp, serverPath1);
		writeLine(PrmFp, serverPath2);
		writeChar(PrmFp, 'M');
		writeChar(PrmFp, ForceOverwriteMode ? 'F' : '-');

		ClientRequest();
	}
	else if(argIs("/RM")) // Remove
	{
		char *serverPath = nextArg();

		cout("REMOVE\n");

		writeLine(PrmFp, serverPath);
		writeLine(PrmFp, "Dummy");
		writeChar(PrmFp, 'X');
		writeChar(PrmFp, '-');

		ClientRequest();
	}
	else if(argIs("/LS")) // List
	{
		char *path;

		writeLine(PrmFp, "Dummy");
		writeLine(PrmFp, "Dummy");
		writeChar(PrmFp, 'L');
		writeChar(PrmFp, '-');

		ClientRequest();

		while(path = readLine(AnsFp))
		{
			line2JLine(path, 1, 0, 0, 1);
			cout("%s\n", path);
			memFree(path);
		}
	}

cr_fnlz:
	CR_Fnlz();
}
