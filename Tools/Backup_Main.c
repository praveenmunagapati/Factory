#include "C:\Factory\Common\all.h"
#include "C:\Factory\Meteor\Toolkit.h"

static int NoShutdownFlag;

static void ToDecAlphaOnly(char *line)
{
	char *p;

	for(p = line; *p; p++)
	{
		if(!m_isdecimal(*p) && !m_isalpha(*p))
		{
			*p = '_';
		}
	}
}
static void ToOnomast(char *line)
{
	char *p;

	line[0] = m_toupper(line[0]);

	for(p = line + 1; *p; p++)
	{
		*p = m_tolower(*p);
	}
}

static autoList_t *GetTargetDirs(void)
{
	autoList_t *paths = ls("C:\\");
	autoList_t dirs;
	char *dir;
	uint index;
	autoList_t *retDirs = createAutoList(16);
	char *userName;

	dirs = gndSubElements(paths, 0, lastDirCount);

	foreach(&dirs, dir, index)
	{
		if(
			!_stricmp(dir, "C:\\Documents and Settings") ||
			!_stricmp(dir, "C:\\PerfLogs") || // for 10
			!_stricmp(dir, "C:\\ProgramData") || // for 7
			!_stricmp(dir, "C:\\Program Files") ||
			!_stricmp(dir, "C:\\Program Files (x86)") || // for 7 x64
			!_stricmp(dir, "C:\\RECYCLER") ||
			!_stricmp(dir, "C:\\System Volume Information") ||
			!_stricmp(dir, "C:\\WINDOWS") || // for XP, 7
			!_stricmp(dir, "C:\\WINNT") || // for 2000
			!_stricmp(dir, "C:\\$Recycle.Bin") || // for 7
			!_stricmp(dir, "C:\\Config.Msi") || // for 7
			!_stricmp(dir, "C:\\Recovery") || // for 7
			!_stricmp(dir, "C:\\Users") || // for 7

			dir[3] == '_' || m_isdecimal(dir[3]) || // _ 0〜9 で始まるフォルダは対象外

//			!_stricmp(dir, "C:\\huge") || // del @ 2017.12.16

			0
			)
		{
			// noop
		}
		else
		{
			addElement(retDirs, (uint)strx(dir));
		}
	}
	releaseDim(paths, 1);
	rapidSortLines(retDirs);

	userName = getenv("UserName");

	errorCase(!userName);
	errorCase(!userName[0]);

	if(existDir("C:\\Users")) // ? 7
	{
		addElement(retDirs, (uint)xcout("C:\\Users\\%s\\Favorites", userName));
//		addElement(retDirs, (uint)xcout("C:\\Users\\%s\\Documents", userName)); // 権限が無いとか言われる。
		addElement(retDirs, (uint)xcout("C:\\Users\\%s\\Desktop", userName));
	}
	else // ? xp
	{
		addElement(retDirs, (uint)xcout("C:\\Documents and Settings\\%s\\Favorites", userName));
		addElement(retDirs, (uint)xcout("C:\\Documents and Settings\\%s\\My Documents", userName));
		addElement(retDirs, (uint)xcout("C:\\Documents and Settings\\%s\\デスクトップ", userName));
	}
	return retDirs;
}
static void CheckTargetDirs(autoList_t *dirs)
{
	autoList_t *localDirs = createAutoList(getCount(dirs));
	char *dir;
	uint index;

	foreach(dirs, dir, index)
	{
		cout("%s\n", dir);
		errorCase(!existDir(dir));
		addElement(localDirs, (uint)getLocal(dir));
	}
	cout("\n");

	foreach(localDirs, dir, index)
	{
		autoList_t followDirs = gndFollowElements(localDirs, index + 1);
		errorCase(findJLineICase(&followDirs, dir) < getCount(&followDirs)); // ? 重複した。
	}
	releaseAutoList(localDirs);
}

#define BATCH_BACKUP "C:\\Factory\\tmp\\Backup.bat"

