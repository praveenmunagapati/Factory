#include "all.h"

static autoList_t *Finalizers;

void addFinalizer(void (*finalizer)(void))
{
	errorCase(!finalizer);

	if(!Finalizers)
		Finalizers = createAutoList(1);

	addElement(Finalizers, (uint)finalizer);
}
void unaddFinalizer(void (*finalizer)(void))
{
	errorCase(!finalizer);
	errorCase(!Finalizers);

	removeElement(Finalizers, (uint)finalizer);
}
void runFinalizers(void)
{
	if(Finalizers)
	{
		while(getCount(Finalizers))
		{
			((void (*)(void))unaddElement(Finalizers))();
		}
	}
}
void termination(uint errorlevel)
{
	runFinalizers();
	termination_fileCloseAll();
	exit(errorlevel);
}

int errorOccurred;
char *errorPosSource = "?";
uint errorPosLineno;
char *errorPosFunction = "?";

void error2(char *source, uint lineno, char *function, char *message)
{
	static int busy;
	uint mtxhdl;

	// 再帰防止
	if(busy)
	{
		system("START ?_Error_In_Error"); // せめて何か出す。
		exit(2);
	}
	busy = 1;

	cout("+-------+\n");
	cout("| ERROR |\n");
	cout("+-------+\n");
	cout("%s (%u) %s\n", source, lineno, function);

	if(message)
		cout("----\n%s\n----\n", message);
//		cout("%s\n", message);
//		cout("error-reason: %s\n", message);

	errorOccurred = 1;
	errorPosSource = source;
	errorPosLineno = lineno;
	errorPosFunction = function;

	runFinalizers();

#define SRC_MUTEX "Mutex.c"
#define NM_MUTEX "ccstackprobe Factory error mutex object"

	if(!_stricmp(source, SRC_MUTEX)) // Mutex のエラーなら Mutex は使えないだろう。
	{
		system("START ?_Mutex_Error"); // せめて何か出す。
		goto endproc;
	}
	mtxhdl = mutexLock(NM_MUTEX);

#if 0
#define ERROR_LOG_FILE "C:\\Factory\\tmp\\Error.txt"
	{
		FILE *fp = rfopen(ERROR_LOG_FILE, "at");
		char *strw;
		char *strw2;

		if(fp)
		{
			writeLine(fp, strw = xcout("%s %s %s (%u) %s", strw2 = makeJStamp(NULL, 0), getSelfFile(), source, lineno, function));
			memFree(strw);
			memFree(strw2);
			fileClose(fp);
		}
	}
#endif

	{
		char *vbsfile = makeTempPath("vbs");
		char *strw;
		char *mbMessage;

		if(message)
			mbMessage = strx(message);
		else
			mbMessage = xcout("An error has occurred @ %s (%u) %s", source, lineno, function);

		writeOneLine(vbsfile, strw = xcout("MsgBox \"%s\", 4096 + 16, \"Error\"", mbMessage));
		memFree(strw);
		memFree(mbMessage);

		/*
			@ 2015.7.23
			このプロセスのメモリが逼迫している(2G近くになる)と、下の vbs が実行されないことがあるぽい。
			system はちゃんとキックしてるぽい。
		*/

		if(isFactoryDirEnabled())
			execute(vbsfile);
		else
			execute_x(xcout("START \"\" /WAIT \"%s\"", vbsfile));

		removeFile(vbsfile);
		memFree(vbsfile);
	}
	mutexUnlock(mtxhdl);

endproc:
	termination(1);
}
