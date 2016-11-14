/*
	DLRun.exe [/C DIR] [/U URL]

		DIR ... ���Ƃ��Ă����o�b�`���s���̃J�����gDIR
*/

#include "C:\Factory\Labo\Socket\libs\http\httpClient.h"

#define FILE_LAST_DL_BAT "C:\\Factory\\tmp\\Remote_LastDLRun_bat.dat"
#define FILE_DL_BAT "Run.bat"

static char *DLUrl = "http://localhost/remote/DLRun.bat";
static char *RunDir;

static int CheckBatImage(autoBlock_t *batImage)
{
	autoBlock_t *lastBatImage;

	if(!batImage)
		return 0;

	if(!getSize(batImage))
		return 0;

	if(existFile(FILE_LAST_DL_BAT))
		lastBatImage = readBinary(FILE_LAST_DL_BAT);
	else
		lastBatImage = newBlock();

	if(isSameBlock(batImage, lastBatImage))
	{
		releaseAutoBlock(lastBatImage);
		return 0;
	}
	writeBinary(FILE_LAST_DL_BAT, batImage);
	return 1;
}
static void DLRun(void)
{
	char *dir = makeTempDir(NULL);
	char *batFile;
	autoBlock_t *batImage;

	LOGPOS_T();

	batFile = combine(dir, FILE_DL_BAT);
	batImage = httpGetOrPost(DLUrl, NULL);

	LOGPOS();

	if(batImage)
	{
		if(CheckBatImage(batImage))
		{
			writeBinary(batFile, batImage);

			addCwd(RunDir);
			{
				coExecute_x(xcout("CMD /C \"%s\"", batFile));
			}
			unaddCwd();
		}
		releaseAutoBlock(batImage);
	}
	recurRemoveDir(dir);
	memFree(dir);
	memFree(batFile);

	LOGPOS();
}
int main(int argc, char **argv)
{
readArgs:
	if(argIs("/U"))
	{
		DLUrl = nextArg();
		goto readArgs;
	}
	if(argIs("/C"))
	{
		RunDir = nextArg();
		goto readArgs;
	}

	if(!RunDir)
		RunDir = getCwd();

	errorCase(m_isEmpty(DLUrl));
	errorCase(!existDir(RunDir));

	DLRun();
}