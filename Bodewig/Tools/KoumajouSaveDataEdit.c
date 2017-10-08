/*
	KoumajouSaveDataEdit.exe [/D �Q�[��DIR] [/S �X�e�[�W�ԍ�] [/H �n�C�X�R�A] [/E+ | /E-]

		�X�e�[�W�ԍ� ... 1 �` 8         --  �͈͊O�͍ŏ�����ɂȂ���ۂ��B
		�n�C�X�R�A   ... 0 �` 99999999  --  �͈͊O�͏�ʌ����̂Ă�����ۂ��B
		/E+          ... �G�N�X�g���J��
		/E-          ... �G�N�X�g�����J��

	- - -
	��

		KoumajouSaveDataEdit.exe /D C:\game\koumajou /S 8 /E+

		KoumajouSaveDataEdit.exe /H 13413983

	- - -

	ver.1.04a �Ō��؂����B
*/

#include "C:\Factory\Common\all.h"

#define S_MASK "055b2d160639034d010b"
#define PREFIX_SIGNATURE "This file is save data of Scarlet Symphony.\r\n"
#define SUFFIX_EXTRA_OPENED "\r\nextramode_enable"

static char *GameDir = "C:\\etc\\Game\\�g����`��";

static uint StageNo = 1;
static uint HiScore = 0;
static int ExtraOpened = 0;

static char *GetSaveDataFile(void)
{
	return combine(GameDir, "data\\savedata.dat");
}
static void DoMask(autoBlock_t *fileData)
{
	autoBlock_t *mask = makeBlockHexLine(S_MASK);
	uint index;

	for(index = 0; index < getSize(fileData); index++)
	{
		b(fileData)[index] ^= b(mask)[index % getSize(mask)];
	}
	releaseAutoBlock(mask);
}
static void ShowSaveData(void)
{
	cout("StageNo: %u\n", StageNo);
	cout("HiScore: %u\n", HiScore);
	cout("ExtraOpened: %d\n", m_01(ExtraOpened));
}
static void LoadSaveData(void)
{
	char *file = GetSaveDataFile();

	if(existFile(file))
	{
		autoBlock_t *fileData = readBinary(file);
		char *fileText;

		DoMask(fileData);
		fileText = unbindBlock2Line(fileData);
		toknext(fileText, "\n");
		StageNo = toValue(toknext(NULL, ","));
		HiScore = toValue(toknext(NULL, "\r"));
		ExtraOpened = (int)toknext(NULL, "");
		memFree(fileText);
	}
	memFree(file);
}
static void OutputSaveData(void)
{
	char *fileText = strx(PREFIX_SIGNATURE);
	autoBlock_t *fileData;

	fileText = addLine_x(fileText, xcout("%u,%u", StageNo, HiScore));

	if(ExtraOpened)
		fileText = addLine(fileText, SUFFIX_EXTRA_OPENED);

	fileData = ab_makeBlockLine_x(fileText);
	DoMask(fileData);
	writeBinary_xx(GetSaveDataFile(), fileData);
}
int main(int argc, char **argv)
{
	int changed = 0;

	if(argIs("/D"))
	{
		GameDir = nextArg();
		goto readArgs;
	}

	// Check 'GameDir'
	{
		char *file;

		errorCase_m(!existDir(GameDir), "Wrong Game-Dir");

		file = combine(GameDir, "koumajou.exe");
		errorCase_m(!existFile(file), "Wrong Game-Dir, koumajou.exe does not exist!");
		memFree(file);

		file = combine(GameDir, "data");
		errorCase_m(!existDir(file), "Wrong Game-Dir, data does not exist!");
		memFree(file);
	}

	LoadSaveData();
	ShowSaveData();

readArgs:
	if(argIs("/S"))
	{
		StageNo = toValue(nextArg());
		changed = 1;
		goto readArgs;
	}
	if(argIs("/H"))
	{
		HiScore = toValue(nextArg());
		changed = 1;
		goto readArgs;
	}
	if(argIs("/E+"))
	{
		ExtraOpened = 1;
		changed = 1;
		goto readArgs;
	}
	if(argIs("/E-"))
	{
		ExtraOpened = 0;
		changed = 1;
		goto readArgs;
	}

	if(changed)
	{
		cout(">\n");

		ShowSaveData();
		OutputSaveData();
	}
}
