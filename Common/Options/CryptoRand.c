/*
	getCryptoByte()のバイト列:

		xor(a1, a2)

	a1:
		Camellia(b[0], k1) +
		Camellia(b[1], k1) +
		Camellia(b[2], k1) +
		...

	a2:
		Camellia(b[0], k2) +
		Camellia(b[1], k2) +
		Camellia(b[2], k2) +
		...

	b:
		cb[0] = 0x00, 0x00, 0x00, ... 0x00, 0x00, 0x00 // 16 バイト
		cb[1] = 0x01, 0x00, 0x00, ... 0x00, 0x00, 0x00
		cb[2] = 0x02, 0x00, 0x00, ... 0x00, 0x00, 0x00
		...
		cb[255] = 0xff, 0x00, 0x00, ... 0x00, 0x00, 0x00
		cb[256] = 0x00, 0x01, 0x00, ... 0x00, 0x00, 0x00
		cb[257] = 0x01, 0x01, 0x00, ... 0x00, 0x00, 0x00
		cb[258] = 0x02, 0x01, 0x00, ... 0x00, 0x00, 0x00
		...
		cb[2^128-1] = 0xff, 0xff, 0xff, ... 0xff, 0xff, 0xff
		cb[2^128+0] = 0x00, 0x00, 0x00, ... 0x00, 0x00, 0x00
		cb[2^128+1] = 0x01, 0x00, 0x00, ... 0x00, 0x00, 0x00
		cb[2^128+2] = 0x02, 0x00, 0x00, ... 0x00, 0x00, 0x00
		...

	k1:
		SHA512(s1)の前半256bit

	s1:
		GetCryptoSeed(, 4096, SEED_FILE) // 1プロセス中1度だけ呼ばれる。

	k2:
		SHA512(s2)の前半256bit

	s2:
		GetCryptoSeed(, 4096, CA_SEED_FILE) // 1プロセス中1度だけ呼ばれる。

	- - -

	Camellia(plain, raw-key) = 鍵長 256 bit の Camellia ブロックの暗号化

	2^128 * 16 バイトの周期を持つが、量的に一周することは有り得ないだろう。

*/

#include "CryptoRand.h"

#define SEED_DIR "C:\\Factory\\tmp"
#define SEED_FILE    SEED_DIR "\\CSeed.dat"
#define CA_SEED_FILE SEED_DIR "\\CSeedCa.dat"

#define SEEDSIZE 4096

static void GetCryptoSeed(uchar *seed, uint seed_size, char *seed_file)
{
	if(isFactoryDirEnabled())
	{
		autoBlock_t gab;

		mutex();

		// zantei >

		if(existFile(seed_file) && getFileSize(seed_file) != (uint64)seed_size)
		{
			cout("#########################################################\n");
			cout("## SEED_FILE SIZE ERROR -- どっかに古い exe があるで！ ##\n");
			cout("#########################################################\n");

			removeFile(seed_file);
		}

		// < zantei

		if(existFile(seed_file))
		{
			FILE *fp;
			uint index;

			fp = fileOpen(seed_file, "rb");
			fileRead(fp, gndBlockVar(seed, seed_size, gab));
			fileClose(fp);

			for(index = 0; index < seed_size; index++)
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
			getCryptoBlock_MS(seed, seed_size);

		writeBinary(seed_file, gndBlockVar(seed, seed_size, gab));
		unmutex();
	}
	else
		getCryptoBlock_MS(seed, seed_size);
}

#define BUFFERSIZE 16 // == camellia block size

static void XorBlock(void *b1, void *b2)
{
	((uint *)b1)[0] ^= ((uint *)b2)[0];
	((uint *)b1)[1] ^= ((uint *)b2)[1];
	((uint *)b1)[2] ^= ((uint *)b2)[2];
	((uint *)b1)[3] ^= ((uint *)b2)[3];
}
static void GetCryptoBlock(uchar *buffer)
{
	static camellia_keyTable_t *cam_kt[2];
	static uchar counter[16];
	static uchar dest[16];

	if(!cam_kt[0]) // 初期化
	{
		uchar (*cam_seed)[SEEDSIZE] = (uchar (*)[SEEDSIZE])memAlloc(SEEDSIZE * 2);
		autoBlock_t gab;

		GetCryptoSeed(cam_seed[0], SEEDSIZE, SEED_FILE);
		GetCryptoSeed(cam_seed[1], SEEDSIZE, CA_SEED_FILE);

		sha512_localize();

		sha512_makeHashBlock(gndBlockVar(cam_seed[0], SEEDSIZE, gab));
		memcpy(cam_seed[0], sha512_hash, 32);
		sha512_makeHashBlock(gndBlockVar(cam_seed[1], SEEDSIZE, gab));
		memcpy(cam_seed[1], sha512_hash, 32);

		sha512_unlocalize();

		errorCase(!memcmp(cam_seed[0], cam_seed[1], SEEDSIZE)); // 不幸な事故

		cam_kt[0] = camellia_createKeyTable(gndBlockVar(cam_seed[0], 32, gab));
		cam_kt[1] = camellia_createKeyTable(gndBlockVar(cam_seed[1], 32, gab));

		memFree(cam_seed);
	}
	else // カウンタ更新
	{
		uint index;

		for(index = 0; ; index++)
//		for(index = 0; index < 16; index++)
		{
			if(counter[index] < 0xff)
			{
				counter[index]++;
				break;
			}
			counter[index] = 0x00;
		}
	}
	camellia_encrypt(cam_kt[0], counter, buffer, 1);
	camellia_encrypt(cam_kt[1], counter, dest, 1);
	XorBlock(buffer, dest);
}
uint getCryptoByte(void)
{
	static uchar buffer[BUFFERSIZE];
	static uint index = BUFFERSIZE;

	if(index == BUFFERSIZE)
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
uint64 getCryptoRand64(void)
{
	return
		(uint64)getCryptoByte() <<  0 |
		(uint64)getCryptoByte() <<  8 |
		(uint64)getCryptoByte() << 16 |
		(uint64)getCryptoByte() << 24 |
		(uint64)getCryptoByte() << 32 |
		(uint64)getCryptoByte() << 40 |
		(uint64)getCryptoByte() << 48 |
		(uint64)getCryptoByte() << 56;
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
