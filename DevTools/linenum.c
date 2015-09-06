#include "C:\Factory\Common\all.h"

static int SkipLine(FILE *fp)
{
	int chr = readChar(fp);

	if(chr == EOF) // ? 0������EOF�ŏI���s <- �s�ƌ��Ȃ��Ȃ��B
		return 0;

	while(chr != '\n') // ? ! ���s
	{
		chr = readChar(fp);

		if(chr == EOF) // ? 1�����ȏ��EOF�ŏI���s <- �s�ƌ��Ȃ��B
			break;
	}
	return 1;
}
static void LineMid(char *file, uint64 minLineNo, uint64 maxLineNo)
{
	FILE *fp = fileOpen(file, "rb");
	uint64 lineNo = 1;

	errorCase(minLineNo < 1);
	errorCase(maxLineNo < minLineNo);

	while(lineNo < minLineNo)
	{
		errorCase(!SkipLine(fp)); // ? �J�n�s���O�Ƀt�@�C�����I�������B
		lineNo++;
	}
	while(lineNo <= maxLineNo)
	{
		char *line = readLine(fp);

		errorCase(!line); // ? �I���s���O�Ƀt�@�C�����I�������B

		cout("%s\n", line);
		memFree(line);
		lineNo++;
	}
	fileClose(fp);
}
static void LineNumMain(char *file)
{
	FILE *fp = file ? fileOpen(file, "rb") : (stdin_set_bin(), stdin);
	uint64 linenum = 0;

	while(SkipLine(fp))
	{
		linenum++;
	}
	cout("%I64u\n", linenum);

	if(file)
		fileClose(fp);
}
int main(int argc, char **argv)
{
	if(argIs("/M"))
	{
		uint64 minLineNo;
		uint64 maxLineNo;
		char *file;

		minLineNo = toValue64(nextArg());
		maxLineNo = toValue64(nextArg());
		file = nextArg();

		LineMid(file, minLineNo, maxLineNo);
		return;
	}
	if(hasArgs(1))
		LineNumMain(nextArg());
	else
		LineNumMain(NULL);
}
