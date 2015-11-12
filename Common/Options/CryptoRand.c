/*
	getCryptoByte()のバイト列:

		sha-512(b[0]){0 -> 63} + sha-512(b[1]){0 -> 63} + sha-512(b[2]){0 -> 63} ...

	b:
		b[0] = GetCryptoSeed(){0 -> 4095}
		b[1] = b[0] + c[0]
		b[2] = b[0] + c[1]
		b[3] = b[0] + c[2]
		b[4] = b[0] + c[3]
		...

	c:
		c[0] = 0x00
		c[1] = 0x01
		c[2] = 0x02
		c[3] = 0x03
		...
		c[255] = 0xff
		c[256] = 0x00, 0x00
		c[257] = 0x01, 0x00
		...
		c[511] = 0xff, 0x00
		c[512] = 0x00, 0x01
		c[513] = 0x01, 0x01
		...
		c[65791] = 0xff, 0xff
		c[65792] = 0x00, 0x00, 0x00
		c[65793] = 0x01, 0x00, 0x00
		...

		★ {} = バイト列の並び方
*/

#include "CryptoRand.h"

void createKeyContainer(void)
{
	HCRYPTPROV hp;

	if(CryptAcquireContext(&hp, 0, 0, PROV_RSA_FULL, CRYPT_NEWKEYSET)) // エラー無視
		CryptReleaseContext(hp, 0);
}
void deleteKeyContainer(void)
{
	HCRYPTPROV hp;

	CryptAcquireContext(&hp, 0, 0, PROV_RSA_FULL, CRYPT_DELETEKEYSET); // エラー無視
}
static void GetCryptoBlock_MS(uchar *buffer, uint size)
{
	HCRYPTPROV hp;

	cout("Read sequence of %u bytes from 'CryptGenRandom' function.\n", size);

	if(!CryptAcquireContext(&hp, 0, 0, PROV_RSA_FULL, 0) &&
		(GetLastError() != NTE_BAD_KEYSET ||
			(cout("Create key container.\n"),
			!CryptAcquireContext(&hp, 0, 0, PROV_RSA_FULL, CRYPT_NEWKEYSET))))
		error();

	if(!CryptGenRandom(hp, size, buffer))
	{
		CryptReleaseContext(hp, 0);
		error();
	}
	CryptReleaseContext(hp, 0);
}
autoBlock_t *makeCryptoBlock_MS(uint count)
{
	autoBlock_t *block = nobCreateBlock(count);

	GetCryptoBlock_MS(directGetBuffer(block), count);
	return block;
}

#define SEED_DIR "C:\\Factory\\tmp"
#define SEED_FILE SEED_DIR "\\CSeed.dat"

#define SEEDSIZE 4096

static void GetCryptoSeed(uchar *seed)
{
	if(isFactoryDirEnabled() && existDir(SEED_DIR))
	{
		autoBlock_t gab;

		mutex();

		if(existFile(SEED_FILE))
		{
			FILE *fp;
			uint index;

			fp = fileOpen(SEED_FILE, "rb");
			fileRead(fp, gndBlockVar(seed, SEEDSIZE, gab));
			fileClose(fp);

			for(index = 0; index < SEEDSIZE; index++)
			{
				if(seed[index] < 0xff)
				{
					seed[index]++;
					break;
				}
				seed[index] = 0x00;
			}
		}
		else
			GetCryptoBlock_MS(seed, SEEDSIZE);

		writeBinary(SEED_FILE, gndBlockVar(seed, SEEDSIZE, gab));
		unmutex();
	}
	else
		GetCryptoBlock_MS(seed, SEEDSIZE);
}

#define BUFFERSIZE 64 // == sha512 hash size

static void GetCryptoBlock(uchar *buffer)
{
	static sha512_t *ctx;

	sha512_evacuate();

	if(!ctx)
	{
		uchar seed[SEEDSIZE];
		autoBlock_t gab;

		GetCryptoSeed(seed);

		ctx = sha512_create();
		sha512_update(ctx, gndBlockVar(seed, SEEDSIZE, gab));
		sha512_makeHash(ctx);
	}
	else
	{
		static autoBlock_t *tremor;
		sha512_t *ictx = sha512_copy(ctx);

		if(!tremor)
			tremor = newBlock();

		// tremor更新
		{
			uint index;

			for(index = 0; index < getSize(tremor); index++)
			{
				uint byteVal = getByte(tremor, index);

				if(byteVal < 0xff)
				{
					setByte(tremor, index, byteVal + 1);
					break;
				}
				setByte(tremor, index, 0x00);
			}
			if(index == getSize(tremor))
			{
				addByte(tremor, 0x00);
			}
		}

		sha512_update(ictx, tremor);
		sha512_makeHash(ictx);
		sha512_release(ictx);
	}
	memcpy(buffer, sha512_hash, BUFFERSIZE);
	sha512_unevacuate();
}
uint getCryptoByte(void)
{
	static uchar buffer[BUFFERSIZE];
	static uint index = BUFFERSIZE;

	if(BUFFERSIZE <= index)
	{
		GetCryptoBlock(buffer);
		index = 0;
	}
	return buffer[index++];
}
uint getCryptoRand16(void)
{
	return getCryptoByte() | getCryptoByte() << 8;
}
uint getCryptoRand24(void)
{
	return getCryptoByte() | getCryptoByte() << 8 | getCryptoByte() << 16;
}
uint getCryptoRand(void)
{
	return getCryptoByte() | getCryptoByte() << 8 | getCryptoByte() << 16 | getCryptoByte() << 24;
}
autoBlock_t *makeCryptoRandBlock(uint count)
{
	autoBlock_t *block = createBlock(count);

	while(count)
	{
		addByte(block, getCryptoByte());
		count--;
	}
	return block;
}
