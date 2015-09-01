#include "C:\Factory\Common\all.h"

#define REV_MAX 100

static int IsAsciiStr(char *str)
{
	char *p;

	for(p = str; *p; p++)
		if(!m_isRange(*p, '\x21', '\x7e'))
			return 0;

	return 1;
}
static char *GetRevision(void)
{
	char *file = makeTempFile(NULL);
	char *revision;

	coExecute_x(xcout("C:\\Factory\\DevTools\\rev.exe /P > \"%s\"", file));
	revision = readFirstLine(file);

	errorCase(!lineExp("<4,09>.<3,09>.<5,09>", revision)); // 2bs

	removeFile(file);
	memFree(file);
	return revision;
}
static void TrimRev(char *appDir)
{
	autoList_t *revDirs = lsDirs(appDir);

	sortJLinesICase(revDirs);
	reverseElements(revDirs); // �I�[ == �ł��������r�W����

	while(REV_MAX < getCount(revDirs))
	{
		char *revDir = (char *)unaddElement(revDirs); // �ł��������r�W���������o���B

		cout("[DEL_REV] %s\n", revDir);

		errorCase(!lineExp("<4,09>.<3,09>.<5,09>", getLocal(revDir))); // 2bs

		forceRemoveDir(revDir);
		memFree(revDir);
	}
	releaseDim(revDirs, 1);
}
static void AddRev_File(char *arcFile, char *docRoot)
{
	char *lfile = getLocal(arcFile);
	char *ext;
	char *appName = NULL;
	char *appDir = NULL;
	char *revision = NULL;
	char *revDir = NULL;
	char *wFile = NULL;

	cout("arcFile: %s\n", arcFile);
	cout("docRoot: %s\n", docRoot);

	ext = getExt(lfile);
	appName= changeExt(lfile, "");

	if(!IsAsciiStr(appName))
	{
		cout("�A�X�L�[�R�[�h�̕����񂶂�Ȃ��̂ŃX�L�b�v\n");
		goto endFunc;
	}
	appDir = combine(docRoot, appName);
	cout("appDir: %s\n", appDir);

	if(!existDir(appDir))
	{
		cout("�A�v���P�[�V�����������̂ŃX�L�b�v\n");
		goto endFunc;
	}
	revision = GetRevision();
	revDir = combine(appDir, revision);
	cout("revDir: %s\n", revDir);

	createDir(revDir);

	wFile = combine(revDir, lfile);
	cout("wFile: %s\n", wFile);

	moveFile(arcFile, wFile);

	cout("�������ړ����܂���������\n");

	TrimRev(appDir);

endFunc:
	memFree(appName);
	memFree(appDir);
	memFree(revision);
	memFree(revDir);
	memFree(wFile);
}
static void AddRev(char *rDir, char *wDir)
{
	rDir = makeFullPath(rDir);
	wDir = makeFullPath(wDir);

	cout("< %s\n", rDir);
	cout("> %s\n", wDir);

	errorCase(!existDir(rDir));
	errorCase(!existDir(wDir));

	{
		autoList_t *files = lsFiles(rDir);
		char *file;
		uint index;

		foreach(files, file, index)
		{
			AddRev_File(file, wDir);
		}
		releaseDim(files, 1);
	}

	memFree(rDir);
	memFree(wDir);
}
int main(int argc, char **argv)
{
	if(hasArgs(2))
	{
		AddRev(getArg(0), getArg(1));
		return;
	}
	AddRev("C:\\pub\\�����[�X��", "C:\\BlueFish\\BlueFish\\HTT\\stackprobe\\home");
}
