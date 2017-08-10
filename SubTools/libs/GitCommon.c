#include "GitCommon.h"

static int IsGitPath(char *path)
{
	return (int)mbs_stristr(path, "\\.git"); // .git で始まるローカル名を含む
}
void RemoveGitPaths(autoList_t *paths)
{
	char *path;
	uint index;

	foreach(paths, path, index)
		if(IsGitPath(path))
//cout("RGP_%s\n", path), // test
			path[0] = '\0';

	trimLines(paths);
}
