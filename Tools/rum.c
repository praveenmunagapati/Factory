/*
	�V���v���E�o�[�W�����Ǘ��v���O����

	rum.exe [/F] [/Q] [/-COLL] [�Ώۃf�B���N�g��]

		/F ... �o�C�i�����O���[�h(Factory���[�h) == .obj .exe C:\\Factory\\tmp �𖳎�
		/Q ... �₢���킹��}�~����B
		/-COLL ... �R���W�����������s��Ȃ��B���������Ȃ邩���B

	rum.exe [/E | /T | /HA | /H | /1A | /1 | /R] [/D] [.rum_�f�B���N�g��]

		/E  ... �R�����g�ҏW
		/T  ... ���݃t�@�C���폜
		/HA ... �t�@�C���̕ύX�������擾�A�t�@�C���̈ꗗ��S���r�W��������擾
		/H  ... �t�@�C���̕ύX�������擾�A�t�@�C���̈ꗗ���Ō�̃��r�W��������擾
		/1A ... �Ō�̃t�@�C�����擾�A�t�@�C���̈ꗗ��S���r�W��������擾
		/1  ... �Ō�̃t�@�C�����擾�A�t�@�C���̈ꗗ���Ō�̃��r�W��������擾
		/R  ... �Ō�̃��r�W�������C���������R�����g�����ł���΍폜����B
		/D  ... ��� C:\\NNN �Ƀ��X�g�A����B(NNN �� 1�`999)

	----
	�g����

	"����!" �Ƃ����f�B���N�g����ۑ�(���|�W�g���݂����Ȃ��̂��쐬)����B

	> rum ����!

	�e�L�X�g�G�f�B�^���N������̂ŃR�����g���L�����ĕۑ�����B
	���s�H�ŃG�X�P�[�v�L�[�������ƃL�����Z������B
	���s����� "����!.rum" �����������B
	2��ڈȍ~�������R�}���h�A�����菇�B

	- - -

	��������ɂ͈ȉ��̃R�}���h�����s����B

	> rum ����!.rum

	�e�L�X�g�G�f�B�^���N������̂ŕ������郊�r�W�����̍s���폜���邩�A�������郊�r�W�����̍s�u�ȊO�v���폜���ĕۑ�����B
	��������s���폜�����ꍇ�� L ��������s�u�ȊO�v���폜�����ꍇ�� R �������B
	"����!" �������ꍇ�� "����!" �ɁA"����!" ���L��ꍇ�� C:\\NNN �Ƀ��X�g�A����B(NNN �� 1�`999)

	C:\\NNN �� zz �ň�Ăɍ폜�ł���B�����ӂ��Ďg���ĂˁB

	----
	����

	�����ƃ^�C���X�^���v�͕ۑ����Ȃ��B
	����MD5�œ��e���قȂ�t�@�C���͓o�^�ł��Ȃ��B-> �G���[�ɂȂ�B(���r�W�����͍쐬����Ȃ��B.rum �͉��Ȃ����Arum /t �K�v)
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\OpenSource\md5.h"

#define EXT_STOREDIR "rum"

#define DIR_FILES "files"
#define DIR_REVISIONS "revisions"

#define FILE_REV_COMMENT "comment.txt"
#define FILE_REV_FILES "files.txt"
#define FILE_REV_TREE "tree.txt"

#define DEFAULT_COMMENT "�R�����g����"

static uint ErrorLevel;

static char *GetComment(char *storeDir, char *stamp)
{
	char *comment;

	addCwd(storeDir);
	addCwd(DIR_REVISIONS);
	addCwd(stamp);

	comment = readFirstLine(FILE_REV_COMMENT);
	errorCase(m_isEmpty(comment)); // 2bs

	unaddCwd();
	unaddCwd();
	unaddCwd();

	return comment;
}
static autoList_t *MakeRevisionList(char *storeDir)
{
	autoList_t *revisions;

	addCwd(storeDir);
	revisions = ls(DIR_REVISIONS);
	unaddCwd();

	eraseParents(revisions);
	rapidSortLines(revisions);

	// �R�����g�̒ǉ�
	{
		char *stamp;
		uint index;

		foreach(revisions, stamp, index)
		{
			char *comment;

			addCwd(storeDir);
			addCwd(DIR_REVISIONS);
			addCwd(stamp);

			comment = readFirstLine(FILE_REV_COMMENT);
			line2JLine(comment, 1, 0, 0, 1);
			trim(comment, ' ');

			errorCase(!*comment); // 2bs

			stamp = addLine_x(stamp, xcout(" %s", comment));
			setElement(revisions, index, (uint)stamp);

			memFree(comment);

			unaddCwd();
			unaddCwd();
			unaddCwd();
		}
	}
	return revisions;
}
static autoList_t *SelectStamps(char *storeDir)
{
	autoList_t *revisions = MakeRevisionList(storeDir);
	autoList_t *stamps;
	char *stamp;
	uint index;
	char *p;

	stamps = selectLines(revisions);

	foreach(stamps, stamp, index)
	{
		p = strchr(stamp, '\x20');
		errorCase(!p);
		*p = '\0';
	}
	releaseDim(revisions, 1);
	return stamps;
}

static void EditComment(char *storeDir)
{
	autoList_t *revisions = MakeRevisionList(storeDir);
	char *line;
	uint index;

	cout("+----------------+\n");
	cout("| �R�����g�̕ҏW |\n");
	cout("+----------------+\n");
	sleep(500);

	revisions = editLines_x(revisions);

	cout("���s�H\n");

	if(clearGetKey() == 0x1b)
		goto cancelled;

	foreach(revisions, line, index)
	{
		line2JLine(line, 1, 0, 0, 1);
		trim(line, ' ');

		if(*line)
		{
			char *stamp = line;
			char *comment = "";
			char *p;

			p = strchr(stamp, '\x20');

			if(p)
			{
				*p = '\0';
				comment = p + 1;
			}
			errorCase(!lineExp("<14,09>", stamp));

			if(!*comment)
				comment = DEFAULT_COMMENT;

			cout("stamp: %s\n", stamp);
			cout("comment: %s\n", comment);

			addCwd(storeDir);
			addCwd(DIR_REVISIONS);
			addCwd(stamp);

			writeOneLine(FILE_REV_COMMENT, comment);

			unaddCwd();
			unaddCwd();
			unaddCwd();
		}
	}

cancelled:
	releaseDim(revisions, 1);
}
static void EraseLostEntries(char *storeDir)
{
	autoList_t *stockFiles;
	autoList_t *revisions;
	char *stamp;
	uint index;

	addCwd(storeDir);

	stockFiles = ls(DIR_FILES);
	eraseParents(stockFiles);

	addCwd(DIR_REVISIONS);

	revisions = ls(".");
	foreach(revisions, stamp, index)
	{
		autoList_t *entries;
		char *entry;
		uint entryIndex;
		int entriesModified = 0;

		addCwd(stamp);
		entries = readLines(FILE_REV_FILES);

	entriesRestart:
		foreach(entries, entry, entryIndex)
		{
			char *p = strchr(entry, '\x20');
			char *hash;

			errorCase(!p);
			*p = '\0';
			hash = strx(entry);
			*p = '\x20';

			if(findLineCase(stockFiles, hash, 1) == getCount(stockFiles)) // ? not found
			{
				cout("�G���g���[�̎��̂������Ă��܂��B\n");
				cout("%s\n", stamp);
				cout("%s\n", entry);
				cout("�폜�H\n");

				if(clearGetKey() != 0x1b)
				{
					cout("�폜���܂��B\n");

					memFree((char *)desertElement(entries, entryIndex));
					entriesModified = 1;
					goto entriesRestart;
				}
			}

			if(pulseSec(2, NULL))
				execute_x(xcout("TITLE rum - E / %u(%u)_%u(%u)", entryIndex, getCount(entries), index, getCount(revisions)));
		}
		if(entriesModified)
		{
			cout("�G���g���[�X�V��...\n");
			writeLines(FILE_REV_FILES, entries);
			cout("�G���g���[�X�V�I��\n");
		}
		releaseDim(entries, 1);
		unaddCwd();
	}
	execute("TITLE rum");

	unaddCwd();
	unaddCwd();

	releaseDim(stockFiles, 1);
	releaseDim(revisions, 1);
}
static void TrimStoreDir(char *storeDir)
{
	autoList_t *stockFiles;
	autoList_t *revisions;
	char *stockFile;
	char *stamp;
	uint index;

	cout("+----------------------------+\n");
	cout("| �Q�Ƃ���Ȃ��t�@�C���̍폜 |\n");
	cout("+----------------------------+\n");
	sleep(500);

	addCwd(storeDir);

	stockFiles = ls(DIR_FILES);
	eraseParents(stockFiles);

	revisions = ls(DIR_REVISIONS);

	foreach(revisions, stamp, index)
	{
		char *filesFile = combine(stamp, FILE_REV_FILES);
		autoList_t *stocks;
		char *line;
		uint lineIndex;

		stocks = readLines(filesFile);

		foreach(stocks, line, lineIndex)
		{
			char *p = strchr(line, '\x20');
			uint foundIndex;

			errorCase(!p);
			*p = '\0';

			errorCase(!lineExp("<32,09afAF>", line)); // 2bs

			foundIndex = findLineCase(stockFiles, line, 1);
			if(foundIndex < getCount(stockFiles))
			{
				memFree((void *)fastDesertElement(stockFiles, foundIndex));
			}

			if(pulseSec(2, NULL))
				execute_x(xcout("TITLE rum - %u / %u(%u)_%u(%u)", getCount(stockFiles), lineIndex, getCount(stocks), index, getCount(revisions)));
		}
		memFree(filesFile);
		releaseDim(stocks, 1);
	}
	execute("TITLE rum");

	addCwd(DIR_FILES);

	if(getCount(stockFiles))
	{
		foreach(stockFiles, stockFile, index)
		{
			cout("* %s\n", stockFile);
		}
		cout("�폜�H\n");

		if(clearGetKey() != 0x1b)
		{
			cout("�폜���܂��B\n");

			foreach(stockFiles, stockFile, index)
			{
				cout("! %s\n", stockFile);
				removeFile(stockFile);
			}
		}
	}
	unaddCwd();
	unaddCwd();

	EraseLostEntries(storeDir);
}
static void FileHistory(char *storeDir, int fromLastRevisionFlag, int lastFileOnly)
{
	autoList_t *revisions;
	autoList_t *srchRevisions;
	char *stamp;
	uint index;
	autoList_t *allStock = newList();
	autoList_t *selStocks;
	char *selStock;
	uint selStockIdx;
	char *restoreRootDir;
	char *outStockTestDir = makeTempDir(NULL);

	cout("+--------------+\n");
	cout("| �t�@�C������ |\n");
	cout("+--------------+\n");
	sleep(500);

	addCwd(storeDir);
	revisions = ls(DIR_REVISIONS);
	rapidSortLines(revisions);

	if(fromLastRevisionFlag)
	{
		srchRevisions = newList();
		addElement(srchRevisions, getLastElement(revisions));
	}
	else
		srchRevisions = copyAutoList(revisions);

	if(lastFileOnly)
		reverseElements(revisions);

	foreach(srchRevisions, stamp, index)
	{
		char *filesFile = combine(stamp, FILE_REV_FILES);
		autoList_t *stocks;
		char *line;
		uint lineIndex;

		stocks = readLines(filesFile);

		foreach(stocks, line, lineIndex)
		{
			char *p = strchr(line, '\x20');

			errorCase(!p);
			p++;
			errorCase(!*p);

			addElement(allStock, (uint)strx(p));
		}
		memFree(filesFile);
		releaseDim(stocks, 1);
	}
	allStock = autoDistinctJLinesICase(allStock);
	selStocks = selectLines(allStock);

	if(!getCount(selStocks))
		goto noSelStocks;

	restoreRootDir = makeFreeDir();

	foreach(selStocks, selStock, selStockIdx)
	{
		autoList_t *histRevs = newList();
		autoList_t *histHashes = newList();
		char *outStock;

		cout("selStock: %s\n", selStock);

		outStock = combine(outStockTestDir, getLocal(selStock));
		outStock = toCreatablePath(outStock, selStockIdx);
		createFile(outStock);
		eraseParent(outStock);

		foreach(revisions, stamp, index)
		{
			char *filesFile = combine(stamp, FILE_REV_FILES);
			autoList_t *stocks;
			char *line;
			uint lineIndex;

			stocks = readLines(filesFile);

			foreach(stocks, line, lineIndex)
			{
				char *hash = line;
				char *stock = strchr(line, '\x20');

				errorCase(!stock);
				*stock = '\0';
				stock++;
				errorCase(!*hash);
				errorCase(!*stock);

				if(!_stricmp(stock, selStock))
				{
					int modified = !getCount(histHashes) || _stricmp(hash, getLine(histHashes, getCount(histHashes) - 1));
					char *lclStamp = getLocal(stamp);

					cout("# %s %s %s\n", lclStamp, hash, modified ? "�V�K�܂��͕ύX�A��" : "�ύX�i�V");

					if(modified)
					{
						addElement(histRevs, (uint)strx(lclStamp));
						addElement(histHashes, (uint)strx(hash));
					}
					break; // ���������B
				}
			}
			memFree(filesFile);
			releaseDim(stocks, 1);

			if(lastFileOnly && getCount(histRevs))
				break;
		}
		addCwd(DIR_FILES);

		foreach(histRevs, stamp, index)
		{
			char *restoreDir = combine(restoreRootDir, stamp);
			char *restoreFile;

#if 1 // ���Ƀt�@�C��
			restoreFile = xcout("%s_%s", restoreDir, outStock);
#else // �f�B���N�g���̉��Ƀt�@�C��
			restoreFile = combine(restoreDir, outStock);
			createDir(restoreDir);
#endif
			copyFile(getLine(histHashes, index), restoreFile);
			memFree(restoreDir);
			memFree(restoreFile);
		}
		memFree(outStock);
		releaseDim(histRevs, 1);
		releaseDim(histHashes, 1);
		unaddCwd();
	}
	execute_x(xcout("START \"\" \"%s\"", restoreRootDir));
	memFree(restoreRootDir);
noSelStocks:
	releaseDim(revisions, 1);
	releaseAutoList(srchRevisions);
	releaseDim(allStock, 1);
	releaseDim(selStocks, 1);
	recurRemoveDir_x(outStockTestDir);
	unaddCwd();
}
static void RemoveLastRevIfNoMod(char *storeDir, int quietFlag)
{
	autoList_t *revisions;
	char *stamp1;
	char *stamp2;
	char *filesFile1;
	char *filesFile2;
	char *treeFile1;
	char *treeFile2;

	addCwd(storeDir);
	addCwd(DIR_REVISIONS);
	revisions = ls(".");

	if(getCount(revisions) < 2)
	{
		cout("�����̃��r�W����������܂���B\n");
		goto endFunc;
	}
	eraseParents(revisions);
	rapidSortLines(revisions);

	stamp1 = getLine(revisions, getCount(revisions) - 2);
	stamp2 = getLine(revisions, getCount(revisions) - 1);

	filesFile1 = combine(stamp1, FILE_REV_FILES);
	filesFile2 = combine(stamp2, FILE_REV_FILES);
	treeFile1 = combine(stamp1, FILE_REV_TREE);
	treeFile2 = combine(stamp2, FILE_REV_TREE);

	if(
		!isSameFile(filesFile1, filesFile2) ||
		!isSameFile(treeFile1, treeFile2)
		)
	{
		cout("�Ō�̃��r�W�����͍X�V����Ă��܂��B\n");
	}
	else if(strcmp(DEFAULT_COMMENT, GetComment(storeDir, stamp2))) // ? != DEFAULT_COMMENT
	{
		cout("�Ō�̃��r�W�����͍X�V����Ă��܂��񂪁A�R�����g���L�q����Ă��܂��B\n");
	}
	else
	{
		char *removeTargetDir = makeFullPath(stamp2);

		cout("+--------------------------------------+\n");
		cout("| �Ō�̃��r�W�����͍X�V����Ă��܂��� |\n");
		cout("+--------------------------------------+\n");
		cout("> %s\n", removeTargetDir);
		cout("�폜�H\n");

		if(quietFlag || clearGetKey() != 0x1b)
		{
			cout("�폜���܂��B\n");
			recurRemoveDir(removeTargetDir);
			cout("�폜���܂����B\n");
		}
		memFree(removeTargetDir);
	}
	memFree(filesFile1);
	memFree(filesFile2);
	memFree(treeFile1);
	memFree(treeFile2);
endFunc:
	releaseDim(revisions, 1);

	unaddCwd();
	unaddCwd();
}
static void OneRestore(char *storeDir, char *targetStamp, char *restoreDir) // storeDir: �o�b�N�A�b�v��, restoreDir: ���쐬�̏o�͐�f�B���N�g��
{
	autoList_t *dirs;
	autoList_t *stocks;
	char *line;
	uint index;

	addCwd(storeDir);
	addCwd(DIR_REVISIONS);
	addCwd(targetStamp);

	dirs = readLines(FILE_REV_TREE);
	stocks = readLines(FILE_REV_FILES);

	unaddCwd();
	unaddCwd();

	createDir(restoreDir);
	addCwd(restoreDir);

	foreach(dirs, line, index)
		createDir(line);

	unaddCwd();

	addCwd(DIR_FILES);

	foreach(stocks, line, index)
	{
		char *stockFile = line;
		char *file;

		file = strchr(stockFile, '\x20');
		errorCase(!file);

		file[0] = '\0';
		file++;

		errorCase(!*stockFile);
		errorCase(!*file);

		file = combine(restoreDir, file);

		cout("< %s\n", stockFile);
		cout("> %s\n", file);

		copyFile(stockFile, file);

		memFree(file);
	}
	unaddCwd();
	unaddCwd();

	releaseDim(dirs, 1);
	releaseDim(stocks, 1);
}
static void MultiRestore(char *storeDir, autoList_t *stamps)
{
	char *restoreRootDir = makeFreeDir();
	char *restoreDir;
	char *targetStamp;
	uint index;

	cout("�����惋�[�g: %s\n", restoreRootDir);

	foreach(stamps, targetStamp, index)
	{
		restoreDir = combine(restoreRootDir, targetStamp);
		cout("�X�^���v�ƕ�����: %u(%u), %s > %s\n", index, getCount(stamps), targetStamp, restoreDir);

		OneRestore(storeDir, targetStamp, restoreDir);

		memFree(restoreDir);
	}
	execute_x(xcout("START \"\" \"%s\"", restoreRootDir));
	memFree(restoreRootDir);

	cout("�����̃��r�W�����𕜌����܂����B\n");
}

static int RestoreFreeDirMode;

static void Checkout(char *dir) // dir: �o�b�N�A�b�v��A���݂��郋�[�g�f�B���N�g���ł͂Ȃ��f�B���N�g���̐�΃p�X
{
	char *restoreDir;
	char *targetStamp;

	// I/O Test
	{
		errorCase(!existDir(dir));

		addCwd(dir);

		errorCase(!existDir(DIR_FILES));
		errorCase(!existDir(DIR_REVISIONS));

		unaddCwd();
	}

	restoreDir = changeExt(dir, "");
	cout("������: %s\n", restoreDir);

	// select stamps
	{
		autoList_t *stamps = SelectStamps(dir);

		switch(getCount(stamps))
		{
		case 0:
			releaseDim(stamps, 1);
			goto endFunc;

		case 1:
			targetStamp = getLine(stamps, 0);
			releaseAutoList(stamps);
			break;

		default:
			MultiRestore(dir, stamps);
			releaseDim(stamps, 1);
			goto endFunc;
		}
	}
	cout("��������X�^���v: %s\n", targetStamp);

	if(existDir(restoreDir))
	{
		eraseParent(restoreDir);
		restoreDir = combine_xx(makeFreeDir(), restoreDir);
		cout("���̕�����: %s\n", restoreDir);
	}
	if(RestoreFreeDirMode)
	{
		memFree(restoreDir);
		restoreDir = makeFreeDir();
		restoreDir = combine_xc(restoreDir, "_");
		cout("�X�Ɏ��̕�����: %s\n", restoreDir);
	}
	OneRestore(dir, targetStamp, restoreDir);
	execute_x(xcout("START \"\" \"%s\"", c_getParent(restoreDir)));

endFunc:
	memFree(restoreDir);
}

static int WithoutExeObjMode;
static int QuietMode;
static int NoCheckCollision;

static void Commit(char *dir) // dir: �o�b�N�A�b�v���A���݂��郋�[�g�f�B���N�g���ł͂Ȃ��f�B���N�g���̐�΃p�X
{
	char *storeDir;
	char *targetStamp;
	char *comment;

	storeDir = strx(dir);
	storeDir = addExt(storeDir, EXT_STOREDIR);
	cout("�i�[��: %s\n", storeDir);

	targetStamp = makeCompactStamp(NULL);
	cout("�i�[����X�^���v: %s\n", targetStamp);

	// I/O Test
	{
		if(existDir(storeDir))
		{
			addCwd(storeDir);

			errorCase(!existDir(DIR_FILES));
			errorCase(!existDir(DIR_REVISIONS));

			addCwd(DIR_REVISIONS);
			errorCase(existPath(targetStamp));
			createDir(targetStamp);
			removeDir(targetStamp);
			unaddCwd();

			unaddCwd();
		}
		else
		{
			cout("##################\n");
			cout("## �i�[�斢�쐬 ##\n");
			cout("##################\n");
			sleep(500);

			createDir(storeDir);
			removeDir(storeDir);
		}
	}

	if(!QuietMode)
	{
		comment = inputLine();
		line2JLine(comment, 1, 0, 0, 1);
		trim(comment, ' ');

		if(!*comment)
			comment = addLine(comment, DEFAULT_COMMENT);
	}
	else
		comment = strx(DEFAULT_COMMENT);

	cout("�R�����g: %s\n", comment);

	if(!existDir(storeDir))
	{
		cout("######################################\n");
		cout("## ���s����Ɗi�[���V�K�쐬���܂� ##\n");
		cout("######################################\n");
	}
	if(!QuietMode)
	{
		cout("���s�H\n");

		if(clearGetKey() == 0x1b)
		{
			ErrorLevel = 1;
			goto cancelled;
		}
		cout("���s���܂��B\n");
	}

	if(!existDir(storeDir))
	{
		createDir(storeDir);
		execute_x(xcout("Compact.exe /C \"%s\"", storeDir)); // ���k

		addCwd(storeDir);
		createDir(DIR_FILES);
		createDir(DIR_REVISIONS);
		unaddCwd();
	}
	addCwd(storeDir);

	// make new revision
	{
		autoList_t *paths = lss(dir);
		autoList_t dirs;
		autoList_t files;
		autoList_t *stockFiles = newList();
		char *file;
		uint index;

		if(WithoutExeObjMode)
		{
			uint dirKilledNum = 0;

			foreach(paths, file, index)
			{
#if 0
				if(index < lastDirCount)
				{
					1; // noop
				}
				else
				{
					if(
						!_stricmp("exe", getExt(file)) ||
						!_stricmp("obj", getExt(file))
						)
					{
						file[0] = '\0';
					}
				}
#else // tmp �z���͎��O�ɃN���A����悤�ɂ����B-> Counter.txt �������Ă��܂��̂ŕ����B
				if(index < lastDirCount)
				{
					if(startsWithICase(file, "C:\\Factory\\tmp\\")) // ? tmp �z���̃f�B���N�g��
					{
						file[0] = '\0';
						dirKilledNum++;
					}
				}
				else
				{
					if(
						!_stricmp("exe", getExt(file)) ||
						!_stricmp("obj", getExt(file)) ||
						startsWithICase(file, "C:\\Factory\\tmp\\") // ? tmp �z���̃t�@�C��
						)
					{
						file[0] = '\0';
					}
				}
#endif
			}
			trimLines(paths);
			lastDirCount -= dirKilledNum;
		}

		dirs = gndSubElements(paths, 0, lastDirCount);
		files = gndFollowElements(paths, lastDirCount);

		sortJLinesICase(&dirs);
		sortJLinesICase(&files);

		// I/O Test
		{
			cout("�ǂݍ��݃e�X�g��\n");

			foreach(&files, file, index)
			{
				FILE *fp = fileOpen(file, "rb");

				readChar(fp);
				readChar(fp);
				readChar(fp);

				fileClose(fp);
			}
			cout("�ǂݍ��݃e�X�gok!\n");
		}
		addCwd(DIR_FILES);

		foreach(&files, file, index)
		{
			char *stockFile = md5_makeHexHashFile(file);

			if(existFile(stockFile))
			{
				cout("* %s\n", file);
				errorCase(!NoCheckCollision && !isSameFile(file, stockFile)); // ? �R���W��������
			}
			else
			{
				/*
					stockFile �𒆓r���[�ɏo�͂��ăG���[�Ƃ��ŗ�����ƁA�n�b�V���ƍ���Ȃ��t�@�C�����o���Ă��܂��}�Y���̂ŁA
					�m���ɓǂݍ��߂� midFile �𒆌p����B
				*/
				char *midFile = makeTempPath("mid");

				cout("< %s\n", file);
				cout("+ %s\n", midFile);
				cout("> %s\n", stockFile);

				copyFile(file, midFile);
				moveFile(midFile, stockFile);

				memFree(midFile);
			}
			addElement(stockFiles, (uint)stockFile);
		}
		unaddCwd();
		execute("Compact.exe /C /S:" DIR_FILES); // �Ĉ��k

		changeRoots(paths, dir, NULL); // ���X�g�o�͗p�ɁA���Ή�

		addCwd(DIR_REVISIONS);
		createDir(targetStamp);
		addCwd(targetStamp);

		if(*comment)
			writeOneLine(FILE_REV_COMMENT, comment);

		writeLines(FILE_REV_TREE, &dirs);

		// make files file
		{
			FILE *fp = fileOpen(FILE_REV_FILES, "wt");

			foreach(&files, file, index)
			{
				writeToken(fp, getLine(stockFiles, index));
				writeChar(fp, '\x20');
				writeLine(fp, file);
			}
			fileClose(fp);
		}
		releaseDim(paths, 1);
		releaseDim(stockFiles, 1);

		unaddCwd();
		unaddCwd();
	}
	unaddCwd();

