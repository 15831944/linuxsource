# Makefile generated by the project manager
#


## core information

PRJ_FILE       = h320mtadp.wpj
PRJ_DIR        = ./
PRJ_TYPE       = vxApp
PRJ_OBJS       = MtAdp.o MtAdpCom.o MtAdpd.o MtAdpInst.o MtAdpNet.o
BUILD_SPEC     = Release
TGT_DIR        = $(WIND_BASE)/target




## build-configuration info

ifeq ($(BUILD_SPEC),PPCEC603gnu)
CPU              = PPCEC603
TOOL             = gnu
DEFAULT_RULE     = h320mtadp.out
endif

ifeq ($(BUILD_SPEC),Release)
CPU              = PPCEC603
TOOL             = gnu
DEFAULT_RULE     = h320mtadp.out
endif

include $(TGT_DIR)/h/make/defs.project

## build-configuration info

ifeq ($(BUILD_SPEC),PPCEC603gnu)
AR               = arppc
AS               = ccppc
CC               = ccppc
CFLAGS           = -g -O0 -mlongcall -mstrict-align -ansi -nostdinc -DRW_MULTI_THREAD -D_REENTRANT -fvolatile -fno-builtin -fno-for-scope -msoft-float -I. -I$(TGT_DIR)/h -DCPU=PPCEC603 -D_VXWORKS_ -I$(PRJ_DIR)/../include -I$(PRJ_DIR)/../../common/include -I$(PRJ_DIR)/../../../10-common/include/platform -I$(PRJ_DIR)/../../../10-common/include/system1 -I$(PRJ_DIR)/../../../10-common/include/mcu -I$(PRJ_DIR)/../../../10-common/include/video1 -I$(PRJ_DIR)/../../../10-common/include/protocol
CFLAGS_AS        = -g -mstrict-align -ansi -nostdinc -fvolatile -fno-builtin -fno-for-scope -P -x assembler-with-cpp -I. -I$(TGT_DIR)/h -DCPU=PPCEC603
CPP              = ccppc -E -P -xc
LD               = ldppc
LDDEPS           = 
LDFLAGS          = -X -N
LD_PARTIAL_FLAGS = -X -r
NM               = nmppc
OPTION_DEFINE_MACRO = -D
OPTION_INCLUDE_DIR = -I
POST_BUILD_RULE  = 
PRJ_LIBS         = $(PRJ_DIR)/../../../10-common/lib/debug/8260/osp.a $(PRJ_DIR)/../../../10-common/lib/debug/8260/driagent.a $(PRJ_DIR)/../../../10-common/lib/debug/8260/kdvsys.a $(PRJ_DIR)/../../../10-common/lib/debug/8260/KdvH320.a $(PRJ_DIR)/../../../10-common/lib/debug/8260/kdvmedianet.a $(PRJ_DIR)/../../../10-common/lib/debug/8260/kdvencrypt.a $(PRJ_DIR)/../../../10-common/lib/debug/8260/h261addrtpheader.a $(PRJ_DIR)/../../../10-common/lib/debug/8260/h263decaddrtp.a 
SIZE             = sizeppc
endif

ifeq ($(BUILD_SPEC),Release)
AR               = arppc
AS               = ccppc
CC               = ccppc
CFLAGS           = -g -O1 -mlongcall -mstrict-align -ansi -nostdinc -DRW_MULTI_THREAD -D_REENTRANT -fvolatile -fno-builtin -fno-for-scope -msoft-float -I. -I$(TGT_DIR)/h -DCPU=PPCEC603 -D_VXWORKS_ -I$(PRJ_DIR)/../include -I$(PRJ_DIR)/../../common/include -I$(PRJ_DIR)/../../../10-common/include/platform -I$(PRJ_DIR)/../../../10-common/include/system1 -I$(PRJ_DIR)/../../../10-common/include/mcu -I$(PRJ_DIR)/../../../10-common/include/video1 -I$(PRJ_DIR)/../../../10-common/include/protocol
CFLAGS_AS        = -g -mstrict-align -ansi -nostdinc -fvolatile -fno-builtin -fno-for-scope -P -x assembler-with-cpp -I. -I$(TGT_DIR)/h -DCPU=PPCEC603
CPP              = ccppc -E -P -xc
LD               = ldppc
LDDEPS           = 
LDFLAGS          = -X -N
LD_PARTIAL_FLAGS = -X -r
NM               = nmppc
OPTION_DEFINE_MACRO = -D
OPTION_INCLUDE_DIR = -I
POST_BUILD_RULE  = 
PRJ_LIBS         = $(PRJ_DIR)/../../../10-common/lib/release/8260/osp.a $(PRJ_DIR)/../../../10-common/lib/release/8260/driagent.a $(PRJ_DIR)/../../../10-common/lib/release/8260/kdvsys.a $(PRJ_DIR)/../../../10-common/lib/release/8260/KdvH320.a $(PRJ_DIR)/../../../10-common/lib/release/8260/kdvmedianet.a $(PRJ_DIR)/../../../10-common/lib/release/8260/kdvencrypt.a $(PRJ_DIR)/../../../10-common/lib/release/8260/h261addrtpheader.a $(PRJ_DIR)/../../../10-common/lib/release/8260/h263decaddrtp.a 
SIZE             = sizeppc
endif

