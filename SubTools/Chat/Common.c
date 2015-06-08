#include "Common.h"

int UTP_DownloadMode;
int UTP_HtmlMode;
int UTP_EndSlash;

char *URLToPath(char *url) // ret: NULL == �s���ȃp�X, �\���\
{
	autoList_t *tokens;
	char *token;
	uint index;
	autoList_t *pathTkns;
	char *path;

	url = urlDecoder(url);

	if(startsWith(url, "http://")) // �X�L�[���E�h���C���E�|�[�g�ԍ� '/' ����
	{
		char *p = strchr(url + 7, '/');

		if(p)
			copyLine(url, p + 1);
	}
	else
		copyLine(url, url + 1); // �擪�� '/' ����

	if(endsWithICase(url, "?mode=download"))
	{
		UTP_DownloadMode = 1;
		strchr(url, '\0')[-14] = '\0';
	}
	else
		UTP_DownloadMode = 0;

	if(endsWithICase(url, "?mode=html"))
	{
		UTP_HtmlMode = 1;
		strchr(url, '\0')[-10] = '\0';
	}
	else
		UTP_HtmlMode = 0;

	if(endsWith(url, "/"))
	{
		UTP_EndSlash = 1;
		strchr(url, '\0')[-1] = '\0';
	}
	else
		UTP_EndSlash = 0;

	escapeYen(url);

	if(startsWith(url, "//")) // ? �l�b�g���[�N�h���C�u
	{
		url[0] = '_'; // dummy
	}
	tokens = tokenize(url, '/');
	memFree(url);

	foreach(tokens, token, index)
	{
		token = lineToFairLocalPath_x(token, 0);

		if(!index) // ? �擪�E�h���C�u��
		{
			if(lineExp("<1,AZaz><1,>", token)) // ���[�J���f�B�X�N
			{
				token[1] = ':';
//				token[2] = '\0';
			}
			else // �l�b�g���[�N
			{
				token[0] = '\\';
				token[1] = '\0';
			}
		}
		setElement(tokens, index, (uint)token);
	}
	if(getLine(tokens, 0)[0] != '\\') // ���[�J���f�B�X�N
	{
		if(getCount(tokens) == 1) // ���[�g�f�B���N�g��
		{
			addElement(tokens, (uint)strx(""));
		}
	}
	else // �l�b�g���[�N
	{
		if(getCount(tokens) < 3) // �l�b�g���[�N�p�X�ł͂Ȃ��B
		{
			releaseDim(tokens, 1);
			return NULL;
		}
	}
	path = untokenize(tokens, "\\");
	releaseDim(tokens, 1);
	return path;
}
char *PathToURL(char *path)
{
	error(); // todo
	return NULL;
}
