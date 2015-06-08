/*
	�f�B���N�g�����t�@�C���ɂ܂Ƃ߂܂��B
	���t�E�����E�����͕ۑ����܂���B

	----

	Cluster.exe [/C] [/Q] [/T] [/OAC] [/OAD] [/K �N���X�^�t�@�C�� |
	             /M  �o�̓N���X�^�t�@�C�� ����DIR |
	             /MO �o�̓N���X�^�t�@�C�� ����DIR |
	             /BM �o�̓N���X�^�t�@�C�� ����DIR |
	             /R  ���̓N���X�^�t�@�C�� �o��DIR |
	             /KR ���̓N���X�^�t�@�C�� �o��DIR |
	             /MR ����DIR �o��DIR]

		/C   ... ��ɃJ�����g�f�B���N�g���ɏo�͂���B
		/Q   ... ���X�g�A���ɔj�����`�F�b�N���Ȃ��B
		/T   ... �g���X�g���[�h
		/OAC ... �o�͌�A���̓t�@�C�����폜����B���̓f�B���N�g���͋�ɂ���B
		/OAD ... �o�͌�A���̓t�@�C�����폜����B���̓f�B���N�g�����폜����B
		/K   ... �`�F�b�N�̂�

		/M
			�N���X�^�t�@�C������
			�o�̓N���X�^�t�@�C�� ... ���݂��Ȃ��쐬�\�ȃp�X�ł��邱�ƁB
			����DIR

		/MO
			�N���X�^�t�@�C������ (�㏑��)
			�o�̓N���X�^�t�@�C�� ... �폜�\�ȃt�@�C�� or ���݂��Ȃ��쐬�\�ȃp�X�ł��邱�ƁB
			����DIR

		/BM
			�N���X�^�t�@�C������ (�P����ێ�)
			�o�̓N���X�^�t�@�C�� ... �폜�\�ȃt�@�C�� or ���݂��Ȃ��쐬�\�ȃp�X�ł��邱�ƁB
			����DIR

		/R
			�f�B���N�g������
			���̓N���X�^�t�@�C��
			�o��DIR ... ���݂��Ȃ��쐬�\�ȃp�X�ł��邱�ƁB

		/RQ
			�f�B���N�g������ (�`�F�b�N����)
			���̓N���X�^�t�@�C��
			�o��DIR ... ���݂��Ȃ��쐬�\�ȃp�X�ł��邱�ƁB

		/MR
			�f�B���N�g���̃R�s�[
			����DIR
			�o��DIR ... ���݂��Ȃ��쐬�\�ȃp�X�ł��邱�ƁB
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\SumStream.h"

#define EXT_CLUSTER "clu"

static FILE *Stream;
static uint64 RWCount;
static int OutputAndCleanMode;
static int OutputAndDeleteMode;

static void Writer(uchar *block, uint size)
{
	autoBlock_t writeBlock = gndBlock(block, size);

	writeBinaryBlock(Stream, &writeBlock);
	RWCount += size;

//	if(2 <= size) // old
	if(2 <= size && pulseSec(1, NULL))
		cout("\r%I64u bytes wrote OK", RWCount);
}
static void Reader(uchar *block, uint size)
{
	autoBlock_t *readBlock = readBinaryBlock(Stream, size);
	uint readSize;
	uint erred = 0;

	readSize = getSize(readBlock);

	if(readSize != size)
	{
		erred = 1;
		memset(block, 0x00, size);
	}
	else
		memcpy(block, directGetBuffer(readBlock), readSize);

	releaseAutoBlock(readBlock);
	RWCount += size;

//	if(2 <= size || erred) // old
	if(2 <= size && pulseSec(1, NULL) || erred)
		cout("\r%I64u bytes read %s", RWCount, erred ? "ERROR\n" : "OK");
}

static void BackupFile(char *file)
{
	char *oldFile = addChar(strx(file), '_');

	cout("Backup File\n");
	cout("< %s\n", file);
	cout("> %s\n", oldFile);

	coExecute_x(xcout("DEL \"%s\"", oldFile));
	coExecute_x(xcout("REN \"%s\" \"%s\"", file, getLocal(oldFile)));

	memFree(oldFile);
}
static void MakeCluster(char *file, char *dir)
{
	cout("Make Cluster\n");
	cout("> %s\n", file);
	cout("< %s\n", dir);

	errorCase(existPath(file));
	errorCase(!existDir(dir));

	Stream = fileOpen(file, "wb");
	RWCount = 0;
	DirToSumStream(dir, Writer);
	fileClose(Stream);

	cout("\r%I64u bytes wrote FINISHED\n", RWCount);

	if(OutputAndCleanMode)
	{
		cout("CLEAN DIR\n");
		forceClearDir(dir); // fixme: ���t�@�C�����ɑΉ�����H
		cout("CLEAN DIR Ok.\n");
	}
	else if(OutputAndDeleteMode)
	{
		cout("DELETE DIR\n");
		forceRemoveDir(dir); // fixme: ���t�@�C�����ɑΉ�����H
		cout("DELETE DIR Ok.\n");
	}
}
static void RestoreCluster(char *file, char *dir)
{
	int retval;

	cout("Restore dir from Cluster\n");
	cout("< %s\n", file);
	cout("> %s\n", dir);

	errorCase(!existFile(file));
	errorCase(existPath(dir));

	createDir(dir);

	Stream = fileOpen(file, "rb");
	RWCount = 0;
	retval = SumStreamToDir(dir, Reader);
	fileClose(Stream);

	cout("\r%I64u bytes read %s\n", RWCount, retval ? "SUCCESSFUL" : "FAULT");
	errorCase(!retval);

	if(OutputAndCleanMode || OutputAndDeleteMode)
	{
		cout("DELETE CLU\n");
		removeFile(file);
		cout("DELETE CLU Ok.\n");
	}
}
static void CheckCluster(char *file)
{
	int retval;
	uint64 fsize;

	cout("Check summary of Cluster\n");
	cout("< %s\n", file);

	errorCase(!existFile(file));

	fsize = getFileSize(file);

	Stream = fileOpen(file, "rb");
	RWCount = 0;
	retval = CheckSumStream(Reader, fsize);
	fileClose(Stream);

	cout("\r%I64u bytes read %s\n", RWCount, retval ? "CHECK SUM OK" : "CHECK SUM ERROR");
	errorCase(!retval);
}

static int RestoreSameDirMode;
static int NoCheckClusterMode;

static void AutoActCluster(char *path)
{
	if(existDir(path))
	{
		char *file = addExt(strx(path), EXT_CLUSTER);

		MakeCluster(file, path);
		memFree(file);
	}
	else
	{
		char *fdir = NULL;
		char *dir;

		errorCase(_stricmp(EXT_CLUSTER, getExt(path)));

		if(!RestoreSameDirMode)
		{
			fdir = makeFreeDir();
			dir = combine_cx(fdir, changeExt(getLocal(path), ""));
		}
		else
			dir = changeExt(path, "");

		if(!NoCheckClusterMode)
			CheckCluster(path);

		RestoreCluster(path, dir);

		if(fdir)
		{
			execute_x(xcout("START %s\n", fdir));
			memFree(fdir);
		}
		memFree(dir);
	}
}

int main(int argc, char **argv)
{
readArgs:
	if(argIs("/C")) // out to same dir (Current dir?) mode
	{
		RestoreSameDirMode = 1;
		goto readArgs;
	}
	if(argIs("/Q")) // Quick mode
	{
		NoCheckClusterMode = 1;
		goto readArgs;
	}
	if(argIs("/T")) // Trust mode
	{
		cout("***********\n");
		cout("** TRUST **\n");
		cout("***********\n");

		STD_TrustMode = 1;
		goto readArgs;
	}
	if(argIs("/OAC"))
	{
		cout("**********************\n");
		cout("** OUTPUT AND CLEAN **\n");
		cout("**********************\n");

		OutputAndCleanMode = 1;
		goto readArgs;
	}
	if(argIs("/OAD"))
	{
		cout("***********************\n");
		cout("** OUTPUT AND DELETE **\n");
		cout("***********************\n");

		OutputAndDeleteMode = 1;
		goto readArgs;
	}

	if(argIs("/M")) // Make cluster
	{
		MakeCluster(getArg(0), getArg(1)); // (0) �o�̓t�@�C�� <- (1) ����DIR, (0) ... ���݂��Ȃ��쐬�\�ȃp�X
		return;
	}
	if(argIs("/MO")) // Make cluster Overwrite
	{
		removeFileIfExist(getArg(0));
		MakeCluster(getArg(0), getArg(1)); // (0) �o�̓t�@�C�� <- (1) ����DIR, (0) ... �폜�\�ȃt�@�C�� or ���݂��Ȃ��쐬�\�ȃp�X
		return;
	}
	if(argIs("/BM")) // Backup and Make cluster
	{
		BackupFile(getArg(0));
		MakeCluster(getArg(0), getArg(1)); // (0) �o�̓t�@�C�� <- (1) ����DIR, (0) ... ���݂��Ȃ��쐬�\�ȃp�X
		return;
	}
	if(argIs("/K")) // Check cluster
	{
		CheckCluster(getArg(0));
		return;
	}
	if(argIs("/RQ")) // Restore dir from cluster
	{
		RestoreCluster(getArg(0), getArg(1)); // (0) ���̓t�@�C�� -> (1) �o��DIR, (1) ... ���݂��Ȃ��쐬�\�ȃp�X
		return;
	}
	if(argIs("/R")) // Check and Restore dir from cluster
	{
		CheckCluster(getArg(0));
		RestoreCluster(getArg(0), getArg(1)); // (0) ���̓t�@�C�� -> (1) �o��DIR, (1) ... ���݂��Ȃ��쐬�\�ȃp�X
		return;
	}
	if(argIs("/MR")) // Make and Restore (Copy)
	{
		char *inpDir = getArg(0); // (0) ����DIR -> (1) �o��DIR, (1) ... ���݂��Ȃ��쐬�\�ȃp�X
		char *outDir = getArg(1);
		char *midFile = makeTempPath(NULL);

		cout("< %s\n", inpDir);
		cout("> %s\n", outDir);
		cout("+ %s\n", midFile);

		MakeCluster(midFile, inpDir);
		RestoreCluster(midFile, outDir);
		removeFile(midFile);
		memFree(midFile);
		return;
	}

	/*
	�N���X�^���h���b�v
		/C ���� -> �o�͐�͑��݂��Ȃ��쐬�\�ȃp�X
		/C �Ȃ� -> C:\1, 2, 3... �ɏo��
	�f�B���N�g�����h���b�v
		�o�͐�͑��݂��Ȃ��쐬�\�ȃp�X
	*/

	if(hasArgs(1))
	{
		AutoActCluster(getArg(0));
		return;
	}

	for(; ; )
	{
		char *path = dropPath();

		if(!path)
			break;

		AutoActCluster(path);
		memFree(path);
		cout("\n");
	}
}
