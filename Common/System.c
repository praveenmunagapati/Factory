#include "all.h"

// ---- flags ----

int sockServerMode;

// ----

int lastSystemRet; // ? ! �R�}���h������Ɏ��s�o���� 0 ��Ԃ����B

void execute(char *commandLine)
{
	lastSystemRet = system(commandLine);
}
void execute_x(char *commandLine)
{
	execute(commandLine);
	memFree(commandLine);
}
void coExecute(char *commandLine)
{
	cout("cmdln: %s\n", commandLine);
	execute(commandLine);
}
void coExecute_x(char *commandLine)
{
	coExecute(commandLine);
	memFree(commandLine);
}
void sleep(uint millis) // ts_
{
	/*
		���X���b�h�ɐ����n�����߂ɁAinner_uncritical -> sleep(0) -> inner_critical ���Ă���Ƃ��낪���邪�A
		sleep(0) ���Ɛ��䂪�n��Ȃ��Bsleep(1) �Ȃ�n��B-> �����I�� sleep(1) �ɂ���B
	*/
	if(!millis)
		millis = 1;

	Sleep(millis);
}
void coSleep(uint millis)
{
	uint elapse = 0;

	cout("%u�~���b�҂��܂�...\n", millis);

	while(elapse < millis)
	{
		uint m = m_min(millis - elapse, 300);

		sleep(m);
		elapse += m;
		cout("\r%u�~���b�o���܂����B", elapse);
	}
	cout("\n");
}
void noop(void)
{
	// noop
}
void noop_u(uint dummy)
{
	// noop
}
void noop_uu(uint dummy1, uint dummy2)
{
	// noop
}
uint getZero(void)
{
	return 0;
}
char *getEnvLine(char *name) // ret: c_
{
	char *line;

	errorCase(!name);
	line = getenv(name);

	if(!line)
		line = "";

	return line;
}
#if 0
static DWORD GetTickCount_TEST(void)
{
	static int initOnce;
	static uint baseTick;

	if(!initOnce)
	{
		initOnce = 1;
		baseTick = UINT_MAX - 10000 - GetTickCount();
	}
	return baseTick + GetTickCount();
}
#endif
uint64 nowTick(void)
{
//	uint currTick = GetTickCount_TEST();
	uint currTick = GetTickCount();
	static uint lastTick;
	static uint64 baseTick;
	uint64 retTick;
	static uint64 lastRetTick;

	if(currTick < lastTick) // ? �J�E���^���߂��� -> �I�[�o�[�t���[�����H
	{
		uint diffTick = lastTick - currTick;

		if(UINTMAX / 2 < diffTick) // �I�[�o�[�t���[���낤�B
		{
			LOGPOS();
			baseTick += (uint64)UINT_MAX + 1;
		}
		else // �I�[�o�[�t���[���H
		{
			LOGPOS();
			baseTick += diffTick; // �O��Ɠ����߂�l�ɂȂ�悤�ɒ�������B
		}
	}
	lastTick = currTick;
	retTick = baseTick + currTick;
	errorCase(retTick < lastRetTick); // 2bs
	lastRetTick = retTick;
	return retTick;
}
uint now(void)
{
	return (uint)(nowTick() / 1000);
}
int pulseSec(uint span, uint *p_nextSec)
{
	static uint defNextSec;
	uint sec = now();

	if(!p_nextSec)
		p_nextSec = &defNextSec;

	if(sec < *p_nextSec)
		return 0;

	*p_nextSec = sec + span;
	return 1;
}
int eqIntPulseSec(uint span, uint *p_nextSec)
{
	static uint callPerCheck = 1;
	static uint count;

	count++;

	if(count % callPerCheck == 0 && pulseSec(span, p_nextSec))
	{
		count = m_max(callPerCheck / 3, count / 10);
		callPerCheck = m_max(1, count);
		count = 0;
		return 1;
	}
	return 0;
}
uint getTick(void)
{
	return GetTickCount();
}
uint getUDiff(uint tick1, uint tick2) // ret: tick2 - tick1
{
	if(tick2 < tick1)
	{
		return 0xffffffffu - ((tick1 - tick2) - 1u);
	}
	return tick2 - tick1;
}
sint getIDiff(uint tick1, uint tick2) // ret: tick2 - tick1
{
	uint diff = getUDiff(tick1, tick2);

	if(diff < 0x80000000u)
	{
		return (sint)diff;
	}
	return (-0x7fffffff - 1) + (sint)(diff - 0x80000000u);
}
int isLittleEndian(void)
{
	uint dword = 1;
	return *(uchar *)&dword;
}

