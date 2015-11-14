/*
	ceruleanIndex.exe INDEX-DIR TEMPLATE-DIR
*/

#include "C:\Factory\Common\all.h"

#define TEMPLATE_INDEX_FILE      "tamplate_index.html.txt"
#define TEMPLATE_GAME_FILE       "tamplate_game.html.txt"
#define TEMPLATE_VER_FILE        "tamplate_ver.html.txt"
#define TEMPLATE_VER_NEWEST_FILE "tamplate_ver_newest.html.txt"

#define INDEX_FILE "index.html"
#define ROOT_LDIR "charlotte"
#define ORDER_FILE "order.txt"
#define TITLE_FILE "title.txt"

static void MakeIndex(char *indexDir, char *templateDir)
{
	char *indexFile;
	char *rootDir;
	char *orderFile;
	autoList_t *names;
	char *name;
	uint name_index;
	char *gameListBuff;
	char *templateIndex;
	char *templateGame;
	char *templateVer;
	char *templateVerNewest;

	LOGPOS();

	errorCase(m_isEmpty(indexDir));
	errorCase(m_isEmpty(templateDir));

	// gomi >

	indexDir    = makeFullPath(indexDir);
	templateDir = makeFullPath(templateDir);

	errorCase(!existDir(indexDir));
	errorCase(!existDir(templateDir));

	indexFile = combine(indexDir, INDEX_FILE);
	rootDir   = combine(indexDir, ROOT_LDIR);
	orderFile = combine(rootDir, ORDER_FILE);

	errorCase(!existFile(indexFile)); // ���S�̂��߁A����ł����Ă���̃t�@�C��������Ă������ƁI
	errorCase(!existDir(rootDir));
	errorCase(!existFile(orderFile));

	names = readResourceLines(orderFile);
	gameListBuff = strx("");

	addCwd(templateDir);

	templateIndex     = readText(TEMPLATE_INDEX_FILE);
	templateGame      = readText(TEMPLATE_GAME_FILE);
	templateVer       = readText(TEMPLATE_VER_FILE);
	templateVerNewest = readText(TEMPLATE_VER_NEWEST_FILE);

	unaddCwd();

	// < gomi

	LOGPOS();

	foreach(names, name, name_index)
	{
		char *gameDir = combine(rootDir, name);
		char *titleFile;
		char *title;
		autoList_t *files;
		char *file;
		uint index;
		char *gameVerListBuff = strx("");
		int newestPassed = 0;

		LOGPOS();

		titleFile = combine(gameDir, TITLE_FILE);

		errorCase(!existDir(gameDir));
		errorCase(!existFile(titleFile));

		title = readFirstLine(titleFile);
		files = lsFiles(gameDir);
		sortJLinesICase(files);
		reverseElements(files); // �V������

		LOGPOS();

		foreach(files, file, index)
		{
			LOGPOS();

			if(lineExpICase("<>_v<3,09>.zip", file)) // ? Ver_�t�@�C��
			{
				char *ver = strchr(file, '\0') - 7;
				char *verLink;
				char *verTitle;
				char *tmp;

				LOGPOS();

				verLink = changeRoot(strx(file), indexDir, NULL);
				escapeYen(verLink);
				verTitle = xcout("%s v%c.%c%c", title, ver[0], ver[1], ver[2]);

				if(newestPassed)
					tmp = strx(templateVer);
				else
					tmp = strx(templateVerNewest);

				tmp = replaceLine(tmp, "*game-ver-link*", verLink, 0);
				tmp = replaceLine(tmp, "*game-ver-title*", verTitle, 0);

				gameVerListBuff = addLine(gameVerListBuff, tmp);

				memFree(verLink);
				memFree(verTitle);
				memFree(tmp);

				newestPassed = 1;

				LOGPOS();
			}
			LOGPOS();
		}
		LOGPOS();

		{
			char *tmp = strx(templateGame);

			tmp = replaceLine(tmp, "*game-title*", title, 0);
			tmp = replaceLine(tmp, "*game-ver-list*", gameVerListBuff, 0);

			gameListBuff = addLine(gameListBuff, tmp);

			memFree(tmp);
		}
		LOGPOS();

		memFree(gameDir);
		memFree(titleFile);
		memFree(title);
		releaseDim(files, 1);
		memFree(gameVerListBuff);

		LOGPOS();
	}
	LOGPOS();

	{
		char *tmp = strx(templateIndex);

		tmp = replaceLine(tmp, "*game-list*", gameListBuff, 0);

		writeOneLineNoRet(indexFile, tmp);

		memFree(tmp);
	}
	LOGPOS();
}
int main(int argc, char **argv)
{
	MakeIndex(getArg(0), getArg(1));
}
