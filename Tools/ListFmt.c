#include "C:\Factory\Common\all.h"

#define S_ESCAPE "\1"

static autoList_t *ListFiles;
static int XMode; // 0-2: �����s���m, �S�Ă̑g�ݍ��킹(�d������), �S�Ă̑g�ݍ��킹(�d���A��)
static char *Format;

static void ListFmt(void)
{
	autoList_t *lists = newList();
	autoList_t *list;
	char *file;
	uint index;

	errorCase(!getCount(ListFiles));

	foreach(ListFiles, file, index)
		addElement(lists, (uint)readLines(file));

	foreach(lists, list, index)
		errorCase(!getCount(list));

	for(; ; )
	{



	}
}
int main(int argc, char **argv)
{
	ListFiles = newList();

readArgs:
	if(argIs("/F"))
	{
		addElement(ListFiles, (uint)nextArg());
		goto readArgs;
	}
	if(argIs("/X-C"))
	{
		XMode = 1;
		goto readArgs;
	}
	if(argIs("/X"))
	{
		XMode = 2;
		goto readArgs;
	}
	argIs("/-");

	Format = untokenize(allArgs(), " "); // gomi
	ListFmt();
}
