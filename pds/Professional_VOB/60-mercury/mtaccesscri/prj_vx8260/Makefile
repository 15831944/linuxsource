# Makefile generated by the project manager
#


## core information

PRJ_FILE       = mtaccesscri.wpj
PRJ_DIR        = 
PRJ_TYPE       = vxApp
PRJ_OBJS       = mtaccesscri.o
BUILD_SPEC     = MtAccessCriRelease
TGT_DIR        = $(WIND_BASE)/target




## build-configuration info

ifeq ($(BUILD_SPEC),MtAccessCriDebug)
CPU              = PPCEC603
TOOL             = gnu
DEFAULT_RULE     = mtaccesscri.out
endif

ifeq ($(BUILD_SPEC),MtAccessCriRelease)
CPU              = PPCEC603
TOOL             = gnu
DEFAULT_RULE     = mtaccesscri.out
endif

include $(TGT_DIR)/h/make/defs.project

## build-configuration info

ifeq ($(BUILD_SPEC),MtAccessCriDebug)
AR               = arppc
AS               = ccppc
CC               = ccppc
CFLAGS           = -g -mstrict-align -ansi -nostdinc -DRW_MULTI_THREAD -D_REENTRANT -fvolatile -mlongcall -fno-builtin -fno-for-scope -msoft-float -I. -Id:/Tornado/target/h -Id:/Tornado/host/x86-win32/i386-pc-mingw32/sys-include -I$(PRJ_DIR)/../include -I$(PRJ_DIR)/../../common/include -I$(PRJ_DIR)/../../../10-Common/include/mcu -I$(PRJ_DIR)/../../../10-Common/include/system1 -I$(PRJ_DIR)/../../../10-Common/include/platform -I$(PRJ_DIR)/../../../10-Common/include/protocol -DCPU=PPCEC603 -D_DEBUG -D_VXWORKS_ -DCPU=PPCEC603
CFLAGS_AS        = -g -mstrict-align -ansi -nostdinc -fvolatile -fno-builtin -fno-for-scope -P -x assembler-with-cpp -I. -Id:/Tornado/target/h -DCPU=PPCEC603
CPP              = ccppc -E -P -xc
LD               = ldppc
LDDEPS           = 
LDFLAGS          = -X -N
LD_PARTIAL_FLAGS = -X -r
NM               = nmppc
OPTION_DEFINE_MACRO = -D
OPTION_INCLUDE_DIR = -I
POST_BUILD_RULE  = 
PRJ_LIBS         = $(PRJ_DIR)/../../../10-Common/lib/debug/8260/kdvsys.a $(PRJ_DIR)/../../../10-Common/lib/debug/8260/osp.a $(PRJ_DIR)/../../../10-Common/lib/debug/8260/mtadp.a $(PRJ_DIR)/../../../10-Common/lib/debug/8260/mp.a $(PRJ_DIR)/../../../10-Common/lib/debug/8260/mcuprs.a $(PRJ_DIR)/../../../10-Common/lib/debug/8260/kdvnetbuf.a $(PRJ_DIR)/../../../10-Common/lib/debug/8260/dataswitch.a $(PRJ_DIR)/../../../10-Common/lib/debug/8260/kdv323adp.a $(PRJ_DIR)/../../../10-Common/lib/debug/8260/kdvcascade.a $(PRJ_DIR)/../../../10-Common/lib/debug/8260/kdv323.a $(PRJ_DIR)/../../../10-Common/lib/debug/8260/kdvdatanet.a $(PRJ_DIR)/../../../10-Common/lib/debug/8260/CriAgent.a $(PRJ_DIR)/../../../10-Common/lib/debug/8260/BasicBoardAgent.a $(PRJ_DIR)/../../../10-Common/lib/debug/8260/CriDrvLib.a $(PRJ_DIR)/../../../10-Common/lib/debug/8260/kdvEncrypt.a
SIZE             = sizeppc
endif

