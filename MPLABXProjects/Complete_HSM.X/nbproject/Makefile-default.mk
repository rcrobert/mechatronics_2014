#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Complete_HSM.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Complete_HSM.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../AD.c ../BOARD.c ../BotConfig.c ../DummyEventChecker.c ../EventCheckerService.c ../IO_Ports.c ../MotorDriver.c ../pwm.c ../serial.c TopHSM.c ExitHSM.c ApproachHSM.c SearchHSM.c ../ES_Framework.c ReturnHSM.c RamSubHSM.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1472/AD.o ${OBJECTDIR}/_ext/1472/BOARD.o ${OBJECTDIR}/_ext/1472/BotConfig.o ${OBJECTDIR}/_ext/1472/DummyEventChecker.o ${OBJECTDIR}/_ext/1472/EventCheckerService.o ${OBJECTDIR}/_ext/1472/IO_Ports.o ${OBJECTDIR}/_ext/1472/MotorDriver.o ${OBJECTDIR}/_ext/1472/pwm.o ${OBJECTDIR}/_ext/1472/serial.o ${OBJECTDIR}/TopHSM.o ${OBJECTDIR}/ExitHSM.o ${OBJECTDIR}/ApproachHSM.o ${OBJECTDIR}/SearchHSM.o ${OBJECTDIR}/_ext/1472/ES_Framework.o ${OBJECTDIR}/ReturnHSM.o ${OBJECTDIR}/RamSubHSM.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1472/AD.o.d ${OBJECTDIR}/_ext/1472/BOARD.o.d ${OBJECTDIR}/_ext/1472/BotConfig.o.d ${OBJECTDIR}/_ext/1472/DummyEventChecker.o.d ${OBJECTDIR}/_ext/1472/EventCheckerService.o.d ${OBJECTDIR}/_ext/1472/IO_Ports.o.d ${OBJECTDIR}/_ext/1472/MotorDriver.o.d ${OBJECTDIR}/_ext/1472/pwm.o.d ${OBJECTDIR}/_ext/1472/serial.o.d ${OBJECTDIR}/TopHSM.o.d ${OBJECTDIR}/ExitHSM.o.d ${OBJECTDIR}/ApproachHSM.o.d ${OBJECTDIR}/SearchHSM.o.d ${OBJECTDIR}/_ext/1472/ES_Framework.o.d ${OBJECTDIR}/ReturnHSM.o.d ${OBJECTDIR}/RamSubHSM.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1472/AD.o ${OBJECTDIR}/_ext/1472/BOARD.o ${OBJECTDIR}/_ext/1472/BotConfig.o ${OBJECTDIR}/_ext/1472/DummyEventChecker.o ${OBJECTDIR}/_ext/1472/EventCheckerService.o ${OBJECTDIR}/_ext/1472/IO_Ports.o ${OBJECTDIR}/_ext/1472/MotorDriver.o ${OBJECTDIR}/_ext/1472/pwm.o ${OBJECTDIR}/_ext/1472/serial.o ${OBJECTDIR}/TopHSM.o ${OBJECTDIR}/ExitHSM.o ${OBJECTDIR}/ApproachHSM.o ${OBJECTDIR}/SearchHSM.o ${OBJECTDIR}/_ext/1472/ES_Framework.o ${OBJECTDIR}/ReturnHSM.o ${OBJECTDIR}/RamSubHSM.o

# Source Files
SOURCEFILES=../AD.c ../BOARD.c ../BotConfig.c ../DummyEventChecker.c ../EventCheckerService.c ../IO_Ports.c ../MotorDriver.c ../pwm.c ../serial.c TopHSM.c ExitHSM.c ApproachHSM.c SearchHSM.c ../ES_Framework.c ReturnHSM.c RamSubHSM.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
	${MAKE} ${MAKE_OPTIONS} -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/Complete_HSM.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=32MX320F128H
MP_LINKER_FILE_OPTION=
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1472/AD.o: ../AD.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/AD.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/AD.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/AD.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/_ext/1472/AD.o.d" -o ${OBJECTDIR}/_ext/1472/AD.o ../AD.c   
	
${OBJECTDIR}/_ext/1472/BOARD.o: ../BOARD.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/BOARD.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/BOARD.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/BOARD.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/_ext/1472/BOARD.o.d" -o ${OBJECTDIR}/_ext/1472/BOARD.o ../BOARD.c   
	
${OBJECTDIR}/_ext/1472/BotConfig.o: ../BotConfig.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/BotConfig.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/BotConfig.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/BotConfig.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/_ext/1472/BotConfig.o.d" -o ${OBJECTDIR}/_ext/1472/BotConfig.o ../BotConfig.c   
	
${OBJECTDIR}/_ext/1472/DummyEventChecker.o: ../DummyEventChecker.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/DummyEventChecker.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/DummyEventChecker.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/DummyEventChecker.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/_ext/1472/DummyEventChecker.o.d" -o ${OBJECTDIR}/_ext/1472/DummyEventChecker.o ../DummyEventChecker.c   
	
