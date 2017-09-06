#include "MergeSort.h"

#define READER_MAX 16

typedef struct Reader_st
{
	FILE *FP;
	uint Element;
}
Reader_t;

sint (*CR_Comp)(uint, uint);

static sint CompReader(uint v1, uint v2)
{
	Reader_t *r1 = (Reader_t *)v1;
	Reader_t *r2 = (Reader_t *)v2;

	if(!r1->Element)
		return r2->Element ? 1 : 0;

	if(!r2->Element)
		return -1;

	return CR_Comp(r1->Element, r2->Element);
}
static char *GetPartFile(char *partsDir, uint partIndex)
{
	static char *file;

	memFree(file);
	file = combine_cx(partsDir, xcout("%010u.part", partIndex));
	return file;
}
static void CommitPart(
	char *partsDir,
	uint partIndex,
	char *wMode,
	void (*writeElement_x)(FILE *fp, uint element),
	sint (*compElement)(uint element1, uint element2),
	autoList_t *elements
	)
{
LOGPOS();
	rapidSort(elements, compElement);
LOGPOS();

	{
		FILE *fp = fileOpen(GetPartFile(partsDir, partIndex), wMode);
		uint element;
		uint index;

		foreach(elements, element, index)
			writeElement_x(fp, element);

		fileClose(fp);
		releaseAutoList(elements);
	}
}
static void MergePart(
	char *partsDir,
	uint partCount,
	char *destFile,
	char *rMode,
	char *wMode,
	uint (*readElement)(FILE *fp),
	void (*writeElement_x)(FILE *fp, uint element),
	sint (*compElement)(uint element1, uint element2)
	)
{
	uint rIndex;
	uint wIndex;
	uint readerCount;
	autoList_t *readers;
	Reader_t readerList[READER_MAX];
	uint index;
	FILE *wfp;

	errorCase(READER_MAX < 2); // 2bs
	errorCase(partCount < 2); // 2bs

	rIndex = 0;
	wIndex = partCount;

	CR_Comp = compElement;

	for(; ; )
	{
		readerCount = m_min(READER_MAX, wIndex - rIndex);
		readers = newList();

		for(index = 0; index < readerCount; index++)
		{
			FILE *fp = fileOpen(GetPartFile(partsDir, rIndex + index), rMode);

			readerList[index].FP = fp;
			readerList[index].Element = readElement(fp);

			addElement(readers, (uint)(readerList + index));
		}
		insertSort(readers, CompReader);

		rIndex += readerCount;
		wfp = fileOpen(rIndex == wIndex ? destFile : GetPartFile(partsDir, wIndex), wMode);

		for(; ; )
		{
			Reader_t *r = (Reader_t *)getElement(readers, 0);

			if(!r->Element)
				break;

			writeElement_x(wfp, r->Element);
			r->Element = readElement(r->FP);

			for(index = 1; index < readerCount; index++)
			{
				if(CompReader(getElement(readers, index - 1), getElement(readers, index)) <= 0)
					break;

				swapElement(readers, index - 1, index);
			}
		}
		fileClose(wfp);

		for(index = 0; index < readerCount; index++)
		{
			fileClose(readerList[index].FP);
			removeFile(GetPartFile(partsDir, rIndex - readerCount + index));
		}
		releaseAutoList(readers);

		if(rIndex == wIndex)
			break;

		wIndex++;
	}
}

