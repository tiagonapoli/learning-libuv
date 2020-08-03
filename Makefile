LIBUV_DIR=./libuv-1.38.1
LIBUV_BUILD_DIR=$(LIBUV_DIR)/build
LIBUV_INCLUDE_DIR=$(LIBUV_DIR)/include
LIBUV_A=$(LIBUV_BUILD_DIR)/libuv_a.a

COMPILE_FLAGS=-Wall
LIBS=-ldl -pthread

GENERIC_COMPILE=g++ ${COMPILE_FLAGS} $< -o $@ ${LIBUV_A} -I ${LIBUV_INCLUDE_DIR} ${LIBS}

all: hello.out idle.out cat.out

hello.out: ./src/hello-world.c
	${GENERIC_COMPILE}

idle.out: ./src/idle-handler.c
	${GENERIC_COMPILE}

cat.out: ./src/cat.c libmyutils.a
	${GENERIC_COMPILE} ./libmyutils.a

tee.out: ./src/tee.c
	${GENERIC_COMPILE}

file-watcher.out: ./src/file-watcher.c
	${GENERIC_COMPILE}

libmyutils.a: ./src/utils/utils.c
	g++ ${COMPILE_FLAGS} -c $< -o utils.o
	ar rv libmyutils.a ./utils.o
	rm utils.o

clean:
	rm -rf ./*.out ./*.o ./*.a