# override make definitions only below this line


# override make definitions only above this line
include $(TGT_DIR)/h/make/rules.project



## build-configuration info

ifeq ($(BUILD_SPEC),PPCEC603gnu)
MtAdp.o:
	$(CC) -g -O0 -mlongcall -mstrict-align -ansi -nostdinc -DRW_MULTI_THREAD -D_REENTRANT -fvolatile -fno-builtin -fno-for-scope -msoft-float -I. -I$(WIND_BASE)/target/h -DCPU=PPCEC603 -D_VXWORKS_ -I$(PRJ_DIR)/../include -I$(PRJ_DIR)/../../common/include -I$(PRJ_DIR)/../../../10-common/include/platform -I$(PRJ_DIR)/../../../10-common/include/system1 -I$(PRJ_DIR)/../../../10-common/include/mcu -I$(PRJ_DIR)/../../../10-common/include/video1 -I$(PRJ_DIR)/../../../10-common/include/protocol -c $(PRJ_DIR)/../source/MtAdp.cpp
MtAdpCom.o:
	$(CC) -g -O0 -mlongcall -mstrict-align -ansi -nostdinc -DRW_MULTI_THREAD -D_REENTRANT -fvolatile -fno-builtin -fno-for-scope -msoft-float -I. -I$(WIND_BASE)/target/h -DCPU=PPCEC603 -D_VXWORKS_ -I$(PRJ_DIR)/../include -I$(PRJ_DIR)/../../common/include -I$(PRJ_DIR)/../../../10-common/include/platform -I$(PRJ_DIR)/../../../10-common/include/system1 -I$(PRJ_DIR)/../../../10-common/include/mcu -I$(PRJ_DIR)/../../../10-common/include/video1 -I$(PRJ_DIR)/../../../10-common/include/protocol -c $(PRJ_DIR)/../source/MtAdpCom.cpp
MtAdpd.o:
	$(CC) -g -O0 -mlongcall -mstrict-align -ansi -nostdinc -DRW_MULTI_THREAD -D_REENTRANT -fvolatile -fno-builtin -fno-for-scope -msoft-float -I. -I$(WIND_BASE)/target/h -DCPU=PPCEC603 -D_VXWORKS_ -I$(PRJ_DIR)/../include -I$(PRJ_DIR)/../../common/include -I$(PRJ_DIR)/../../../10-common/include/platform -I$(PRJ_DIR)/../../../10-common/include/system1 -I$(PRJ_DIR)/../../../10-common/include/mcu -I$(PRJ_DIR)/../../../10-common/include/video1 -I$(PRJ_DIR)/../../../10-common/include/protocol -c $(PRJ_DIR)/../source/MtAdpd.cpp
MtAdpInst.o:
	$(CC) -g -O0 -mlongcall -mstrict-align -ansi -nostdinc -DRW_MULTI_THREAD -D_REENTRANT -fvolatile -fno-builtin -fno-for-scope -msoft-float -I. -I$(WIND_BASE)/target/h -DCPU=PPCEC603 -D_VXWORKS_ -I$(PRJ_DIR)/../include -I$(PRJ_DIR)/../../common/include -I$(PRJ_DIR)/../../../10-common/include/platform -I$(PRJ_DIR)/../../../10-common/include/system1 -I$(PRJ_DIR)/../../../10-common/include/mcu -I$(PRJ_DIR)/../../../10-common/include/video1 -I$(PRJ_DIR)/../../../10-common/include/protocol -c $(PRJ_DIR)/../source/MtAdpInst.cpp
