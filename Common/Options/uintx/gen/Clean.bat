rem gen-src

C:\Factory\SubTools\rept.exe /P %%HBIT%% 128 /P %%LBIT%% 64 /P %%HSZ%% 4 /P %%LSZ%% 2 uintx.c.h ..\uint128.c
C:\Factory\SubTools\rept.exe /P %%HBIT%% 128 /P %%LBIT%% 64 /P %%HSZ%% 4 /P %%LSZ%% 2 uintx.h.h ..\uint128.h

C:\Factory\SubTools\rept.exe /P %%HBIT%% 256 /P %%LBIT%% 128 /P %%HSZ%% 8 /P %%LSZ%% 4 uintx.c.h ..\uint256.c
C:\Factory\SubTools\rept.exe /P %%HBIT%% 256 /P %%LBIT%% 128 /P %%HSZ%% 8 /P %%LSZ%% 4 uintx.h.h ..\uint256.h

C:\Factory\SubTools\rept.exe /P %%HBIT%% 512 /P %%LBIT%% 256 /P %%HSZ%% 16 /P %%LSZ%% 8 uintx.c.h ..\uint512.c
C:\Factory\SubTools\rept.exe /P %%HBIT%% 512 /P %%LBIT%% 256 /P %%HSZ%% 16 /P %%LSZ%% 8 uintx.h.h ..\uint512.h

C:\Factory\SubTools\rept.exe /P %%HBIT%% 1024 /P %%LBIT%% 512 /P %%HSZ%% 32 /P %%LSZ%% 16 uintx.c.h ..\uint1024.c
C:\Factory\SubTools\rept.exe /P %%HBIT%% 1024 /P %%LBIT%% 512 /P %%HSZ%% 32 /P %%LSZ%% 16 uintx.h.h ..\uint1024.h

C:\Factory\SubTools\rept.exe /P %%HBIT%% 2048 /P %%LBIT%% 1024 /P %%HSZ%% 64 /P %%LSZ%% 32 uintx.c.h ..\uint2048.c
C:\Factory\SubTools\rept.exe /P %%HBIT%% 2048 /P %%LBIT%% 1024 /P %%HSZ%% 64 /P %%LSZ%% 32 uintx.h.h ..\uint2048.h

C:\Factory\SubTools\rept.exe /P %%HBIT%% 4096 /P %%LBIT%% 2048 /P %%HSZ%% 128 /P %%LSZ%% 64 uintx.c.h ..\uint4096.c
C:\Factory\SubTools\rept.exe /P %%HBIT%% 4096 /P %%LBIT%% 2048 /P %%HSZ%% 128 /P %%LSZ%% 64 uintx.h.h ..\uint4096.h
