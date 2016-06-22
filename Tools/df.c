#include "C:\Factory\Common\all.h"

static void DispDiskFree(int drive)
{
	updateDiskSpace(drive);

	cout("%c�h���C�u\n", m_toupper(drive));
	cout("�f�B�X�N�g�p�� = %.3f %%\n", (lastDiskSize - lastDiskFree) * 100.0 / lastDiskSize);
	cout("�f�B�X�N�� = %I64u �o�C�g\n", lastDiskFree);
	cout("�f�B�X�N�e�� = %I64u �o�C�g\n", lastDiskSize);
	cout("���p�\�̈� = %I64u �o�C�g\n", lastDiskFree_User);
	cout("���p�s�̈� = %I64u �o�C�g\n", lastDiskFree - lastDiskFree_User);
}
int main(int argc, char **argv)
{
	if(argIs("*"))
	{
		char drive[] = "_:\\";

		for(drive[0] = 'A'; drive[0] <= 'Z'; drive[0]++)
			if(existDir(drive))
				DispDiskFree(drive[0]);

		return;
	}
	DispDiskFree((hasArgs(1) ? nextArg() : c_getCwd())[0]);
}