MtAdpNet.o:
	$(CC) -g -O0 -mlongcall -mstrict-align -ansi -nostdinc -DRW_MULTI_THREAD -D_REENTRANT -fvolatile -fno-builtin -fno-for-scope -msoft-float -I. -I$(WIND_BASE)/target/h -DCPU=PPCEC603 -D_VXWORKS_ -I$(PRJ_DIR)/../include -I$(PRJ_DIR)/../../common/include -I$(PRJ_DIR)/../../../10-common/include/platform -I$(PRJ_DIR)/../../../10-common/include/system1 -I$(PRJ_DIR)/../../../10-common/include/mcu -I$(PRJ_DIR)/../../../10-common/include/video1 -I$(PRJ_DIR)/../../../10-common/include/protocol -c $(PRJ_DIR)/../source/MtAdpNet.cpp
endif

ifeq ($(BUILD_SPEC),Release)
MtAdp.o:
	$(CC) -g -O1 -mlongcall -mstrict-align -ansi -nostdinc -DRW_MULTI_THREAD -D_REENTRANT -fvolatile -fno-builtin -fno-for-scope -msoft-float -I. -I$(WIND_BASE)/target/h -DCPU=PPCEC603 -D_VXWORKS_ -I$(PRJ_DIR)/../include -I$(PRJ_DIR)/../../common/include -I$(PRJ_DIR)/../../../10-common/include/platform -I$(PRJ_DIR)/../../../10-common/include/system1 -I$(PRJ_DIR)/../../../10-common/include/mcu -I$(PRJ_DIR)/../../../10-common/include/video1 -I$(PRJ_DIR)/../../../10-common/include/protocol -c $(PRJ_DIR)/../source/MtAdp.cpp
MtAdpCom.o:
	$(CC) -g -O1 -mlongcall -mstrict-align -ansi -nostdinc -DRW_MULTI_THREAD -D_REENTRANT -fvolatile -fno-builtin -fno-for-scope -msoft-float -I. -I$(WIND_BASE)/target/h -DCPU=PPCEC603 -D_VXWORKS_ -I$(PRJ_DIR)/../include -I$(PRJ_DIR)/../../common/include -I$(PRJ_DIR)/../../../10-common/include/platform -I$(PRJ_DIR)/../../../10-common/include/system1 -I$(PRJ_DIR)/../../../10-common/include/mcu -I$(PRJ_DIR)/../../../10-common/include/video1 -I$(PRJ_DIR)/../../../10-common/include/protocol -c $(PRJ_DIR)/../source/MtAdpCom.cpp
MtAdpd.o:
	$(CC) -g -O1 -mlongcall -mstrict-align -ansi -nostdinc -DRW_MULTI_THREAD -D_REENTRANT -fvolatile -fno-builtin -fno-for-scope -msoft-float -I. -I$(WIND_BASE)/target/h -DCPU=PPCEC603 -D_VXWORKS_ -I$(PRJ_DIR)/../include -I$(PRJ_DIR)/../../common/include -I$(PRJ_DIR)/../../../10-common/include/platform -I$(PRJ_DIR)/../../../10-common/include/system1 -I$(PRJ_DIR)/../../../10-common/include/mcu -I$(PRJ_DIR)/../../../10-common/include/video1 -I$(PRJ_DIR)/../../../10-common/include/protocol -c $(PRJ_DIR)/../source/MtAdpd.cpp
MtAdpInst.o:
	$(CC) -g -O1 -mlongcall -mstrict-align -ansi -nostdinc -DRW_MULTI_THREAD -D_REENTRANT -fvolatile -fno-builtin -fno-for-scope -msoft-float -I. -I$(WIND_BASE)/target/h -DCPU=PPCEC603 -D_VXWORKS_ -I$(PRJ_DIR)/../include -I$(PRJ_DIR)/../../common/include -I$(PRJ_DIR)/../../../10-common/include/platform -I$(PRJ_DIR)/../../../10-common/include/system1 -I$(PRJ_DIR)/../../../10-common/include/mcu -I$(PRJ_DIR)/../../../10-common/include/video1 -I$(PRJ_DIR)/../../../10-common/include/protocol -c $(PRJ_DIR)/../source/MtAdpInst.cpp
