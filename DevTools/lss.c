#include "C:\Factory\Common\all.h"

static int DirFileMode;
static int AddDirToFilesMode; // ? ディレクトリも Files.txt に追加する。

static void DispList(char *targetDir, autoList_t *searchPtns)
{
	autoList_t *foundFiles = newList();
	autoList_t *paths;
	char *path;
	uint index;

	     if(DirFileMode == 'D') paths = lssDirs(targetDir);
	else if(DirFileMode == 'F') paths = lssFiles(targetDir);
	else                        paths = lss(targetDir);

	sortJLinesICase(paths);

	foreach(paths, path, index)
	{
		int found;

		if(getCount(searchPtns))
		{
			char *path_a = xcout("%s*", path);
			char *searchPtn;
			uint ndx;

			found = 0;

			foreach(searchPtns, searchPtn, ndx)
			{
				if(mbs_stristr(path_a, searchPtn))
				{
					found = 1;
					break;
				}
			}
			memFree(path_a);
		}
		else
			found = 1;

		if(found)
		{
			cout("%s\n", path);

			if(AddDirToFilesMode || existFile(path))
			{
				addElement(foundFiles, (uint)strx(path));
			}
		}
	}
	if(getCount(foundFiles))
	{
		writeLines(FOUNDLISTFILE, foundFiles);
	}
	releaseDim(foundFiles, 1);
	releaseDim(paths, 1);
}
int main(int argc, char **argv)
{
	antiSubversion = 1;

readArgs:
	if(argIs("/D"))
	{
		DirFileMode = 'D';
		goto readArgs;
	}
	if(argIs("/F"))
	{
		DirFileMode = 'F';
		goto readArgs;
	}
	if(argIs("/SVN"))
	{
		antiSubversion = 0;
		goto readArgs;
	}
	if(argIs("/D+"))
	{
		AddDirToFilesMode = 1;
		goto readArgs;
	}

	if(hasArgs(2))
	{
		DispList(getArg(0), getFollowArgs(1));
	}
	else
	{
		DispList(".", allArgs());
	}
}