#define FILE_SHARE_COUNTER "C:\\Factory\\tmp\\Counter.txt"

uint64 nextCommonCount(void)
{
	char *line;
	uint64 counter;

	mutex();

	if(existFile(FILE_SHARE_COUNTER))
	{
		line = readFirstLine(FILE_SHARE_COUNTER);
		counter = toValue64(line);
		memFree(line);
	}
	else // �J�E���^������
	{
		counter = toValue64Digits_xc(makeCompactStamp(NULL), hexadecimal) << 8;
		// ex. 1980/01/02 02:03:55 -> 0x1980010202035500
	}
	errorCase(counter == UINT64MAX); // �J���X�g..�L�蓾�Ȃ����낤����..

	if(UINT64MAX / 2 < counter)
		LOGPOS(); // �J���X�g����..�L�蓾�Ȃ����낤����..

	line = xcout("%I64u", counter + 1);
	writeOneLine(FILE_SHARE_COUNTER, line);

	unmutex();

	memFree(line);
	return counter;
}
static char *c_GetTempSuffix(void)
{
	static char *ret;
	static int useFactoryTmpDir;

	if(ret)
		memFree(ret);
	else
		useFactoryTmpDir = isFactoryDirEnabled() && existDir("C:\\Factory\\tmp");

	if(!useFactoryTmpDir)
	{
		static uint pid;
		static uint64 pFATime;
		static uint64 counter;

		errorCase(counter == UINT64MAX); // �J���X�g..�L�蓾�Ȃ����낤����..

		if(UINT64MAX / 2 < counter)
			LOGPOS(); // �J���X�g����..�L�蓾�Ȃ����낤����..

		if(!pid)
		{
			pid = (uint)GetCurrentProcessId();
			errorCase(!pid); // 0 == System Idle Process
			pFATime = (uint64)time(NULL);
		}
		ret = xcout("%x_%I64x_%I64x", pid, pFATime, counter);
		counter++;
	}
	else
		ret = xcout("%I64x", nextCommonCount());

	return ret;
}
char *makeTempPath(char *ext) // ext: NULL ok
{
	static char *pbase;
	char *path;

	if(!pbase)
	{
		int useFactoryTmpDir = isFactoryDirEnabled() && existDir("C:\\Factory\\tmp");

		if(!useFactoryTmpDir)
		{
			char *tmpDir = combine(getSelfDir(), "tmp");

			if(existDir(tmpDir))
				pbase = combine(getSelfDir(), "tmp\\");
			else
				pbase = combine(getSelfDir(), "$tmp$");

			memFree(tmpDir);
		}
		else
			pbase = "C:\\Factory\\tmp\\";
	}
	for(; ; )
	{
		path = xcout("%s%s", pbase, c_GetTempSuffix());

		if(ext)
			path = addExt(path, ext);

		if(!accessible(path))
			break;

		memFree(path);
	}
	return path;
}
char *makeTempFile(char *ext) // ext: NULL ok
{
	char *file = makeTempPath(ext);
	createFile(file);
	return file;
}
char *makeTempDir(char *ext) // ext: NULL ok
{
	char *dir = makeTempPath(ext);
	createDir(dir);
	return dir;
}
char *makeFreeDir(void)
{
	char *path;

	mutex();
	path = toCreatablePath(strx("C:\\1"), 999 - 1);
	createDir(path);
	unmutex();

	return path;
}

#define SELFBUFFSIZE 1024
#define SELFBUFFMARGIN 16