MtAdpNet.o:
	$(CC) -g -O1 -mlongcall -mstrict-align -ansi -nostdinc -DRW_MULTI_THREAD -D_REENTRANT -fvolatile -fno-builtin -fno-for-scope -msoft-float -I. -I$(WIND_BASE)/target/h -DCPU=PPCEC603 -D_VXWORKS_ -I$(PRJ_DIR)/../include -I$(PRJ_DIR)/../../common/include -I$(PRJ_DIR)/../../../10-common/include/platform -I$(PRJ_DIR)/../../../10-common/include/system1 -I$(PRJ_DIR)/../../../10-common/include/mcu -I$(PRJ_DIR)/../../../10-common/include/video1 -I$(PRJ_DIR)/../../../10-common/include/protocol -c $(PRJ_DIR)/../source/MtAdpNet.cpp
endif


## dependencies

MtAdp.o: $(PRJ_DIR)/../source/MtAdp.cpp \
	$(PRJ_DIR)/../../../10-common/include/platform/osp.h \
	$(PRJ_DIR)/../../../10-common/include/platform/brddrvlib.h \
	$(PRJ_DIR)/../../../10-common/include/system1/kdvtype.h \
	$(PRJ_DIR)/../../common/include/driagent.h \
	$(PRJ_DIR)/../../../10-common/include/protocol/kdvsys.h \
	$(PRJ_DIR)/../../../10-common/include/mcu/agtcomm.h \
	$(PRJ_DIR)/../../common/include/driconfig.h \
	$(PRJ_DIR)/../../../10-common/include/mcu/mcuconst.h \
	$(PRJ_DIR)/../../../10-common/include/system1/vccommon.h \
	$(PRJ_DIR)/../../../10-common/include/system1/kdvdef.h \
	$(PRJ_DIR)/../../common/include/mcuagtstruct.h \
	$(PRJ_DIR)/../../../10-common/include/mcu/eqpcfg.h \
	$(PRJ_DIR)/../../../10-common/include/platform/brdDrvLib.h \
	$(PRJ_DIR)/../include/MtAdpInst.h \
	$(PRJ_DIR)/../include/MtAdpCom.h \
	$(PRJ_DIR)/../../../10-common/include/mcu/mcustruct.h \
	$(PRJ_DIR)/../../../10-common/include/system1/kdvDef.h \
	$(PRJ_DIR)/../../../10-common/include/protocol/kdvh320def.h \
	$(PRJ_DIR)/../../../10-common/include/protocol/kdvh320.h \
	$(PRJ_DIR)/../../common/include/evmcumt.h \
	$(PRJ_DIR)/../../../10-common/include/system1/eventid.h

MtAdpCom.o: $(PRJ_DIR)/../source/MtAdpCom.cpp \
	$(PRJ_DIR)/../include/MtAdpCom.h \
	$(PRJ_DIR)/../../../10-common/include/mcu/mcustruct.h \
	$(PRJ_DIR)/../../../10-common/include/platform/osp.h \
	$(PRJ_DIR)/../../../10-common/include/platform/brddrvlib.h \
	$(PRJ_DIR)/../../../10-common/include/system1/kdvtype.h \
	$(PRJ_DIR)/../../../10-common/include/protocol/kdvsys.h \
	$(PRJ_DIR)/../../../10-common/include/mcu/mcuconst.h \
	$(PRJ_DIR)/../../../10-common/include/system1/vccommon.h \
	$(PRJ_DIR)/../../../10-common/include/system1/kdvdef.h \
	$(PRJ_DIR)/../../../10-common/include/system1/kdvDef.h \
	$(PRJ_DIR)/../../../10-common/include/protocol/kdvh320def.h \
	$(PRJ_DIR)/../../../10-common/include/protocol/kdvh320.h \
	$(PRJ_DIR)/../include/MtAdpInst.h

