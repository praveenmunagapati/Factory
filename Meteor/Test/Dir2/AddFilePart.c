#include "..\..\Dir2.h"

int main(int argc, char **argv)
{
	errorCase_m(!DC_AddFilePart(getArg(0), toValue64(getArg(1)), readBinary(getArg(2))), "���s���܂����B"); // g
}
