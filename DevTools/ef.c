#include "C:\Factory\Common\all.h"

int main(int argc, char **argv)
{
	autoList_t *files;
	autoList_t *selfiles;

	createFileIfNotExist(FOUNDLISTFILE);
	files = readLines(FOUNDLISTFILE);
	selfiles = selectLines(files);

	if(getCount(selfiles))
	{
		if(isSameLines(files, selfiles, 0))
		{
			cout("�X�V�s�v\n");
		}
		else
		{
			writeLines(FOUNDLISTFILE, selfiles);
			cout("�X�VOK\n");
		}
	}
	releaseDim(files, 1);
	releaseDim(selfiles, 1);
}
