/*
	qrumall [/D [COMMENT] | ROOT-DIR]

		/D ... Daily-Release mode
*/

#include "C:\Factory\Common\all.h"

int main(int argc, char **argv)
{
	int dailymode = argIs("/D");
	autoList_t *dirs;
	char *dir;
	uint index;

	if(dailymode)
	{
		char *comment = hasArgs(1) ? nextArg() : "Daily";

		dirs = lssDirs("C:\\Dev");

		coExecute_x(xcout("rum /c \"%s\"", comment));

		coExecute("frum -qa");
	}
	else
	{
		char *rootDir = hasArgs(1) ? nextArg() : c_dropDir();

		dirs = lssDirs(rootDir);

		coExecute("rum /c");

		// confirm
		{
			cout("���s�H\n");

			if(clearGetKey() == 0x1b)
				termination(0);

			cout("���s���܂��B\n");
		}
	}

	foreach(dirs, dir, index)
	{
		char *rumDir = addExt(strx(dir), "rum");

		if(existDir(rumDir))
		{
			cout("* %s\n", dir);

			addCwd(dir);
			{
				coExecute("qrum -- -qa");
			}
			unaddCwd();
		}
		memFree(rumDir);
	}
	releaseDim(dirs, 1);

	coExecute("rum /c-");
}