#include "all.h"

// ---- mutex ----

static uint Mtx;

static void DoLock(void)
{
	errorCase(Mtx);
	Mtx = mutexLock("{64d73646-c565-4b8a-a877-db526cc65532}");
}
static void DoUnlock(void)
{
	errorCase(!Mtx);
	mutexUnlock(Mtx);
	Mtx = 0;
}

// ---- collabo ----

#define DIR2_DIR "C:\\app\\Kit\\Dir2"

static char *GetDir2File(void)
{
	static char *file;

	if(!file)
		file = GetCollaboFile(DIR2_DIR "\\Dir2.exe");

	return file;
}
static char *GetDir2ToolsFile(void)
{
	static char *file;

	if(!file)
		file = GetCollaboFile(DIR2_DIR "\\Dir2Tools.exe");

	return file;
}
static char *GetAddFilePartFile(void)
{
	static char *file;

	if(!file)
		file = GetCollaboFile(DIR2_DIR "\\AddFilePart.exe");

	return file;
}
static char *GetGetFilePartFile(void)
{
	static char *file;

	if(!file)
		file = GetCollaboFile(DIR2_DIR "\\GetFilePart.exe");

	return file;
}
static char *GetSetFileTimeFile(void)
{
	static char *file;

	if(!file)
		file = GetCollaboFile(DIR2_DIR "\\SetFileTime.exe");

	return file;
}

// ----

