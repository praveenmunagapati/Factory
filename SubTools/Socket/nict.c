/*
	��NTP�ŃV�X�e���������킹�B
*/

#include "C:\Factory\Common\Options\SClient.h"
#include "libs\ApplyStampData.h"

#define NICT_DOMAIN "ntp-a1.nict.go.jp"
//#define NICT_DOMAIN "ntp-b1.nict.go.jp" // �\���̎I�H
#define NICT_PORT 80

static time_t NictTime;

static int NictPerform(int sock, uint prm_dummy)
{
	SockStream_t *ss = CreateSockStream(sock, 5);
	char *line;
	int ret = 0;

	SockSendLine_NF(ss, "GET /cgi-bin/jst HTTP/1.1");
	SockSendLine_NF(ss, "Host: " NICT_DOMAIN);
	SockSendLine(ss, "");

	for(; ; ) // �w�b�_�ǂݔ�΂�
	{
		char *line = SockRecvLine(ss, 2000);

		if(!*line)
		{
			memFree(line);
			break;
		}
		memFree(line);
	}

	memFree(SockRecvLine(ss, 100)); // <HTML>
	memFree(SockRecvLine(ss, 100)); // <HEAD><TITLE> ...
	memFree(SockRecvLine(ss, 100)); // <BODY>

	line = SockRecvLine(ss, 100);

	line2JLine(line, 1, 0, 0, 1); // �\���̂���
	cout("[%s]\n", line);

	if(
		lineExp("<1,,09>", line) ||
		lineExp("<1,,09>.<1,,09>", line)
		)
	{
		*strchrEnd(line, '.') = '\0'; // �����_�ȉ�������

		NictTime = toValue64(line);

		if(m_isRange(NictTime, 1000000000ui64, 32500000000ui64)) // ? ���悻 2000�N �` 3000�N, ����ȊO�͉������������I
			ret = 1;
	}
	ReleaseSockStream(ss);
	memFree(line);
	return ret;
}
static int GetNictTime(void) // ret: ? ����
{
	int ret;

	// fixme: �b�P�ʂŉ������x���Ȃ�΁A���ꂾ���s���m�ȓ����ɂȂ��Ȃ�...
	// -- Slew�œ����Z�b�g���Ă���̂Ŏ��X�����������Ƃ������Ă�������...

	LOGPOS();
	ret = SClient(NICT_DOMAIN, NICT_PORT, NictPerform, 0);
	LOGPOS();

	return ret;
}
int main(int argc, char **argv)
{
	int viewOnly = 0;
	int dayChangeEvasion = 1;

readArgs:
	if(argIs("/V"))
	{
		viewOnly = 1;
		goto readArgs;
	}
	if(argIs("/-E"))
	{
		dayChangeEvasion = 0;
		goto readArgs;
	}

	// ----

	LOGPOS();

	// ss.000 �b�ɂȂ�ׂ��߂Â��Ă݂�B
	{
		time_t t = time(NULL);

		do
		{
			sleep(10);
		}
		while(t == time(NULL));
	}

	LOGPOS();

	if(dayChangeEvasion)
	{
		uint hms = (uint)(toValue64(c_makeCompactStamp(NULL)) % 1000000ui64);

		cout("hms: %06u\n", hms);

		// �A�N�e�B�u�I�[�v���ɍŒ� 20 sec, �ʐM�^�C���A�E�g 5 sec �ŁA�������� 25 �b + 2�`3 �b���炢�ōX�V���|������͂��B
		// �]�T�� 35 �b���炢�ł����Ǝv���B
		// -- 1 ���ł�����B

//		if(hms < 5 || 235925 < hms)
		if(hms < 5 || 235900 < hms)
		{
			cout("���t�ύX���߂��̂ŁA���~���܂��B\n");
			return;
		}
	}
	if(!GetNictTime())
	{
		cout("�擾���s\n");
		return;
	}
	cout("�擾���������F%s\n", c_makeJStamp(getStampDataTime(NictTime), 0));

	if(!viewOnly)
	{
		LOGPOS();
		SlewApplyTimeData(NictTime);
		LOGPOS();
	}
}
