/*
	.7z �̈��k�̂�

	- - -

	z7.exe [/C] [/T] [/7] [/OAD] [���̓t�@�C�� | ����DIR]

		/C   ... ���̓t�@�C���Ɠ����ꏊ�Ɉ��k����B
		/T   ... �s�v�ȏ�ʊK�w����������B(DIR�̂Ƃ��̂�)
		/7   ... .7z �ɂ���B
		/OAD ... ���t�@�C���E�f�B���N�g�������폜
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Collabo.h"
#include "C:\Factory\Meteor\7z.h"

static int OutputSameDir;
static int TrimTopDir;
static int WFileType = 'Z'; // "7Z"
static int OutputAndDelete;

static char *Get7zExeFile(void) // ret: �󔒂��܂܂Ȃ��p�X�ł��邱�ƁB
{
	static char *file;

	if(!file)
		file = GetCollaboFile(FILE_7Z_EXE);

	return file;
}

static char *LastOutputDir;

static char *GetWFile(char *path)
{
	char *file7z;
	char *ext;

	cout("< %s\n", path);

	switch(WFileType)
	{
	case '7': ext = "7z";  break;
	case 'Z': ext = "zip"; break;

	default:
		error();
	}

	if(OutputSameDir)
	{
		file7z = addExt(strx(path), ext);
		file7z = toCreatableTildaPath(file7z, IMAX);
	}
	else
	{
		char *dir = makeFreeDir();

		file7z = combine(dir, getLocal(path));
		file7z = addExt(file7z, ext);

		memFree(LastOutputDir);
		LastOutputDir = dir;
	}

	cout("> %s\n", file7z);

	return file7z;
}
static void Pack7z_File(char *file)
{
	char *file7z = GetWFile(file);

	coExecute_x(xcout("%s a \"%s\" \"%s\"", Get7zExeFile(), file7z, file));

	memFree(file7z);

	LOGPOS();
}
static void Pack7z_Dir(char *dir)
{
	char *file7z;

	errorCase(isAbsRootDir(dir)); // ���[�gDIR �s��

	file7z = GetWFile(dir);

	if(TrimTopDir)
	{
		autoList_t *subPaths = ls(dir);
		char *subPath;
		uint index;

		foreach(subPaths, subPath, index)
		{
			coExecute_x(xcout("%s a \"%s\" \"%s\"", Get7zExeFile(), file7z, subPath));
		}
		releaseDim(subPaths, 1);
	}
	else
	{
		coExecute_x(xcout("%s a \"%s\" \"%s\"", Get7zExeFile(), file7z, dir));
	}
	memFree(file7z);

	LOGPOS();
}
static void Pack7z(char *path)
{
	path = makeFullPath(path);

	if(existFile(path))
	{
		Pack7z_File(path);
	}
	else if(existDir(path))
	{
		Pack7z_Dir(path);
	}
	else
	{
		error();
	}
	if(!OutputSameDir)
	{
		LOGPOS();
		coExecute_x(xcout("START %s", LastOutputDir));
	}
	if(OutputAndDelete)
	{
		LOGPOS();
		recurRemovePath(path);
	}
	memFree(path);

	LOGPOS();
}
int main(int argc, char **argv)
{
	cout("=======\n");
	cout("z7 ���k\n");
	cout("=======\n");

readArgs:
	if(argIs("/C"))
	{
		OutputSameDir = 1;
		goto readArgs;
	}
	if(argIs("/T"))
	{
		TrimTopDir = 1;
		goto readArgs;
	}
	if(argIs("/7"))
	{
		WFileType = '7';
		goto readArgs;
	}
	if(argIs("/OAD"))
	{
		OutputAndDelete = 1;
		goto readArgs;
	}

	if(hasArgs(1))
	{
		Pack7z(nextArg());
		return;
	}

	for(; ; )
	{
		Pack7z(c_dropDirFile());

		cout("\n");
		cout("-------\n");
		cout("z7 ���k\n");
		cout("-------\n");
	}
}
