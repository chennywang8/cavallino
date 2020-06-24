# Chenny Wang
# --------- BUILD CFG ------------
CC = /cygdrive/c/build/18.0/sysroots/i686-nilrtsdk-mingw32/usr/bin/arm-nilrt-linux-gnueabi/arm-nilrt-linux-gnueabi-gcc.exe
SYSROOT  = "C:\build\18.0\sysroots\cortexa9-vfpv3-nilrt-linux-gnueabi"
LIBS	 = -L"C:\build\18.0\sysroots\cortexa9-vfpv3-nilrt-linux-gnueabi\usr\local\vxipnp\linux\lib" -ldl -lpthread -lzmq -lvisa
CFLAGS   = -I../include -I../fpga -I../../visa  \
		   -mfpu=vfp3 -mfloat-abi=softfp --sysroot=${SYSROOT} -w

# --------- FILES -----------------
HEADERS  = $(wildcard ../include/*.h) $(wildcard ../fpga/*.h) $(wildcard ../../visa/*.h)
OBJ_DATA = $(patsubst %.c, %.o, $(wildcard ../src/*.c))

# --------- TARGET CFG ------------
# config for target connection
TARGETIP = 192.168.1.84
USERNAME = admin
TARGET	 = ${USERNAME}@${TARGETIP}:/home/${USERNAME}

# ---------- MAKE ------------
all : server

server: ${OBJ_DATA}
	echo ${OBJ_DATA}
	${CC} --sysroot=${SYSROOT} ${OBJ_DATA} ${LIBS} -o $@
	# copy files to target
	scp server ../fpga/*.lvbitx ${TARGET}

%.o: %.c ${HEADERS}
	${CC} -c ${CFLAGS} $< -o $@

.PHONY : clean
clean : 
	-rm -v -f ../src/*.o server	 
	