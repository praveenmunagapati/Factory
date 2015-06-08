#include "ConsoleColor.h"

static void ChangeColor(int color)
{
	errorCase(!m_isRange(color, 0x00, 0xff));

	if(isChangeColorDisabled())
	{
		cout("CCD_COLOR %02x\n", color);
		return;
	}
	execute_x(xcout("COLOR %02x\n", color));
}

#define DEF_COLOR 0x0f

static int ResColor = DEF_COLOR;

void setEmgConsoleColor(int color)
{
	ChangeColor(color);
}
void setDefConsoleColor(void)
{
	setConsoleColor(DEF_COLOR);
}
void setConsoleColor(int color)
{
	ResColor = color;
	restoreConsoleColor();
}
void restoreConsoleColor(void)
{
	ChangeColor(ResColor);
}
