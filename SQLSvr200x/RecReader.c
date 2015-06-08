#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\csv.h"
#include "libs\RecReader.h"

int main(int argc, char **argv)
{
	int noHeader = 0;
	char *recFile;
	char *csvFile;
	autoList_t *table;

	if(argIs("/-H"))
		noHeader = 1;

	recFile = nextArg();
	csvFile = nextArg();

	cout("< %s\n", recFile);
	cout("> %s\n", csvFile);

	table = SqlRecReader(recFile);

	if(noHeader)
		releaseDim((autoList_t *)desertElement(table, 0), 1);

	writeCSVFile_cx(csvFile, table);
}
