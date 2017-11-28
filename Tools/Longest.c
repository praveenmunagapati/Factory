/*
	Longest.exe [/S �ŒZ�̉���] [���̓f�B���N�g�� | ���̓t�@�C��]

	���̓f�B���N�g���̔z���̂ɂ���u�Œ��E�ŒZ�̃t���p�X�v�����t�@�C���E�f�B���N�g����T���A���ꂼ��̒�����\�����܂��B
	�ŒZ�̉������w�肵�Ȃ���΁A�ŒZ�̃t���p�X�͓��̓f�B���N�g�����̂��̂ɂȂ�܂��B
	���̓t�@�C���̏ꍇ�A�Œ��̍s��T���A���ꂼ��̒�����\�����܂��B
*/

#include "C:\Factory\Common\all.h"

static uint ShortestMin = 0;

static void DispLongest(char *path)
{
	autoList_t *lines;
	char *line;
	uint index;
	uint minlen;
	uint maxlen;

	if(existDir(path))
	{
		lines = lss(path);
		insertElement(lines, 0, (uint)makeFullPath(path));
	}
	else
		lines = readLines(path);

	minlen = UINTMAX;
	maxlen = 0;

	foreach(lines, line, index)
	{
		uint clen = strlen(line);

		if(clen < minlen && ShortestMin <= clen)
		{
			minlen = clen;
			cout("< %u:%s\n", minlen, line);
		}
		if(maxlen < clen)
		{
			maxlen = clen;
			cout("> %u:%s\n", maxlen, line);
		}
	}
	releaseDim(lines, 1);

	cout("----\n");
	cout("%u\n", minlen);
	cout("%u\n", maxlen);
}
int main(int argc, char **argv)
{
	if(argIs("/S"))
	{
		ShortestMin = toValue(nextArg());
		cout("Shortest-Min: %u\n", ShortestMin);
	}

	if(hasArgs(1))
	{
		DispLongest(nextArg());
		return;
	}
	for(; ; )
	{
		char *path = dropPath();

		if(!path)
			break;

		DispLongest(path);
		cout("\n");

		memFree(path);
	}
}
