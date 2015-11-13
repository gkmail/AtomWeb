include config.mk

Q:=@

VERSION:=0.0.1

GLOBAL_CFLAGS:=-Wall -O2 -Iinclude
GLOBAL_HOST_CFLAGS:=-Wall -O2
GLOBAL_LDFLAGS:=

ifeq ($(DEBUG),1)
	GLOBAL_HOST_CFLAGS+=-DAW_DEBUG -Iinclude
endif

CONFIG_NAME:=aw_config.h
CONFIG_INCLUDE:=ARCH
CONFIG_NUMBER:=DEBUG\
	MEMWATCH\
	INPUT_BUFFER_SIZE
CONFIG_STRING:=
CONFIG_PREFIX:=AW_

include build/head.mk

TARGET:=libatomweb
SRCS:=src/aw_hash.c\
	src/aw_manager.c\
	src/aw_uri.c\
	src/aw_server.c\
	src/aw_session.c\
	src/aw_object.c\
	src/aw_map.c
ifeq ($(MEMWATCH),1)
SRCS+=src/memwatch.c
endif
include build/slib.mk

TARGET:=aw-scanner
SRCS:=tools/aw-scanner.c\
	tools/aw-string.c
ifeq ($(DEBUG),1)
SRCS+=src/memwatch.c
endif
include build/host_exe.mk

TARGET:=server_test
SRCS:=tests/server_test.c
SLIBS:=libatomweb
include build/exe.mk

TARGET:=uri_test
SRCS:=tests/uri_test.c
SLIBS:=libatomweb
include build/exe.mk

include build/install.mk

include build/tail.mk
