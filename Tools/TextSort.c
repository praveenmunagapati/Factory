// smpl

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\MergeSort.h"

int main(int argc, char **argv)
{
	uint partSize = 128 * 1024 * 1024;

	if(argIs("/S"))
	{
		partSize = toValue(nextArg());
	}

	if(argIs("/1"))
	{
		MergeSortText(getArg(0), getArg(0), partSize);
	}
	else
	{
		MergeSortText(getArg(0), getArg(1), partSize);
	}
}
