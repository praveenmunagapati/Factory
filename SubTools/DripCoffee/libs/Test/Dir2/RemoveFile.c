#include "..\..\all.h"

int main(int argc, char **argv)
{
	errorCase_m(!DC_RemoveFile(nextArg()), "失敗しました。");
}
