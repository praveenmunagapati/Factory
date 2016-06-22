#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Calc2.h"

static char *GetPct(uint64 numer, uint64 denom)
{
	calcBasement = 2;
	return calc_xx(xcout("%I64u00", numer), '/', xcout("%I64u", denom));
}
static void DispDiskFree(int drive)
{
	updateDiskSpace(drive);

	cout("%c�h���C�u\n", m_toupper(drive));
	cout("�f�B�X�N�g�p�� = %.3f %%\n", 100.0 - lastDiskFree / (double)lastDiskSize * 100.0);
	cout("�f�B�X�N�� = %I64u �o�C�g (�g�p�\ = %I64u �o�C�g, %s %%)\n", lastDiskFree, lastDiskFree_User, GetPct(lastDiskFree_User, lastDiskFree));
	cout("�f�B�X�N�e�� = %I64u �o�C�g\n", lastDiskSize);
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