/*
	srcFile
		�ǂݍ��݌��t�@�C��

	destFile
		�o�͐�t�@�C��
		srcFile �Ɠ����t�@�C���ł����Ă��ǂ��B

	textMode
		�^�̂Ƃ� -> �e�L�X�g���[�h
		�U�̂Ƃ� -> �o�C�i���[���[�h

	uint readElement(FILE *fp)
		�P���R�[�h�ǂݍ��ށB
		����ȏヌ�R�[�h�������Ƃ��� 0 ��Ԃ����ƁB

	void writeElement_x(FILE *fp, uint element)
		�P���R�[�h�������ށB
		�K�v�ł���� element ���J�����邱�ƁB

	sint compElement(uint element1, uint element2)
		element1 �� element2 ���r�������ʂ� strcmp() �I�ɕԂ��B

	partSize
		�������Ɉ�x���ɓǂݍ��߂�u���R�[�h�̍��v�o�C�g���v�̍ő�l�̖ڈ�
		srcFile �̃V�[�N�ʒu�̕ω����o�C�g�Ɋ��Z���Ă��邾���B
		�P���R�[�h���� recordConstWeightSize ��������B
		0 �̂Ƃ��͏�ɂP���R�[�h���ɂȂ�B

	recordConstWeightSize
		100 ���炢���w�肵�ĂˁB

*/
void MergeSort(
	char *srcFile,
	char *destFile,
	int textMode,
	uint (*readElement)(FILE *fp),
	void (*writeElement_x)(FILE *fp, uint element),
	sint (*compElement)(uint element1, uint element2),
	uint partSize,
	uint recordConstWeightSize
	)
{
	char *rMode;
	char *wMode;
	FILE *fp;
	char *partsDir = makeTempPath("parts");
	uint partCount = 0;
	autoList_t *elements = NULL;
	uint64 startPos = 0;

	if(textMode)
	{
		rMode = "rt";
		wMode = "wt";
	}
	else
	{
		rMode = "rb";
		wMode = "wb";
	}
	fp = fileOpen(srcFile, rMode);
	createDir(partsDir);

	for(; ; )
	{
		uint element = readElement(fp);
		uint64 currPos;
		uint64 readSize;

		if(!element)
			break;

		if(!elements)
			elements = createAutoList(partSize / 1024); // XXX

		addElement(elements, element);

		currPos = _ftelli64(fp);
		errorCase(currPos < 0);
		readSize = currPos - startPos;
		readSize += (uint64)getCount(elements) * recordConstWeightSize;

		if(partSize < readSize)
		{
			CommitPart(partsDir, partCount, wMode, writeElement_x, compElement, elements);
			partCount++;
			elements = NULL;
			startPos = currPos;
		}
	}
	if(elements)
	{
		CommitPart(partsDir, partCount, wMode, writeElement_x, compElement, elements);
		partCount++;
	}
	fileClose(fp);

	switch(partCount)
	{
	default: // 2-
		MergePart(partsDir, partCount, destFile, rMode, wMode, readElement, writeElement_x, compElement);
		break;

	case 1:
		removeFileIfExist(destFile);
		moveFile(GetPartFile(partsDir, 0), destFile);
		break;

	case 0:
		createFile(destFile);
		break;
	}
	removeDir(partsDir);
	memFree(partsDir);
}

void MergeSortTextComp(char *srcFile, char *destFile, sint (*funcComp)(char *, char *), uint partSize)
{
	MergeSort(srcFile, destFile, 1, (uint (*)(FILE *))readLine_strr, (void (*)(FILE *, uint))writeLine_x, (sint (*)(uint, uint))funcComp, partSize, 10);
	// �e�s 0�`2 �o�C�g�̂Ƃ��A10 �ɂ����� 1.8 GB ���炢�H�����B100 �� 180 MB ���炢�B100 �����S���ۂ��B@ 2016.3.18
	// <- readLine �̒��� createBlock(128); ���Ă邹������BreadLine -> readLine_strr �ɂ����B100 -> 10 �ɂ����B@ 2016.4.8
	// ���M�K�̋���t�@�C���ɂ��āA
	// �S�s 0 �o�C�g�Ńs�[�N�� 220 MB ���炢�B
	// �S�s 0�`2 �o�C�g�i���� 1 �o�C�g�j�Ńs�[�N�� 210 MB ���炢�B
	// �S�s 0�`1000 �o�C�g�i���� 500 �o�C�g�j�Ńs�[�N�� 140 MB ���炢�B
}
void MergeSortText(char *srcFile, char *destFile, uint partSize)
{
	MergeSortTextComp(srcFile, destFile, strcmp, partSize);
}
void MergeSortTextICase(char *srcFile, char *destFile, uint partSize)
{
//	MergeSortTextComp(srcFile, destFile, strcmp3, partSize); // strcmp3 <- rapidSort() �ɓn���̂� mbs_stricmp ����}�Y���B<- �}�Y���Ȃ��B@ 2016.6.8
	MergeSortTextComp(srcFile, destFile, mbs_stricmp, partSize);
}
