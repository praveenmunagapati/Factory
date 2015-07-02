#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Prime2.h"

#define PRIME_MAX 18446744073709551557ui64 // uint64 �ő�̑f��

#define DUMMY_UUID "{905cac95-9d4c-482d-a189-e6a941c68fbe}"

static void WrUI64(FILE *fp, uint64 value)
{
	static char buff[] = "12345678901234567890\n";
	char *p = buff + 20;
	uint s;

	do
	{
		p--;
		*p = '0' + (value % 10);
		value /= 10;
	}
	while(value != 0);

	s = ((uint)buff + 21) - (uint)p;

	errorCase(fwrite(p, 1, s, fp) != s);
}
static uint64 GetLowPrime(uint64 value)
{
	while(2 < value)
	{
		value--;

		if(IsPrime(value))
			return value;
	}
	return 0;
}
static uint64 GetHiPrime(uint64 value)
{
	while(value < PRIME_MAX)
	{
		value++;

		if(IsPrime(value))
			return value;
	}
	return 0;
}
static void PrimeRange(uint64 minval, uint64 maxval, char *outFile, char *cancelEvName, char *reportEvName, char *reportMtxName, char *reportFile)
{
	FILE *fp = fileOpen(outFile, "wb");
	uint cancelEv = eventOpen(cancelEvName);
	uint reportEv = eventOpen(reportEvName);
	uint reportMtx = mutexOpen(reportMtxName);
	uint64 count;

	errorCase(setvbuf(fp, NULL, _IOFBF, 64 * 1024 * 1024)); // ? ���s

	if(minval <= 2)
	{
		if(2 <= maxval)
			errorCase(fprintf(fp, "2\n") < 0);

		minval = 3;
	}
	else
		minval |= 1;

	m_minim(maxval, PRIME_MAX);

	for(count = minval; count <= maxval; count += 2)
	{
		if(count % 0x08000000 == 1)
		{
			if(handleWaitForMillis(cancelEv, 0))
			{
				errorCase(fprintf(fp, "���~���܂����B\n") < 0);
				break;
			}

			handleWaitForever(reportMtx);
			writeOneLine_cx(reportFile, xcout("%I64u\n", count));
			mutexRelease(reportMtx);

			eventSet(reportEv);
		}
		if(IsPrime_R(count))
//			errorCase(fprintf(fp, "%I64u\n", count) < 0);
			WrUI64(fp, count);
	}
	fileClose(fp);

	handleWaitForever(reportMtx);
	removeFileIfExist(reportFile);
	mutexRelease(reportMtx);

	handleClose(cancelEv);
	handleClose(reportEv);
	handleClose(reportMtx);
}
int main(int argc, char **argv)
{
	if(argIs("/P"))
	{
		uint64 value = toValue64(nextArg());

		if(IsPrime(value))
			cout("IS_PRIME\n");
		else
			cout("IS_NOT_PRIME\n");

		return;
	}
	if(argIs("/L"))
	{
		uint64 value = toValue64(nextArg());

		cout("%I64u\n", GetLowPrime(value));
		return;
	}
	if(argIs("/H"))
	{
		uint64 value = toValue64(nextArg());

		cout("%I64u\n", GetHiPrime(value));
		return;
	}
	if(argIs("/LH"))
	{
		uint64 value = toValue64(nextArg());

		cout("%I64u\n", GetLowPrime(value));
		cout("%I64u\n", GetHiPrime(value));
		return;
	}
	if(argIs("/R"))
	{
		uint64 minval;
		uint64 maxval;
		char *outFile;

		minval = toValue64(nextArg());
		maxval = toValue64(nextArg());
		outFile = nextArg();

		PrimeRange(
			minval,
			maxval,
			outFile,
			DUMMY_UUID "_1",
			DUMMY_UUID "_2",
			DUMMY_UUID "_3",
			makeTempFile("tmp")
			);
		return;
	}
	if(argIs("/R2"))
	{
		uint64 minval;
		uint64 maxval;
		char *outFile;
		char *cancelEvName;
		char *reportEvName;
		char *reportMtxName;
		char *reportFile;

		minval = toValue64(nextArg());
		maxval = toValue64(nextArg());
		outFile = nextArg();
		cancelEvName = nextArg();
		reportEvName = nextArg();
		reportMtxName = nextArg();
		reportFile = nextArg();

		PrimeRange(minval, maxval, outFile, cancelEvName, reportEvName, reportMtxName, reportFile);
		return;
	}
	if(argIs("/F"))
	{
		uint64 value = toValue64(nextArg());
		uint64 dest[64];
		uint index;

		Factorization(value, dest);

		for(index = 0; dest[index] != 0; index++)
		{
			cout("%I64u\n", dest[index]);
		}
		return;
	}
}