MtAdpd.o: $(PRJ_DIR)/../source/MtAdpd.cpp \
	$(PRJ_DIR)/../include/MtAdpInst.h \
	$(PRJ_DIR)/../include/MtAdpCom.h \
	$(PRJ_DIR)/../../../10-common/include/mcu/mcustruct.h \
	$(PRJ_DIR)/../../../10-common/include/platform/osp.h \
	$(PRJ_DIR)/../../../10-common/include/platform/brddrvlib.h \
	$(PRJ_DIR)/../../../10-common/include/system1/kdvtype.h \
	$(PRJ_DIR)/../../../10-common/include/protocol/kdvsys.h \
	$(PRJ_DIR)/../../../10-common/include/mcu/mcuconst.h \
	$(PRJ_DIR)/../../../10-common/include/system1/vccommon.h \
	$(PRJ_DIR)/../../../10-common/include/system1/kdvdef.h \
	$(PRJ_DIR)/../../../10-common/include/system1/kdvDef.h \
	$(PRJ_DIR)/../../../10-common/include/protocol/kdvh320def.h \
	$(PRJ_DIR)/../../../10-common/include/protocol/kdvh320.h \
	$(PRJ_DIR)/../../common/include/evmcumt.h \
	$(PRJ_DIR)/../../../10-common/include/system1/eventid.h \
	$(PRJ_DIR)/../../common/include/boardagent.h

MtAdpInst.o: $(PRJ_DIR)/../source/MtAdpInst.cpp \
	$(PRJ_DIR)/../include/MtAdpInst.h \
	$(PRJ_DIR)/../include/MtAdpCom.h \
	$(PRJ_DIR)/../../../10-common/include/mcu/mcustruct.h \
	$(PRJ_DIR)/../../../10-common/include/platform/osp.h \
	$(PRJ_DIR)/../../../10-common/include/platform/brddrvlib.h \
	$(PRJ_DIR)/../../../10-common/include/system1/kdvtype.h \
	$(PRJ_DIR)/../../../10-common/include/protocol/kdvsys.h \
	$(PRJ_DIR)/../../../10-common/include/mcu/mcuconst.h \
	$(PRJ_DIR)/../../../10-common/include/system1/vccommon.h \
	$(PRJ_DIR)/../../../10-common/include/system1/kdvdef.h \
	$(PRJ_DIR)/../../../10-common/include/system1/kdvDef.h \
	$(PRJ_DIR)/../../../10-common/include/protocol/kdvh320def.h \
	$(PRJ_DIR)/../../../10-common/include/protocol/kdvh320.h \
	$(PRJ_DIR)/../../common/include/evmcumt.h \
	$(PRJ_DIR)/../../../10-common/include/system1/eventid.h \
	$(PRJ_DIR)/../../common/include/boardagent.h \
	$(PRJ_DIR)/../include/mtadpnet.h \
	$(PRJ_DIR)/../include/mtadpcom.h \
	$(PRJ_DIR)/../../../10-common/include/protocol/kdvMediaNet.h

MtAdpNet.o: $(PRJ_DIR)/../source/MtAdpNet.cpp \
	$(PRJ_DIR)/../include/MtAdpNet.h \
	$(PRJ_DIR)/../include/mtadpcom.h \
	$(PRJ_DIR)/../../../10-common/include/mcu/mcustruct.h \
	$(PRJ_DIR)/../../../10-common/include/platform/osp.h \
	$(PRJ_DIR)/../../../10-common/include/platform/brddrvlib.h \
	$(PRJ_DIR)/../../../10-common/include/system1/kdvtype.h \
	$(PRJ_DIR)/../../../10-common/include/protocol/kdvsys.h \
	$(PRJ_DIR)/../../../10-common/include/mcu/mcuconst.h \
	$(PRJ_DIR)/../../../10-common/include/system1/vccommon.h \
	$(PRJ_DIR)/../../../10-common/include/system1/kdvdef.h \
	$(PRJ_DIR)/../../../10-common/include/system1/kdvDef.h \
	$(PRJ_DIR)/../../../10-common/include/protocol/kdvh320def.h \
	$(PRJ_DIR)/../../../10-common/include/protocol/kdvh320.h \
	$(PRJ_DIR)/../../../10-common/include/protocol/kdvMediaNet.h \
	$(PRJ_DIR)/../include/mtadpinst.h \
	$(PRJ_DIR)/../include/MtAdpCom.h \
	$(PRJ_DIR)/../../../10-common/include/video1/h261addrtpheader.h \
	$(PRJ_DIR)/../../../10-common/include/video1/h263DecRtpReform.h


## user defined rules