cancelled:
	memFree(storeDir);
	memFree(targetStamp);
	memFree(comment);
}

static int EditCommentMode;
static int TrimStoreDirMode;
static int FileHistoryMode;
static int FileHistoryMode_FromLastRevision;
static int FileHistoryMode_LastFileOnly;
static int RemoveLastRevIfNoModMode;
static int RemoveLastRevIfNoModMode_QuietMode;
static char *InputDirExt;

static void Rum(char *dir)
{
	execute("TITLE rum");

	dir = makeFullPath(dir);
	cout("�Ώ�: %s\n", dir);

	if(InputDirExt)
	{
		dir = changeExt_xc(dir, InputDirExt);
		cout("�·U: %s\n", dir);
	}
	errorCase(isAbsRootDir(dir));
	errorCase(!existDir(dir));

	if(!_stricmp(EXT_STOREDIR, getExt(dir)))
	{
		if(EditCommentMode)
		{
			EditComment(dir);
		}
		else if(TrimStoreDirMode)
		{
			TrimStoreDir(dir);
		}
		else if(FileHistoryMode)
		{
			FileHistory(dir, FileHistoryMode_FromLastRevision, FileHistoryMode_LastFileOnly);
		}
		else if(RemoveLastRevIfNoModMode)
		{
			RemoveLastRevIfNoMod(dir, RemoveLastRevIfNoModMode_QuietMode);
		}
		else
		{
			Checkout(dir);
		}
	}
	else
		Commit(dir);

	memFree(dir);
}
int main(int argc, char **argv)
{
readArgs:
	if(argIs("/E")) // Edit comment
	{
		EditCommentMode = 1;
		goto readArgs;
	}
	if(argIs("/F")) // without .exe and .obj (Factory mode)
	{
		WithoutExeObjMode = 1;
		goto readArgs;
	}
	if(argIs("/T")) // Trim store-dir
	{
		TrimStoreDirMode = 1;
		goto readArgs;
	}
	if(argIs("/HA")) // file History from All revision
	{
		FileHistoryMode = 1;
		goto readArgs;
	}
	if(argIs("/H")) // file History from last revision
	{
		FileHistoryMode = 1;
		FileHistoryMode_FromLastRevision = 1;
		goto readArgs;
	}
	if(argIs("/1A")) // last file from All revision
	{
		FileHistoryMode = 1;
		FileHistoryMode_LastFileOnly = 1;
		goto readArgs;
	}
	if(argIs("/1")) // last file from last revision
	{
		FileHistoryMode = 1;
		FileHistoryMode_FromLastRevision = 1;
		FileHistoryMode_LastFileOnly = 1;
		goto readArgs;
	}
	if(argIs("/R")) // Remove last revison if no modifications
	{
		RemoveLastRevIfNoModMode = 1;
		goto readArgs;
	}
	if(argIs("/RR")) // qrum �p
	{
		RemoveLastRevIfNoModMode = 1;
		InputDirExt = "rum";
		goto readArgs;
	}
	if(argIs("/RRR")) // qrum �p
	{
		RemoveLastRevIfNoModMode = 1;
		RemoveLastRevIfNoModMode_QuietMode = 1;
		InputDirExt = "rum";
		goto readArgs;
	}
	if(argIs("/Q")) // Quiet mode
	{
		QuietMode = 1;
		goto readArgs;
	}
	if(argIs("/D")) // restore free Dir mode
	{
		RestoreFreeDirMode = 1;
		goto readArgs;
	}
	if(argIs("/-COLL"))
	{
		NoCheckCollision = 1;
		goto readArgs;
	}
	Rum(hasArgs(1) ? nextArg() : c_dropDir());

	termination(ErrorLevel);
}
