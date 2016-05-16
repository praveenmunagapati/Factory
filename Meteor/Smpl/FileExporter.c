/*
	FileExporter.exe /E ���̓f�B���N�g�� [�o�̓f�B���N�g��]
	FileExporter.exe /I ���̓f�B���N�g��
*/

#include "C:\Factory\Common\all.h"
#include "..\FileExporter.h"

int main(int argc, char **argv)
{
	if(argIs("/E"))
	{
		char *rDir;
		char *wDir;

		rDir = nextArg();
		wDir = hasArgs(1) ? nextArg() : makeFreeDir();

		rDir = makeFullPath(rDir);
		wDir = makeFullPath(wDir);

		cout("< %s\n", rDir);
		cout("> %s\n", wDir);

		if(existDir(wDir) && lsCount(wDir))
		{
			cout("�o�͐�f�B���N�g���͋�ł͂���܂���B\n");
			cout("��ɂ���H\n");

			if(getKey() == 0x1b)
				termination(0);

			cout("��ɂ��܂��B\n");
		}
		recurRemoveDirIfExist(wDir);

		errorCase_m(!FileExporter(rDir, wDir), "�G�N�X�|�[�g���s");

		memFree(rDir);
		memFree(wDir);
		return;
	}
	if(argIs("/I"))
	{
		char *rDir;

		rDir = makeFullPath(nextArg());
		cout("< %s\n", rDir);

		errorCase_m(!FileImporter(rDir), "�C���|�[�g���s");

		memFree(rDir);
		return;
	}
}
