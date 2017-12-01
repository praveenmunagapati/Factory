/*
	FileCharCount.exe (���� | �����R�[�h��16�i��2��) [���̓t�@�C��]

	���s��
		FileCharCount.exe 0a 123.txt
			123.txt ���̉��s�̐��𐔂���B

	0d == CR
	0a == LF
*/

#include "C:\Factory\Common\all.h"

static int TargetChr;

static void FileCharCount(char *file)
{
	FILE *fp = fileOpen(file, "rb");
	uint64 count = 0;

	for(; ; )
	{
		int chr = readChar(fp);

		if(chr == EOF)
			break;

		if(chr == TargetChr)
			count++;
	}
	fileClose(fp);

	cout("%I64u\n", count);
}
int main(int argc, char **argv)
{
	char *sCode = nextArg();

	if(!_stricmp(sCode, "LF"))
	{
		TargetChr = '\n';
	}
	else if(strlen(sCode) == 1)
	{
		TargetChr = sCode[0];
	}
	else if(strlen(sCode) == 2)
	{
		errorCase(!lineExp("<2,09AFaf>", sCode));

		TargetChr = toValueDigits(sCode, hexadecimal);
	}
	else
		error();

	if(hasArgs(1))
	{
		FileCharCount(nextArg());
	}
	else
	{
		for(; ; )
		{
			FileCharCount(c_dropFile());
			cout("\n");
		}
	}
}
