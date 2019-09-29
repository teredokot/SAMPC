all:
	gcc -O2 -c -fpack-struct=1 -w -Iamx/ -DLINUX -DSAMPSRV -DAMX_NODYNALOAD amx/*.c
	g++ -O2 -c -fpack-struct=1 -w -Iamx/ -DLINUX -DSAMPSRV -DAMX_NODYNALOAD *.cpp
	g++ -O2 -c -fpack-struct=1 -w -Iamx/ -DLINUX -DSAMPSRV -DAMX_NODYNALOAD ../raknet/*.cpp
	g++ -O2    -fpack-struct=1 -w -Iamx/ -ldl -lpthread -lm -o mpsvr -DLINUX -DSAMPSRV -DAMX_NODYNALOAD *.o

debug:
	gcc -c -g -fpack-struct=1 -w -Iamx/ -DLINUX -DSAMPSRV -DAMX_NODYNALOAD amx/*.c
	g++ -c -g -fpack-struct=1 -w -Iamx/ -DLINUX -DSAMPSRV -DAMX_NODYNALOAD *.cpp
	g++ -c -g -fpack-struct=1 -w -Iamx/ -DLINUX -DSAMPSRV -DAMX_NODYNALOAD ../raknet/*.cpp
	g++    -g -fpack-struct=1 -w -Iamx/ -ldl -lpthread -lm -o mpsvrd -DLINUX -DSAMPSRV -DAMX_NODYNALOAD *.o