char *getSelfFile(void)
{
	static char *fileBuff;

	if(!fileBuff)
	{
		fileBuff = memAlloc(SELFBUFFSIZE + SELFBUFFMARGIN);

		if(!GetModuleFileName(NULL, fileBuff, SELFBUFFSIZE))
			error();

		/*
			? �t���p�X�̎��s�\�t�@�C���ł͂Ȃ��B
		*/
		errorCase(strlen(fileBuff) < 8); // �ŒZ�ł� "C:\\a.exe"
		errorCase(!m_isalpha(fileBuff[0]));
		errorCase(memcmp(fileBuff + 1, ":\\", 2));
		errorCase(_stricmp(strchr(fileBuff, '\0') - 4, ".exe"));

		fileBuff = strr(fileBuff);
	}
	return fileBuff;
}
char *getSelfDir(void)
{
	static char *dirBuff;

	if(!dirBuff)
		dirBuff = getParent(getSelfFile());

	return dirBuff;
}

static char *GetOutDir(void)
{
	static char *dir;

	if(!dir)
		dir = makeTempDir("out");

	return dir;
}
char *getOutFile(char *localFile)
{
	return combine(GetOutDir(), localFile);
}
char *c_getOutFile(char *localFile)
{
	static char *outFile;
	memFree(outFile);
	return outFile = getOutFile(localFile);
}
void openOutDir(void)
{
	execute_x(xcout("START %s", GetOutDir()));
}

// ---- args ----

autoList_t *tokenizeArgs(char *str)
{
	autoList_t *args = newList();
	autoBlock_t *buff = newBlock();
	char *p;
	int literalMode = 0;

	for(p = str; *p; p++)
	{
		if(literalMode)
		{
			if(*p == '"' && (p[1] == ' ' || !p[1]))
			{
				literalMode = 0;
				goto addEnd;
			}
		}
		else
		{
			if(*p == ' ')
			{
				addElement(args, (uint)unbindBlock2Line(buff));
				buff = newBlock();
				goto addEnd;
			}
			if(*p == '"' && !getSize(buff))
			{
				literalMode = 1;
				goto addEnd;
			}
		}

		if(isMbc(p))
		{
			addByte(buff, *p);
			p++;
		}
		else if(*p == '\\' && (p[1] == '\\' || p[1] == '"'))
		{
			p++;
		}
		addByte(buff, *p);
	addEnd:;
	}
	addElement(args, (uint)unbindBlock2Line(buff));
	return args;
}

static autoList_t *Args;

static void ReadSysArgs(void)
{
	uint argi;

	for(argi = 0; argi < getCount(Args); )
	{
		char *arg = getLine(Args, argi);

		if(!_stricmp(arg, "//$")) // �ǂݍ��ݒ��~
		{
			desertElement(Args, argi);
			break;
		}
		else if(!_stricmp(arg, "//F")) // �p�����[�^���t�@�C������ǂݍ���Œu��������B
		{
			char *text;
			autoList_t *subArgs;

			desertElement(Args, argi);
			arg = (char *)desertElement(Args, argi);

			text = readResourceText(innerResPathFltr(arg));
			subArgs = tokenizeArgs(text);
			memFree(text);

			while(getCount(subArgs))
				insertElement(Args, argi, (uint)unaddElement(subArgs));

			releaseAutoList(subArgs);
		}
		else if(!_stricmp(arg, "//R")) // �p�����[�^���t�@�C������ǂݍ���Œu��������B���X�|���X�t�@�C�� (���s�������̋�؂�ƌ��Ȃ�)
		{
			autoList_t *subArgs;

			desertElement(Args, argi);
			arg = (char *)desertElement(Args, argi);

			subArgs = readLines(arg);

			while(getCount(subArgs))
				insertElement(Args, argi, (uint)unaddElement(subArgs));

			releaseAutoList(subArgs);
		}
		else if(!_stricmp(arg, "//O")) // �W���o��(cout�̏o��)���t�@�C���ɏ����o���B�����ӁFtermination();���Ȃ��ƃX�g���[���J�����ρI
		{
			desertElement(Args, argi);
			arg = (char *)desertElement(Args, argi);

			setCoutWrFile(arg, 0);
		}
		else if(!_stricmp(arg, "//A")) // �W���o��(cout�̏o��)���t�@�C���ɒǋL����B�����ӁFtermination();���Ȃ��ƃX�g���[���J�����ρI
		{
			desertElement(Args, argi);
			arg = (char *)desertElement(Args, argi);

			setCoutWrFile(arg, 1);
		}
		else
			argi++;
	}
}
static autoList_t *GetArgs(void)
{
	if(!Args)
	{
		uint argi;

		Args = newList();

		for(argi = 1; argi < __argc; argi++)
		{
			addElement(Args, (uint)__argv[argi]);
		}
		ReadSysArgs();
	}
	return Args;
}

