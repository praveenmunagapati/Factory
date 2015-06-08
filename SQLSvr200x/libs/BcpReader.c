#include "BcpReader.h"

autoList_t *SqlBcpReader(char *bcpFile)
{
	autoList_t *lines;
	char *line;
	uint index;
	autoList_t *table = newList();

	errorCase(m_isEmpty(bcpFile));

	lines = readLines(bcpFile);

	foreach(lines, line, index)
	{
		line2JLine(line, 1, 0, 1, 1);

		addElement(table, (uint)tokenize(line, '\t'));
	}
	releaseDim(lines, 1);

	// check
	{
		if(getCount(table)) // ? �s���P�ȏ゠��B
		{
			uint colcnt = getCount(getList(table, 0));
			autoList_t * row;
			uint rowidx;
			char *cell;
			uint colidx;

			errorCase(colcnt < 1); // ? �񂪂P�������B

			foreach(table, row, rowidx)
			{
				errorCase(getCount(row) != colcnt); // ? �񐔂����ł͂Ȃ��B

				foreach(row, cell, colidx)
				{
					errorCase(cell == NULL); // ? cell == NULL
				}
			}
		}
	}
	return table;
}
