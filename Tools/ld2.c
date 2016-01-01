#include "C:\Factory\Common\all.h"
#include "libs\SaveDir.h"

#define BATCH_FILE "C:\\Factory\\tmp\\LoadDir.bat"

static void ShowList(void)
{
	autoList_t *lines = readLines(SAVE_FILE);
	char *line;
	uint index;

	for(index = 0; index < getCount(lines); index += 2)
		cout("%s %s\n", getLine(lines, index), getLine(lines, index + 1));

	releaseDim(lines, 1);
}
static void LoadDir(char *name)
{
	autoList_t *lines = readLines(SAVE_FILE);
	uint index;

	for(index = 0; index < getCount(lines); index += 2)
		if(!_stricmp(name, getLine(lines, index)))
			break;

	if(index < getCount(lines))
	{
		writeOneLine_cx(BATCH_FILE, xcout("CD \"%s\"", getLine(lines, index + 1)));
	}
	else
	{
		ShowList();
	}
	releaseDim(lines, 1);
}
int main(int argc, char **argv)
{
	createFileIfNotExist(SAVE_FILE);
	removeFileIfExist(BATCH_FILE);

	if(hasArgs(1))
	{
		LoadDir(nextArg());
	}
	else
	{
		ShowList();
	}
}