static uint ArgIndex;

int hasArgs(uint count)
{
	return count <= getCount(GetArgs()) - ArgIndex;
}
int argIs(char *spell)
{
	if(ArgIndex < getCount(GetArgs()))
	{
		if(!_stricmp(getLine(GetArgs(), ArgIndex), spell))
		{
			ArgIndex++;
			return 1;
		}
	}
	return 0;
}
char *getArg(uint index)
{
	errorCase(getCount(GetArgs()) - ArgIndex <= index);
	return getLine(GetArgs(), ArgIndex + index);
}
char *nextArg(void)
{
	char *arg = getArg(0);

	ArgIndex++;
	return arg;
}
void skipArg(uint count)
{
	for(; count; count--) nextArg();
}
/*
	ret: �c��̃R�}���h������ index �Ԗڈȍ~�S�Ă�Ԃ��B
		index ���c����Ɠ����ꍇ { } ��Ԃ��B
*/
autoList_t *getFollowArgs(uint index)
{
	errorCase(getCount(GetArgs()) - ArgIndex < index);
	return recreateAutoList((uint *)directGetList(GetArgs()) + ArgIndex + index, getCount(GetArgs()) - ArgIndex - index);
}
autoList_t *allArgs(void)
{
	autoList_t *args = getFollowArgs(0);

	ArgIndex = getCount(GetArgs());
	return args;
}
uint getFollowArgCount(uint index)
{
	errorCase(getCount(GetArgs()) - ArgIndex < index);
	return getCount(GetArgs()) - (ArgIndex + index);
}
uint getArgIndex(void)
{
	return ArgIndex;
}
void setArgIndex(uint index)
{
	errorCase(getCount(GetArgs()) < index); // ? ! �S���ǂݏI��������
	ArgIndex = index;
}

// ---- innerResPathFltr ----

static char *FPP_Path;

static int FindPathParent(char *dir, char *localPath) // dir: abs_dir
{
	for(; ; )
	{
		FPP_Path = combine(dir, localPath);
//cout("FPP.1:%s\n", dir); // test
//cout("FPP.2:%s\n", FPP_Path); // test

		if(existPath(FPP_Path))
			return 1;

		memFree(FPP_Path);

		if(isAbsRootDir(dir))
			return 0;

		dir = getParent(dir);
	}
}
char *innerResPathFltr(char *path)
{
	if(isFactoryDirDisabled() && startsWithICase(path, "C:\\Factory\\")) // ? Factory ���� && Factory �z�����Q��
		goto go_search;

	if(getLocal(path) != path) // ? �p�X���w�肵�Ă���B
		if(existPath(path))
			goto foundPath;

go_search:
	if(FindPathParent(getSelfDir(), getLocal(path)))
	{
		path = FPP_Path;
		goto foundPath;
	}
	if(FindPathParent(getCwd(), getLocal(path)))
	{
		path = FPP_Path;
		goto foundPath;
	}
	cout("res_nf: %s\n", path);
writeOneLine(getOutFile("innerResPathFltr_path.txt"), path); // XXX
	error(); // not found

foundPath:
	cout("res: %s\n", path);
	return path; // path, strx() ���݂��Ă��邪 const char[] �Ƃ��Ĉ������ƁB
}

// ----

char *LOGPOS_Time(void)
{
	static char buff[23]; // UINT64MAX -> "307445734561825:51.615"
	uint64 millis = nowTick();

	sprintf(buff, "%I64u:%02u.%03u", millis / 60000, (uint)((millis / 1000) % 60), (uint)(millis % 1000));

	return buff;
}