ifeq ($(BUILD_SPEC),MtAccessCriRelease)
AR               = arppc
AS               = ccppc
CC               = ccppc
CFLAGS           = -g -mstrict-align -ansi -nostdinc -DRW_MULTI_THREAD -D_REENTRANT -fvolatile -mlongcall -fno-builtin -fno-for-scope -msoft-float -I. -Id:/Tornado/target/h -Id:/Tornado/host/x86-win32/i386-pc-mingw32/sys-include -I$(PRJ_DIR)/../include -I$(PRJ_DIR)/../../common/include -I$(PRJ_DIR)/../../../10-Common/include/mcu -I$(PRJ_DIR)/../../../10-Common/include/system1 -I$(PRJ_DIR)/../../../10-Common/include/platform -I$(PRJ_DIR)/../../../10-Common/include/protocol -DCPU=PPCEC603 -D_VXWORKS_ -DCPU=PPCEC603
CFLAGS_AS        = -g -mstrict-align -ansi -nostdinc -fvolatile -fno-builtin -fno-for-scope -P -x assembler-with-cpp -I. -Id:/Tornado/target/h -DCPU=PPCEC603
CPP              = ccppc -E -P -xc
LD               = ldppc
LDDEPS           = 
LDFLAGS          = -X -N
LD_PARTIAL_FLAGS = -X -r
NM               = nmppc
OPTION_DEFINE_MACRO = -D
OPTION_INCLUDE_DIR = -I
POST_BUILD_RULE  = 
PRJ_LIBS         = $(PRJ_DIR)/../../../10-Common/lib/release/8260/kdvsys.a $(PRJ_DIR)/../../../10-Common/lib/release/8260/osp.a $(PRJ_DIR)/../../../10-Common/lib/release/8260/mtadp.a $(PRJ_DIR)/../../../10-Common/lib/release/8260/mp.a $(PRJ_DIR)/../../../10-Common/lib/release/8260/mcuprs.a $(PRJ_DIR)/../../../10-Common/lib/release/8260/kdvnetbuf.a $(PRJ_DIR)/../../../10-Common/lib/release/8260/dataSwitch.a $(PRJ_DIR)/../../../10-Common/lib/release/8260/kdv323adp.a $(PRJ_DIR)/../../../10-Common/lib/release/8260/kdv323.a $(PRJ_DIR)/../../../10-Common/lib/release/8260/kdvcascade.a $(PRJ_DIR)/../../../10-Common/lib/release/8260/kdvdatanet.a $(PRJ_DIR)/../../../10-Common/lib/release/8260/CriAgent.a $(PRJ_DIR)/../../../10-Common/lib/release/8260/BasicBoardAgent.a $(PRJ_DIR)/../../../10-Common/lib/release/8260/CriDrvLib.a $(PRJ_DIR)/../../../10-Common/lib/release/8260/kdvEncrypt.a
SIZE             = sizeppc
endif

# override make definitions only below this line


# override make definitions only above this line
include $(TGT_DIR)/h/make/rules.project



## build-configuration info

ifeq ($(BUILD_SPEC),MtAccessCriDebug)
mtaccesscri.o:
	$(CC) -g -mstrict-align -ansi -nostdinc -DRW_MULTI_THREAD -D_REENTRANT -fvolatile -mlongcall -fno-builtin -fno-for-scope -msoft-float -I. -I$(WIND_BASE)/target/h -I$(WIND_BASE)/host/$(WIND_HOST_TYPE)/i386-pc-mingw32/sys-include -I$(PRJ_DIR)/../include -I$(PRJ_DIR)/../../common/include -I$(PRJ_DIR)/../../../10-Common/include/mcu -I$(PRJ_DIR)/../../../10-Common/include/system1 -I$(PRJ_DIR)/../../../10-Common/include/platform -I$(PRJ_DIR)/../../../10-Common/include/protocol -DCPU=PPCEC603 -D_DEBUG -D_VXWORKS_ -DCPU=PPCEC603 -c $(PRJ_DIR)/../source/mtaccesscri.cpp
