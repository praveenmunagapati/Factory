#include "..\..\Dir2.h"

int main(int argc, char **argv)
{
	autoBlock_t *ret = DC_GetFilePart(getArg(0), toValue64(getArg(1)), toValue(getArg(2)));
	char *tmp;

	errorCase_m(!ret, "���s���܂����B");

	tmp = toPrintLine(ret, 1);

	cout("�擾�����T�C�Y = %u\n", getSize(ret));
	cout("�擾�����f�[�^ = [%s]\n", tmp);

	releaseAutoBlock(ret);
	memFree(tmp);
}
