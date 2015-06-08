/*
	Service.dat に以下を追加

		X-BIND C:\Factory\SubTools\HTT\Bind.exe /S

	ロック

		Bind.exe /L DOMAIN PORT-NO [/CEX CONNECT-ERROR-MAX]

			CONNECT-ERROR-MAX ...

				接続失敗回数がこれに達すると中断 (キャンセルと同じ) する。
				サーバーが停止していることを考慮するときのため。
				デフォルト == UINTMAX (無効)

	ロック解除

		Bind.exe /U

	- - -

	★同じPCで実行してね。
*/

#include "C:\Factory\Common\Options\SockClient.h"
#include "C:\Factory\Common\Options\Progress.h"

#define MUTEX_UUID "{c4049337-18db-4a1d-84ab-eb0a82310c85}"
#define LOCKED_EVENT_UUID "{57349467-d90b-4e77-804e-516c4d07c0d6}"
#define UNLOCK_EVENT_UUID "{715811de-35ff-4d11-928f-e94f1690ab23}"

#define LOCK_FLAG_FILE "C:\\Factory\\tmp\\Bind_Lock.flg"

static char *Domain;
static uint PortNo;
static uint ConnectErrorMax = UINTMAX;
static uint ConnectErrorCount;

static autoBlock_t *GetHTTRequestMessage(void)
{
	return ab_makeBlockLine("X-BIND\x20");
}
static void ThrowHTTRequest(void)
{
	static uchar ip[4];
	static int sock = -1;
	autoBlock_t *message;

	if(sock != -1)
	{
		LOGPOS();
		sockDisconnect(sock);
	}
	LOGPOS();
	sock = sockConnect(ip, Domain, PortNo);
	cout("sock: %d\n", sock);

	if(sock == -1)
	{
		ConnectErrorCount++;
		return;
	}
	LOGPOS();
	message = GetHTTRequestMessage();
	SockSendSequLoop(sock, message, 2000);
	releaseAutoBlock(message);
	LOGPOS();
}

static void DoWakeup(uint hdl)
{
	LOGPOS();
	eventSet(hdl);
	LOGPOS();
}
static void DoWait(uint hdl, void (*interrupt)(void))
{
	int cancelled = 0;

	LOGPOS();
	ProgressBegin();

	for(; ; )
	{
		Progress();
		interrupt();

		if(ConnectErrorMax != UINTMAX && ConnectErrorMax <= ConnectErrorCount)
		{
			cout("CONNECT-ERROR-MAX\n");
			cancelled = 1;
			break;
		}
		if(handleWaitForMillis(hdl, 2000))
			break;

		while(hasKey())
			if(getKey() == 0x1b)
				cancelled = 1;

		if(cancelled)
			break;
	}
	ProgressEnd(cancelled);
	LOGPOS();
}

int main(int argc, char **argv)
{
	int hdlMtx = mutexOpen(MUTEX_UUID);
	int hdlLocked = eventOpen(LOCKED_EVENT_UUID);
	int hdlUnlock = eventOpen(UNLOCK_EVENT_UUID);

	if(argIs("/S")) // Service
	{
//		LOGPOS();
		handleWaitForever(hdlMtx);
		{
			if(!existFile(LOCK_FLAG_FILE))
			{
				mutexRelease(hdlMtx);
//				LOGPOS();
				goto endProc;
			}
			removeFile(LOCK_FLAG_FILE);
		}
		mutexRelease(hdlMtx);
		LOGPOS();

		collectEvents(hdlUnlock, 0); // ゴミイベント回収

		DoWakeup(hdlLocked);
		DoWait(hdlUnlock, noop);
	}
	else if(argIs("/L")) // Lock
	{
		Domain = nextArg();
		PortNo = toValue(nextArg());

		if(argIs("/CEX"))
			ConnectErrorMax = toValue(nextArg());

		errorCase(hasArgs(1)); // 不明なオプション
		errorCase(m_isEmpty(Domain));
		errorCase(!PortNo || 0xffff < PortNo);

		LOGPOS();
		handleWaitForever(hdlMtx);
		{
			createFile(LOCK_FLAG_FILE);
		}
		mutexRelease(hdlMtx);
		LOGPOS();

		SockStartup();
		DoWait(hdlLocked, ThrowHTTRequest);
		SockCleanup();
	}
	else if(argIs("/U")) // Unlock
	{
		DoWakeup(hdlUnlock);
	}

endProc:
	handleClose(hdlMtx);
	handleClose(hdlLocked);
	handleClose(hdlUnlock);
}