endif

ifeq ($(BUILD_SPEC),MtAccessCriRelease)
mtaccesscri.o:
	$(CC) -g -mstrict-align -ansi -nostdinc -DRW_MULTI_THREAD -D_REENTRANT -fvolatile -mlongcall -fno-builtin -fno-for-scope -msoft-float -I. -I$(WIND_BASE)/target/h -I$(WIND_BASE)/host/$(WIND_HOST_TYPE)/i386-pc-mingw32/sys-include -I$(PRJ_DIR)/../include -I$(PRJ_DIR)/../../common/include -I$(PRJ_DIR)/../../../10-Common/include/mcu -I$(PRJ_DIR)/../../../10-Common/include/system1 -I$(PRJ_DIR)/../../../10-Common/include/platform -I$(PRJ_DIR)/../../../10-Common/include/protocol -DCPU=PPCEC603 -D_VXWORKS_ -DCPU=PPCEC603 -c $(PRJ_DIR)/../source/mtaccesscri.cpp
endif


## dependencies

mtaccesscri.o: $(PRJ_DIR)/../source/mtaccesscri.cpp \
	$(PRJ_DIR)/../../../10-Common/include/platform/osp.h \
	$(PRJ_DIR)/../../../10-Common/include/platform/brddrvlib.h \
	$(PRJ_DIR)/../../../10-Common/include/system1/kdvtype.h \
	$(PRJ_DIR)/../../../10-Common/include/mcu/mcustruct.h \
	$(PRJ_DIR)/../../../10-Common/include/protocol/kdvsys.h \
	$(PRJ_DIR)/../../../10-Common/include/mcu/mcuconst.h \
	$(PRJ_DIR)/../../../10-Common/include/system1/vccommon.h \
	$(PRJ_DIR)/../../../10-Common/include/system1/kdvdef.h \
	$(PRJ_DIR)/../../../10-Common/include/system1/kdvDef.h \
	$(PRJ_DIR)/../../../10-Common/include/protocol/ummessagestruct.h \
	$(PRJ_DIR)/../../../10-Common/include/protocol/umconst.h \
	$(PRJ_DIR)/../../../10-Common/include/protocol/md5.h \
	$(PRJ_DIR)/../../common/include/mcuver.h \
	$(PRJ_DIR)/../../common/include/bindmtadp.h \
	$(PRJ_DIR)/../../common/include/bindmp.h \
	$(PRJ_DIR)/../../../10-Common/include/platform/dataswitch.h \
	$(PRJ_DIR)/../../../10-Common/include/mcu/eqplib.h \
	$(PRJ_DIR)/../../../10-Common/include/mcu/eqpcfg.h \
	$(PRJ_DIR)/../../common/include/boardagent.h \
	$(PRJ_DIR)/../../common/include/cri_agent.h \
	$(PRJ_DIR)/../../common/include/cri_config.h \
	$(PRJ_DIR)/../../common/include/boardconfigbasic.h \
	$(PRJ_DIR)/../../common/include/mcuagtstruct.h \
	$(PRJ_DIR)/../../common/include/agentcommon.h \
	$(PRJ_DIR)/../../common/include/evagtsvc.h \
	$(PRJ_DIR)/../../../10-Common/include/system1/eventid.h \
	$(PRJ_DIR)/../../../10-Common/include/mcu/agtcomm.h \
	$(PRJ_DIR)/../../../10-Common/include/platform/brddrvLib.h \
	$(PRJ_DIR)/../../common/include/boardagentbasic.h \
	$(PRJ_DIR)/../../common/include/evmcumt.h \
	$(PRJ_DIR)/../../common/include/evmcu.h \
	$(PRJ_DIR)/../../common/include/evmcugk.h


## user defined rules

