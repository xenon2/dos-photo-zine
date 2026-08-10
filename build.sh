# setup env
# 32 bit version do not work under WSL
#export PATH="$WATCOM/binl64:$PATH" INCLUDE="$WATCOM/h"
#wcl -bt=dos -ml -i=../bdgl/include src/main.c -fe=main.exe
make clean
make