${OBJECTDIR}/_ext/1472/EventCheckerService.o: ../EventCheckerService.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/EventCheckerService.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/EventCheckerService.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/EventCheckerService.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/_ext/1472/EventCheckerService.o.d" -o ${OBJECTDIR}/_ext/1472/EventCheckerService.o ../EventCheckerService.c   
	
${OBJECTDIR}/_ext/1472/IO_Ports.o: ../IO_Ports.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/IO_Ports.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/IO_Ports.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/IO_Ports.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/_ext/1472/IO_Ports.o.d" -o ${OBJECTDIR}/_ext/1472/IO_Ports.o ../IO_Ports.c   
	
${OBJECTDIR}/_ext/1472/MotorDriver.o: ../MotorDriver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/MotorDriver.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/MotorDriver.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/MotorDriver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/_ext/1472/MotorDriver.o.d" -o ${OBJECTDIR}/_ext/1472/MotorDriver.o ../MotorDriver.c   
	
${OBJECTDIR}/_ext/1472/pwm.o: ../pwm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/pwm.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/pwm.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/pwm.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/_ext/1472/pwm.o.d" -o ${OBJECTDIR}/_ext/1472/pwm.o ../pwm.c   
	
${OBJECTDIR}/_ext/1472/serial.o: ../serial.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/serial.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/serial.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/serial.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/_ext/1472/serial.o.d" -o ${OBJECTDIR}/_ext/1472/serial.o ../serial.c   
	
${OBJECTDIR}/TopHSM.o: TopHSM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/TopHSM.o.d 
	@${RM} ${OBJECTDIR}/TopHSM.o 
	@${FIXDEPS} "${OBJECTDIR}/TopHSM.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/TopHSM.o.d" -o ${OBJECTDIR}/TopHSM.o TopHSM.c   
	
${OBJECTDIR}/ExitHSM.o: ExitHSM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/ExitHSM.o.d 
	@${RM} ${OBJECTDIR}/ExitHSM.o 
	@${FIXDEPS} "${OBJECTDIR}/ExitHSM.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/ExitHSM.o.d" -o ${OBJECTDIR}/ExitHSM.o ExitHSM.c   
	
${OBJECTDIR}/ApproachHSM.o: ApproachHSM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/ApproachHSM.o.d 
	@${RM} ${OBJECTDIR}/ApproachHSM.o 
	@${FIXDEPS} "${OBJECTDIR}/ApproachHSM.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/ApproachHSM.o.d" -o ${OBJECTDIR}/ApproachHSM.o ApproachHSM.c   
	
${OBJECTDIR}/SearchHSM.o: SearchHSM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/SearchHSM.o.d 
	@${RM} ${OBJECTDIR}/SearchHSM.o 
	@${FIXDEPS} "${OBJECTDIR}/SearchHSM.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/SearchHSM.o.d" -o ${OBJECTDIR}/SearchHSM.o SearchHSM.c   
	
${OBJECTDIR}/_ext/1472/ES_Framework.o: ../ES_Framework.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/ES_Framework.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/ES_Framework.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/ES_Framework.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/_ext/1472/ES_Framework.o.d" -o ${OBJECTDIR}/_ext/1472/ES_Framework.o ../ES_Framework.c   
	
${OBJECTDIR}/ReturnHSM.o: ReturnHSM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/ReturnHSM.o.d 
	@${RM} ${OBJECTDIR}/ReturnHSM.o 
	@${FIXDEPS} "${OBJECTDIR}/ReturnHSM.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/ReturnHSM.o.d" -o ${OBJECTDIR}/ReturnHSM.o ReturnHSM.c   
	
${OBJECTDIR}/RamSubHSM.o: RamSubHSM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/RamSubHSM.o.d 
	@${RM} ${OBJECTDIR}/RamSubHSM.o 
	@${FIXDEPS} "${OBJECTDIR}/RamSubHSM.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/RamSubHSM.o.d" -o ${OBJECTDIR}/RamSubHSM.o RamSubHSM.c   
	
else
${OBJECTDIR}/_ext/1472/AD.o: ../AD.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/AD.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/AD.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/AD.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/_ext/1472/AD.o.d" -o ${OBJECTDIR}/_ext/1472/AD.o ../AD.c   
	
${OBJECTDIR}/_ext/1472/BOARD.o: ../BOARD.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/BOARD.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/BOARD.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/BOARD.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/_ext/1472/BOARD.o.d" -o ${OBJECTDIR}/_ext/1472/BOARD.o ../BOARD.c   
	
${OBJECTDIR}/_ext/1472/BotConfig.o: ../BotConfig.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/BotConfig.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/BotConfig.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/BotConfig.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/_ext/1472/BotConfig.o.d" -o ${OBJECTDIR}/_ext/1472/BotConfig.o ../BotConfig.c   
	
