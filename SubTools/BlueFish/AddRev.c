#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Collabo.h"

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
	reverseElements(revDirs); // 終端 == 最も旧いリビジョン

	while(REV_MAX < getCount(revDirs))
	{
		char *revDir = (char *)unaddElement(revDirs); // 最も旧いリビジョンを取り出す。

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
		cout("アスキーコードの文字列じゃないのでスキップ\n");
		goto endFunc;
	}
	appDir = combine(docRoot, appName);
	cout("appDir: %s\n", appDir);

	if(!existDir(appDir))
	{
		cout("アプリケーションが無いのでスキップ\n");
		goto endFunc;
	}
	revision = GetRevision();
	revDir = combine(appDir, revision);
	cout("revDir: %s\n", revDir);

	createDir(revDir);

	wFile = combine(revDir, lfile);
	cout("wFile: %s\n", wFile);

	moveFile(arcFile, wFile);

	cout("★★★移動しました★★★\n");

	TrimRev(appDir);

endFunc:
	memFree(appName);
	memFree(appDir);
	memFree(revision);
	memFree(revDir);
	memFree(wFile);
}
static void ExtractCluster(char *cluster, char *rootDir)
{
	char *lCluster = getLocal(cluster);
	char *node;
	char *wDir;

	LOGPOS();

	node = changeExt(lCluster, "");
	wDir = combine(rootDir, node);

	cout("wDir: %s\n", wDir);

	if(existDir(wDir))
		coExecute_x(xcout("C:\\Factory\\Tools\\Cluster.exe /OAD /OW /R \"%s\" \"%s\"", cluster, wDir));

	memFree(node);
	memFree(wDir);

	LOGPOS();
}
static void AddRev(char *rDir, char *wDir, char *extCluWDir)
{
	rDir = makeFullPath(rDir);
	wDir = makeFullPath(wDir);
	extCluWDir = makeFullPath(extCluWDir);

	cout("< %s\n", rDir);
	cout("> %s\n", wDir);
	cout("> %s\n", extCluWDir);

	errorCase(!existDir(rDir));
	errorCase(!existDir(wDir));
	errorCase(!existDir(extCluWDir));

	{
		autoList_t *files = lsFiles(rDir);
		char *file;
		uint index;

		foreach(files, file, index)
		{
			AddRev_File(file, wDir);

			if(existFile(file) && !_stricmp("clu", getExt(file)))
			{
				ExtractCluster(file, extCluWDir);
			}
		}
		releaseDim(files, 1);
	}

	memFree(rDir);
	memFree(wDir);
	memFree(extCluWDir);
}
int main(int argc, char **argv)
{
	if(hasArgs(3))
	{
		AddRev(getArg(0), getArg(1), getArg(2));
		return;
	}
	AddRev(
		"C:\\pub\\リリース物",
		"C:\\BlueFish\\BlueFish\\HTT\\stackprobe\\home",
		"C:\\BlueFish\\BlueFish\\HTT"
		);
}