static void BackupDirs(autoList_t *targetDirs)
{
	char *targetDir;
	uint index;

	foreach(targetDirs, targetDir, index)
	{
		char *destDir = getLocal(targetDir);
		FILE *fp;
		char *line;

		cmdTitle_x(xcout("Backup - %u / %u (%u) -S=%d", index, getCount(targetDirs), getCount(targetDirs) - index, NoShutdownFlag));

		fp = fileOpen(BATCH_BACKUP, "wt");

		writeLine(fp, "SET COPYCMD=");
		writeLine(fp, line = xcout("MD \"%s\"", destDir)); memFree(line);
		writeLine(fp, line = xcout("ROBOCOPY.EXE \"%s\" \"%s\" /MIR", targetDir, destDir)); memFree(line);
		writeLine(fp, "ECHO ERRORLEVEL=%ERRORLEVEL%");

		fileClose(fp);

		cout("コピーしています...\n");

		execute(line = xcout("START /B /WAIT CMD /C %s", BATCH_BACKUP)); memFree(line);

		cout("コピーしました。\n");
		cout("\n");
	}
	cmdTitle("Backup - done");
}

// ---- huge dir ----

static autoList_t *GetCatalog(char *dir)
{
	autoList_t *files =lssFiles(dir);
	char *file;
	uint index;
	autoList_t *catalog = newList();

	foreach(files, file, index)
	{
		uint64 wTime;

		getFileStamp(file, NULL, NULL, &wTime);
		wTime &= ~1ui64; // for FAT
		addElement(catalog, (uint)xcout("%020I64u*%020I64u*%s", wTime, getFileSize(file), eraseRoot(file, dir)));
	}
	releaseDim(files, 1);
	sortJLinesICase(catalog);
	return catalog;
}
static char *GetFileByCatalogLine(char *line, char *rootDir)
{
	char *p = line;

	p = ne_strchr(p, '*') + 1;
	p = ne_strchr(p, '*') + 1;
	return combine(rootDir, p);
}
static uint64 GetWTimeByCatalogLine(char *line)
{
	uint64 wTime;

	line = strx(line);
	ne_strchr(line, '*')[0] = '\0';
	wTime = toValue64(line);
	memFree(line);
	return wTime;
}
static void DeleteEmptyDirs(char *rootDir)
{
	autoList_t *dirs = lssDirs(rootDir);
	char *dir;
	uint index;

	reverseElements(dirs);

	foreach(dirs, dir, index)
	{
		uint count = lsCount(dir);

		if(!count)
		{
			cout("# %s\n", dir);

			removeDir(dir);
		}
	}
	releaseDim(dirs, 1);
}

#define HUGE_DIR "C:\\huge"

static void BackupHugeDir(char *destDir)
{
	autoList_t *rCatalog;
	autoList_t *wCatalog;
	autoList_t *addCatalog;
	autoList_t *removeCatalog;
	char *line;
	uint index;

	LOGPOS();

	if(!existDir(HUGE_DIR))
		return;

	destDir = xcout("%s_huge", destDir);
	createDirIfNotExist(destDir);

	coExecute_x(xcout("Compact.exe /C \"%s\"", destDir));
//	coExecute_x(xcout("Compact.exe /C /S:\"%s\"", destDir));

	LOGPOS();

	rCatalog = GetCatalog(HUGE_DIR);
	wCatalog = GetCatalog(destDir);
	addCatalog = newList();
	removeCatalog = newList();

	mergeLines2(rCatalog, wCatalog, addCatalog, NULL, removeCatalog);

	foreach(removeCatalog, line, index)
	{
		char *file = GetFileByCatalogLine(line, destDir);

		cout("! %s\n", file);

		errorCase(!existFile(file)); // 2bs?

		removeFile(file);
		memFree(file);
	}
	DeleteEmptyDirs(destDir);

	foreach(addCatalog, line, index)
	{
		char *rFile = GetFileByCatalogLine(line, HUGE_DIR);
		char *wFile = GetFileByCatalogLine(line, destDir);
		uint64 wTime = GetWTimeByCatalogLine(line);

		cout("< %s\n", rFile);
		cout("> %s\n", wFile);
		cout("T %I64u\n", wTime);

		errorCase(!existFile(rFile)); // 2bs?
		errorCase( existFile(wFile)); // 2bs?

		createPath(wFile, 'X');
		copyFile(rFile, wFile);
		setFileStamp(wFile, 0ui64, 0ui64, wTime);
		memFree(rFile);
		memFree(wFile);
	}
	memFree(destDir);
	releaseDim(rCatalog, 1);
	releaseDim(wCatalog, 1);
	releaseAutoList(addCatalog);
	releaseAutoList(removeCatalog);

	LOGPOS();
}

// ----