${OBJECTDIR}/_ext/1472/DummyEventChecker.o: ../DummyEventChecker.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/DummyEventChecker.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/DummyEventChecker.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/DummyEventChecker.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/_ext/1472/DummyEventChecker.o.d" -o ${OBJECTDIR}/_ext/1472/DummyEventChecker.o ../DummyEventChecker.c   
	
${OBJECTDIR}/_ext/1472/EventCheckerService.o: ../EventCheckerService.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/EventCheckerService.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/EventCheckerService.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/EventCheckerService.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/_ext/1472/EventCheckerService.o.d" -o ${OBJECTDIR}/_ext/1472/EventCheckerService.o ../EventCheckerService.c   
	
${OBJECTDIR}/_ext/1472/IO_Ports.o: ../IO_Ports.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/IO_Ports.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/IO_Ports.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/IO_Ports.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/_ext/1472/IO_Ports.o.d" -o ${OBJECTDIR}/_ext/1472/IO_Ports.o ../IO_Ports.c   
	
${OBJECTDIR}/_ext/1472/MotorDriver.o: ../MotorDriver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/MotorDriver.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/MotorDriver.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/MotorDriver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/_ext/1472/MotorDriver.o.d" -o ${OBJECTDIR}/_ext/1472/MotorDriver.o ../MotorDriver.c   
	
${OBJECTDIR}/_ext/1472/pwm.o: ../pwm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/pwm.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/pwm.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/pwm.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/_ext/1472/pwm.o.d" -o ${OBJECTDIR}/_ext/1472/pwm.o ../pwm.c   
	
${OBJECTDIR}/_ext/1472/serial.o: ../serial.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/serial.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/serial.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/serial.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/_ext/1472/serial.o.d" -o ${OBJECTDIR}/_ext/1472/serial.o ../serial.c   
	
${OBJECTDIR}/TopHSM.o: TopHSM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/TopHSM.o.d 
	@${RM} ${OBJECTDIR}/TopHSM.o 
	@${FIXDEPS} "${OBJECTDIR}/TopHSM.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/TopHSM.o.d" -o ${OBJECTDIR}/TopHSM.o TopHSM.c   
	
${OBJECTDIR}/ExitHSM.o: ExitHSM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/ExitHSM.o.d 
	@${RM} ${OBJECTDIR}/ExitHSM.o 
	@${FIXDEPS} "${OBJECTDIR}/ExitHSM.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/ExitHSM.o.d" -o ${OBJECTDIR}/ExitHSM.o ExitHSM.c   
	
${OBJECTDIR}/ApproachHSM.o: ApproachHSM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/ApproachHSM.o.d 
	@${RM} ${OBJECTDIR}/ApproachHSM.o 
	@${FIXDEPS} "${OBJECTDIR}/ApproachHSM.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/ApproachHSM.o.d" -o ${OBJECTDIR}/ApproachHSM.o ApproachHSM.c   
	
${OBJECTDIR}/SearchHSM.o: SearchHSM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/SearchHSM.o.d 
	@${RM} ${OBJECTDIR}/SearchHSM.o 
	@${FIXDEPS} "${OBJECTDIR}/SearchHSM.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/SearchHSM.o.d" -o ${OBJECTDIR}/SearchHSM.o SearchHSM.c   
	
${OBJECTDIR}/_ext/1472/ES_Framework.o: ../ES_Framework.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/ES_Framework.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/ES_Framework.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/ES_Framework.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/_ext/1472/ES_Framework.o.d" -o ${OBJECTDIR}/_ext/1472/ES_Framework.o ../ES_Framework.c   
	
${OBJECTDIR}/ReturnHSM.o: ReturnHSM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/ReturnHSM.o.d 
	@${RM} ${OBJECTDIR}/ReturnHSM.o 
	@${FIXDEPS} "${OBJECTDIR}/ReturnHSM.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/ReturnHSM.o.d" -o ${OBJECTDIR}/ReturnHSM.o ReturnHSM.c   
	
${OBJECTDIR}/RamSubHSM.o: RamSubHSM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/RamSubHSM.o.d 
	@${RM} ${OBJECTDIR}/RamSubHSM.o 
	@${FIXDEPS} "${OBJECTDIR}/RamSubHSM.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DTOPHSM_TEST -I"." -I"../" -I"C:/CMPE118/include" -MMD -MF "${OBJECTDIR}/RamSubHSM.o.d" -o ${OBJECTDIR}/RamSubHSM.o RamSubHSM.c   
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/Complete_HSM.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mdebugger -D__MPLAB_DEBUGGER_PK3=1 -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/Complete_HSM.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}           -mreserve=data@0x0:0x1FC -mreserve=boot@0x1FC02000:0x1FC02FEF -mreserve=boot@0x1FC02000:0x1FC024FF  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map"
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/Complete_HSM.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/Complete_HSM.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map"
	${MP_CC_DIR}\\xc32-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/Complete_HSM.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