static void PrintCatalogLineError(char *line, char *reason)
{
	line = strx(line);
	line2JLine(line, 1, 0, 0, 1);

	cout("�s���ȍs���J�^���O���珜�����܂����B\n");
	cout("\t���s��[%s]\n", line);
	cout("\t�����R��%s\n", reason);

	memFree(line);
}
static int ToFairCatalogLine(char *line) // ret: ? ! �s���ȍs
{
	uint64 size;
	uint64 stamp;
	uint64 millis;

	errorCase(!line);

	if(!lineExp("<19,09>*<17,09>*<1,DDFF>*<1,300,>", line))
	{
		PrintCatalogLineError(line, "�s���ȍs�t�H�[�}�b�g");
		return 0;
	}
	if(!isFairLocalPath(line + 40, PUB_DIR_LENMAX))
	{
		PrintCatalogLineError(line, "�s���ȃ��[�J���t�@�C����");
		return 0;
	}
	size = toValue64_x(strxl(line, 19));

	if(IMAX_64 < size)
	{
		PrintCatalogLineError(line, "���傷����t�@�C��");
		return 0;
	}
	stamp = toValue64_x(strxl(line + 20, 17));

	if(!IsFairFileStamp(stamp))
	{
		PrintCatalogLineError(line, "�s���ȃt�@�C������");
		return 0;
	}
	millis = FileStampToMillis(stamp);
	millis += WTIME_UNIT - 1;
	millis /= WTIME_UNIT;
	millis *= WTIME_UNIT;
	stamp = MillisToFileStamp(millis);
	m_range(stamp, WTIME_MIN, WTIME_MAX);

	_snprintf(line + 20, 17, "%017I64u", stamp);

	return 1;
}
void DC_ToFairCatalog(autoList_t *catalog)
{
	char *line;
	uint index;

	errorCase(!catalog);

	foreach(catalog, line, index)
		if(!ToFairCatalogLine(line))
			line[0] = '\0';

	trimLines(catalog);
	sortJLinesICase(catalog);
}
autoList_t *DC_GetCatalog(char *dir) // ret: NULL == ���s
{
	char *outFile = makeTempPath(NULL);
	char *successfulFile = makeTempPath(NULL);
	autoList_t *catalog;

	errorCase(!dir);

	DoLock();
	{
		coExecute_x(xcout("START \"\" /B /WAIT \"%s\" \"%s\" \"%s\" \"%s\"", GetDir2File(), dir, outFile, successfulFile));
	}
	DoUnlock();

	if(existFile(successfulFile))
	{
		catalog = readLines(outFile);
		DC_ToFairCatalog(catalog);
	}
	else
		catalog = NULL;

	removeFileIfExist(outFile);
	removeFileIfExist(successfulFile);
	memFree(outFile);
	memFree(successfulFile);
	return catalog;
}
static int DoDir2Tools(char *command, char *path) // ret: ? ����
{
	char *successfulFile = makeTempPath(NULL);
	int ret;

	errorCase(!path);

	DoLock();
	{
		coExecute_x(xcout("START \"\" /B /WAIT \"%s\" %s \"%s\" \"%s\"", GetDir2ToolsFile(), command, path, successfulFile));
	}
	DoUnlock();

	ret = existFile(successfulFile);

	removeFileIfExist(successfulFile);
	memFree(successfulFile);
	return ret;
}
int DC_CreateDir(char *dir)
{
	return DoDir2Tools("/MD", dir);
}
int DC_RemoveDir(char *dir)
{
	return DoDir2Tools("/RD", dir);
}
int DC_RemoveFile(char *file)
{
	return DoDir2Tools("/DEL", file);
}
static int DoDir2Tools2(char *command, char *rFile, char *wFile) // ret: ? ����
{
	char *successfulFile = makeTempPath(NULL);
	int ret;

	errorCase(!rFile);
	errorCase(!wFile);

	DoLock();
	{
		coExecute_x(xcout("START \"\" /B /WAIT \"%s\" %s \"%s\" \"%s\" \"%s\"", GetDir2ToolsFile(), command, rFile, wFile, successfulFile));
	}
	DoUnlock();

	ret = existFile(successfulFile);

	removeFileIfExist(successfulFile);
	memFree(successfulFile);
	return ret;
}
int DC_MoveFile(char *rFile, char *wFile)
{
	return DoDir2Tools2("/MOVE", rFile, wFile);
}
int DC_AddFilePart(char *wFile, uint64 startPos, autoBlock_t *rData) // ret: ? ����
{
	char *rFile = makeTempPath(NULL);
	char *successfulFile = makeTempPath(NULL);
	int ret;

	errorCase(!wFile);
	errorCase(IMAX_64 < startPos);
	errorCase(!rData);

	writeBinary(rFile, rData);

	DoLock();
	{
		coExecute_x(xcout("START \"\" /B /WAIT \"%s\" \"%s\" \"%s\" %I64u \"%s\"", GetAddFilePartFile(), rFile, wFile, startPos, successfulFile));
	}
	DoUnlock();

	ret = existFile(successfulFile);

	removeFileIfExist(rFile);
	removeFileIfExist(successfulFile);
	memFree(rFile);
	memFree(successfulFile);
	return ret;
}
autoBlock_t *DC_GetFilePart(char *rFile, uint64 startPos, uint readSize) // ret: NULL == (���s || �w��̈悪�t�@�C���f�[�^�͈̔͊O)
{
	char *wFile = makeTempPath(NULL);
	char *successfulFile = makeTempPath(NULL);
	autoBlock_t *ret;

	errorCase(!rFile);
	errorCase(IMAX_64 < startPos);
	errorCase(IMAX < readSize);

	DoLock();
	{
		coExecute_x(xcout("START \"\" /B /WAIT \"%s\" \"%s\" \"%s\" %I64u %u \"%s\"", GetGetFilePartFile(), rFile, wFile, startPos, readSize, successfulFile));
	}
	DoUnlock();

	if(existFile(successfulFile))
		ret = readBinary(wFile);
	else
		ret = NULL;

	removeFileIfExist(wFile);
	removeFileIfExist(successfulFile);
	memFree(wFile);
	memFree(successfulFile);
	return ret;
}
int DC_SetFileTime(char *wFile, uint64 stamp) // ret: ? ����
{
	char *successfulFile = makeTempPath(NULL);
	int ret;

	errorCase(!wFile);

	DoLock();
	{
		coExecute_x(xcout("START \"\" /B /WAIT \"%s\" \"%s\" %I64u \"%s\"", GetSetFileTimeFile(), wFile, stamp, successfulFile));
	}
	DoUnlock();

	ret = existFile(successfulFile);

	removeFileIfExist(successfulFile);
	memFree(successfulFile);
	return ret;
}