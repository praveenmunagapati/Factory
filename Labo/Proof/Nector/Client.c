/*
	START Server.exe

	�ŁA�N�����Ă����āA

	Client.exe ����ނ݂炢�����̂Ȃ񂽂炩�񂽂�...

	���Ă��B
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\SubTools\libs\Nector.h"

int main(int argc, char **argv)
{
	Nector_t *i = CreateNector("Nector_Test");

	NectorSendLine(i, nextArg());
	ReleaseNector(i);
}
