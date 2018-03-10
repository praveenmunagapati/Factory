/*
	newcs c �v���W�F�N�g��
	newcs f �v���W�F�N�g��
	newcs t �v���W�F�N�g��
*/

#include "C:\Factory\Common\all.h"

static void RenamePaths(char *fromPtn, char *toPtn)
{
	autoList_t *paths = ls(".");
	char *path;
	char *nPath;
	uint index;

	eraseParents(paths);

	foreach(paths, path, index)
	{
		if(index < lastDirCount)
		{
			addCwd(path);
			{
				RenamePaths(fromPtn, toPtn);
			}
			unaddCwd();
		}
		nPath = replaceLine(strx(path), fromPtn, toPtn, 1);

		if(replaceLine_getLastReplacedCount())
		{
			coExecute_x(xcout("REN \"%s\" \"%s\"", path, nPath));
		}
		memFree(nPath);
	}
	releaseDim(paths, 1);
}
static void Main2(char *tmplProject, char *tmplDir)
{
	char *project = nextArg();

	errorCase(!existDir(tmplDir)); // 2bs ?

	errorCase_m(!lineExp("<1,30,__09AZaz>", project), "�s���ȃv���W�F�N�g���ł��B");
	errorCase_m(existPath(project), "���ɑ��݂��܂��B");

	createDir(project);
	copyDir(tmplDir, project);

	addCwd(project);
	{
		coExecute("qq -f");

		RenamePaths(tmplProject, project);

		coExecute_x(xcout("Search.exe %s", tmplProject));
		coExecute_x(xcout("trep.exe /F %s", project));

//		execute("START .");

		execute_x(xcout("%s.sln", project)); // zantei
		execute("START /MAX C:\\Dev\\CSharp\\Module2\\Module2"); // zantei
	}
	unaddCwd();
}
int main(int argc, char **argv)
{
	if(argIs("C"))
	{
		Main2("CCCC", "C:\\Dev\\CSharp\\Template\\CUIProgramTemplate");
		return;
	}
	if(argIs("F"))
	{
		Main2("FFFF", "C:\\Dev\\CSharp\\Template\\FormApplicationTemplate");
		return;
	}
	if(argIs("T"))
	{
		Main2("TTTT", "C:\\Dev\\CSharp\\Template\\TaskTrayTemplate");
		return;
	}
	cout("usage: newcs (C�bF�bT) �v���W�F�N�g��\n");
}
