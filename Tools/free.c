#include "C:\Factory\Common\all.h"

int main(int argc, char **argv)
{
	updateMemory();

	cout("�������g�p�� = %u %%\n", lastMemoryLoad);
	cout("������������ = %I64u �o�C�g\n", lastMemoryFree);
	cout("�����������e�� = %I64u �o�C�g\n", lastMemorySize);
	cout("���z�������� = %I64u �o�C�g\n", lastVirtualFree);
	cout("���z�������g�� = %I64u �o�C�g\n", lastExVirtualFree);
	cout("���z�������e�� = %I64u �o�C�g\n", lastVirtualSize);
	cout("�y�[�W�T�C�Y�� = %I64u �o�C�g\n", lastPageFileFree);
	cout("�y�[�W�T�C�Y�ő� = %I64u �o�C�g\n", lastPageFileSize);
}
