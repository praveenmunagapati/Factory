#include "..\..\all.h"

int main(int argc, char **argv)
{
	autoList_t *catalog = DC_GetCatalog(nextArg());
	char *line;
	uint index;

	errorCase_m(!catalog, "�J�^���O�̎擾�Ɏ��s���܂����B");

	foreach(catalog, line, index)
		cout("%s\n", line);

	releaseDim(catalog, 1);
}
