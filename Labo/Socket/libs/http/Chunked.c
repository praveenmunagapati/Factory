#include "Chunked.h"

#define RECVSIZE_ONCE_MAX (1024 * 1024 * 16)

httpChunkedRecver_t *httpCreateChunkedRecver(SockStream_t *ss)
{
	httpChunkedRecver_t *i = (httpChunkedRecver_t *)memAlloc(sizeof(httpChunkedRecver_t));

	i->Stream = ss;
	i->RemSize = 0;

	return i;
}
void httpReleaseChunkedRecver(httpChunkedRecver_t *i)
{
	memFree(i);
}
autoBlock_t *httpRecvChunked(httpChunkedRecver_t *i) // ret == NULL: 終了
{
	uint rSize;
	void *rBlock;

	errorCase(!i);
	errorCase(!i->Stream); // ? 既に終了

	if(!i->RemSize)
	{
		char *line = SockRecvLine(i->Stream, 30); // 最大８文字だけど、改行とかあるしギリギリにする必要もないかなと。

		strchrEnd(line, ';')[0] = '\0'; // chunk-extension の排除

		i->RemSize = toValueDigits(line, hexadecimal);
		memFree(line);

		if(!i->RemSize)
		{
			i->Stream = NULL;
			return NULL;
		}
	}
	rSize = m_min(RECVSIZE_ONCE_MAX, i->RemSize);
	rBlock = memAlloc(rSize);

	SockRecvBlock(i->Stream, rBlock, rSize);
	i->RemSize -= rSize;

	if(!i->RemSize)
	{
		// CR-LF
		SockRecvChar(i->Stream);
		SockRecvChar(i->Stream);
	}
	return bindBlock(rBlock, rSize);
}
