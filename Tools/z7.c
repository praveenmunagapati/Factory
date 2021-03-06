/*
	.7z の圧縮のみ

	- - -

	z7.exe [/C] [/T] [/7] [/OAD] [入力ファイル | 入力DIR]

		/C   ... 入力ファイルと同じ場所に圧縮する。
		/T   ... 不要な上位階層を除去する。(DIRのときのみ)
		/7   ... .7z にする。
		/OAD ... 元ファイル・ディレクトリ自動削除
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Collabo.h"
#include "C:\Factory\Meteor\7z.h"

static int OutputSameDir;
static int TrimTopDir;
static int WFileType = 'Z'; // "7Z"
static int OutputAndDelete;

static char *Get7zExeFile(void) // ret: 空白を含まないパスであること。
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

	errorCase(isAbsRootDir(dir)); // ルートDIR 不可

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
	cout("z7 圧縮\n");
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
		cout("z7 圧縮\n");
		cout("-------\n");
	}
}