int main(int argc, char **argv)
{
	char *strDestDrv;
	int destDrv;
	char *pcname;
	char *destDir;
	char *destBackDir;
	autoList_t *targetDirs;
	char *cmdln;
	int doShutdownFlag;

	// 外部コマンド存在確認
	{
		errorCase(!existFile(FILE_TOOLKIT_EXE));
	}

	NoShutdownFlag = argIs("/-S");

	if(!NoShutdownFlag)
	{
		cout("********************************\n");
		cout("** 終了後シャットダウンします **\n");
		cout("********************************\n");
	}
	cout("+---------------------------------------------+\n");
	cout("| メーラーなど、動作中のアプリを閉じて下さい。|\n");
	cout("+---------------------------------------------+\n");
	cout("+---------------------------------------+\n");
	cout("| ウィルス対策ソフトを無効にして下さい。|\n");
	cout("+---------------------------------------+\n");
	cout("バックアップ先ドライブをドロップして下さい。\n");

	strDestDrv = dropPath();

	if(!strDestDrv)
		termination(0);

	destDrv = strDestDrv[0];

	errorCase(!m_isalpha(destDrv));
	errorCase(destDrv == 'C');

	pcname = getenv("ComputerName");

	errorCase(!pcname);
	errorCase(!pcname[0]);
	errorCase(strchr(pcname, ' '));

	pcname = strx(pcname);
	ToDecAlphaOnly(pcname);
	ToOnomast(pcname);
	destDir = xcout("%c:\\%s", destDrv, pcname);
	destBackDir = xcout("%s_", destDir);
	memFree(pcname);

	cout("出力先: %s (%s)\n", destDir, destBackDir);
	cout("\n");

	targetDirs = GetTargetDirs();
	CheckTargetDirs(targetDirs);

	if(existDir(destBackDir))
	{
		cout("前々回のバックアップを削除しています...\n");

		cmdln = xcout("RD /S /Q %s", destBackDir);
		execute(cmdln);
		memFree(cmdln);

		cout("削除しました。\n");
		cout("\n");
	}
	errorCase(existPath(destBackDir));

	if(existDir(destDir))
	{
		cout("前回のバックアップをバックアップしています...\n");

		cmdln = xcout("REN %s %s", destDir, getLocal(destBackDir));
		execute(cmdln);
		memFree(cmdln);

		cout("バックアップしました。\n");
		cout("\n");
	}
	errorCase(existPath(destDir));
	createDir(destDir);

	coExecute_x(xcout("Compact.exe /C /S:\"%s\"", destDir));

	addCwd(destDir);
	BackupDirs(targetDirs);
	unaddCwd();
//	BackupHugeDir(destDir); // del @ 2017.12.16

	// _ 0〜9 で始まる名前はバックアップ対象外なので、destDir の直下は _ 0〜9 で始めておけば重複しないはず。

#if 1 // Toolkit が USB HDD への書き込みエラーを起こしたので、書き込み先をシステムドライブにして様子を見る。@ 2018.3.5
	{
	char *midDir = makeTempDir("Backup_Hash_txt_mid");

	coExecute_x(xcout(FILE_TOOLKIT_EXE " /SHA-512-128 %s %s\\_Hash.txt", destDir, midDir));

	coExecute_x(xcout("COPY /Y %s\\_Hash.txt %s\\_Hash.txt", midDir, destDir));
	coExecute_x(xcout("COPY /Y %s\\_Hash.txt C:\\tmp\\Backup_Hash.txt", midDir));
	coExecute_x(xcout("COPY /Y %s\\_Hash.txt C:\\tmp\\Backup_Hash_old.txt", destBackDir)); // 無いかもしれない。

	recurRemoveDir_x(midDir);
	}
#else // del @ 2018.3.5
	coExecute_x(xcout(FILE_TOOLKIT_EXE " /SHA-512-128 %s %s\\_Hash.txt", destDir, destDir));

	coExecute_x(xcout("COPY /Y %s\\_Hash.txt C:\\tmp\\Backup_Hash.txt", destDir));
	coExecute_x(xcout("COPY /Y %s\\_Hash.txt C:\\tmp\\Backup_Hash_old.txt", destBackDir)); // 無いかもしれない。
#endif

	// Backup.bat との連携
	{
		if(existDir("C:\\888"))
		{
			writeOneLine_cx("C:\\888\\Extra_0001.bat", xcout("COPY /Y C:\\tmp\\Backup.log %s\\_Backup.log", destDir));
		}
	}

	memFree(strDestDrv);
	memFree(destDir);
	memFree(destBackDir);
	releaseDim(targetDirs, 1);

	cout("\\e\n");

	if(!NoShutdownFlag)
	{
		coExecute("shutdown -s -t 30");
	}
}
