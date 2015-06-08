#include "CCCCommon.h"

/*
	片方のソケットが切断した場合の以下のタイムアウト [秒]

	・切断したソケットから読み込めなくなるまで読み込む (CCのみ)
	・切断していないソケットへバッファが空になるまで書き込む (CommとCC)

	値域は 0 〜 IMAX とする。上限に特に根拠はない。

	0 == タイムアウト無し
*/
uint DOSTimeoutSec = 3600;
