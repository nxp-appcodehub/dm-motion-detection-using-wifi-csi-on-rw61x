if(NOT DEFINED FPU)
	set(FPU "-mfpu=fpv5-sp-d16 -mfloat-abi=hard")
endif()

if(${LIBRARY_TYPE} STREQUAL "REDLIB")
	set(SPECS "-specs=redlib.specs")
elseif(${LIBRARY_TYPE} STREQUAL "NEWLIB_NANO")
	set(SPECS "--specs=nano.specs")
endif()

if(NOT DEFINED DEBUG_CONSOLE_CONFIG)
	set(DEBUG_CONSOLE_CONFIG "-DSDK_DEBUGCONSOLE=1")
endif()

set(CMAKE_ASM_FLAGS_DEBUG " \
    ${CMAKE_ASM_FLAGS_DEBUG} \
    ${FPU} \
    -mcpu=cortex-m33+nodsp \
    -mthumb \
")

set(CMAKE_C_FLAGS_DEBUG " \
    ${CMAKE_C_FLAGS_DEBUG} \
    ${FPU} \
    ${DEBUG_CONSOLE_CONFIG} \
    -std=gnu99 \
    -DFSL_SDK_DRIVER_QUICK_ACCESS_ENABLE=1 \
    -DCONFIG_MONOLITHIC_WIFI=1 \
    -DUSE_RTOS=1 \
    -DPRINTF_ADVANCED_ENABLE=1 \
    -DTIMER_PORT_TYPE_OSTIMER \
    -DCPU_RW612ETA2I \
    -DMCUXPRESSO_SDK \
    -DBOOT_HEADER_ENABLE=1 \
    -DFSL_OSA_TASK_ENABLE=1 \
    -DSDK_OS_FREE_RTOS \
    -DSO_REUSE=1 \
    -DOSA_USED \
    -DLWIP_DNS=1 \
    -DLWIP_NETIF_HOSTNAME=1 \
    -DCONFIG_NXP_WIFI_SOFTAP_SUPPORT=1 \
    -DLWIP_IGMP=1 \
    -D_XOPEN_SOURCE=500 \
    -DMFLASH_FILE_BASEADDR=7340032 \
    -DGENERIC_LIST_LIGHT=1 \
    -DSERIAL_PORT_TYPE_UART=1 \
    -DCPU_RW612ETA2I_cm33_nodsp \
    -DWIFI_BOARD_FRDM_RW61X \
    -DCR_INTEGER_PRINTF \
    -D__MCUXPRESSO \
    -D__USE_CMSIS \
    -DDEBUG \
    -DMCUX_DBI_LEGACY=0 \
    -DILI9341_USE_DBI_IFACE=1 \
    -DFT6X06_USE_CMSIS_DRIVER=0 \
    -O0 \
    -fno-common \
    -fmerge-constants \
    -g3 \
    -mcpu=cortex-m33+nodsp -ffunction-sections -fdata-sections -fno-builtin -imacros \"${ProjDirPath}/source/wifi_config.h\" \
    -fstack-usage \
    -mcpu=cortex-m33+nodsp \
    -mthumb \
")

set(CMAKE_CXX_FLAGS_DEBUG " \
    ${CMAKE_CXX_FLAGS_DEBUG} \
    ${FPU} \
    ${DEBUG_CONSOLE_CONFIG} \
    -O0 \
    -fno-common \
    -fmerge-constants \
    -g3 \
    -Wall \
    -fstack-usage \
    -mcpu=cortex-m33+nodsp \
    -mthumb \
")

set(CMAKE_EXE_LINKER_FLAGS_DEBUG " \
    ${CMAKE_EXE_LINKER_FLAGS_DEBUG} \
    ${FPU} \
    ${SPECS} \
    -nostdlib \
    -Xlinker \
    -no-warn-rwx-segments \
    -Xlinker \
    -Map=output.map \
    -Xlinker \
    --gc-sections \
    -Xlinker \
    -print-memory-usage \
    -Xlinker \
    --sort-section=alignment \
    -Xlinker \
    --cref \
    -mcpu=cortex-m33+nodsp \
    -mthumb \
    -T\"${ProjDirPath}/frdmrw612_wifi_sensing_Debug.ld\" \
")

set(CMAKE_ASM_FLAGS_RELEASE " \
    ${CMAKE_ASM_FLAGS_RELEASE} \
    ${FPU} \
    -mcpu=cortex-m33+nodsp \
    -mthumb \
")

set(CMAKE_C_FLAGS_RELEASE " \
    ${CMAKE_C_FLAGS_RELEASE} \
    ${FPU} \
    ${DEBUG_CONSOLE_CONFIG} \
    -std=gnu99 \
    -DFSL_SDK_DRIVER_QUICK_ACCESS_ENABLE=1 \
    -DCONFIG_MONOLITHIC_WIFI=1 \
    -DUSE_RTOS=1 \
    -DPRINTF_ADVANCED_ENABLE=1 \
    -DCPU_RW612ETA2I \
    -DFSL_SDK_DRIVER_QUICK_ACCESS_ENABLE=1 \
    -DCONFIG_MONOLITHIC_WIFI=1 \
    -DUSE_RTOS=1 \
    -DPRINTF_ADVANCED_ENABLE=1 \
    -DTIMER_PORT_TYPE_OSTIMER \
    -DCPU_RW612ETA2I \
    -DMCUXPRESSO_SDK \
    -DBOOT_HEADER_ENABLE=1 \
    -DFSL_OSA_TASK_ENABLE=1 \
    -DSDK_OS_FREE_RTOS \
    -DSO_REUSE=1 \
    -DOSA_USED \
    -DLWIP_DNS=1 \
    -DLWIP_NETIF_HOSTNAME=1 \
    -DCONFIG_NXP_WIFI_SOFTAP_SUPPORT=1 \
    -DLWIP_IGMP=1 \
    -D_XOPEN_SOURCE=500 \
    -DMFLASH_FILE_BASEADDR=7340032 \
    -DGENERIC_LIST_LIGHT=1 \
    -DSERIAL_PORT_TYPE_UART=1 \
    -DCPU_RW612ETA2I_cm33_nodsp \
    -DWIFI_BOARD_FRDM_RW61X \
    -DCR_INTEGER_PRINTF \
    -D__MCUXPRESSO \
    -D__USE_CMSIS \
    -DDEBUG \
    -DMCUX_DBI_LEGACY=0 \
    -DILI9341_USE_DBI_IFACE=1 \
    -DFT6X06_USE_CMSIS_DRIVER=0 \
    -DMCUXPRESSO_SDK \
    -DBOOT_HEADER_ENABLE=1 \
    -DFSL_OSA_TASK_ENABLE=1 \
    -DSDK_OS_FREE_RTOS \
    -DSO_REUSE=1 \
    -DOSA_USED \
    -DLWIP_DNS=1 \
    -DLWIP_NETIF_HOSTNAME=1 \
    -DLWIP_IGMP=1 \
    -D_XOPEN_SOURCE=500 \
    -DMFLASH_FILE_BASEADDR=7340032 \
    -DGENERIC_LIST_LIGHT=1 \
    -DSERIAL_PORT_TYPE_UART=1 \
    -DCPU_RW612ETA2I_cm33_nodsp \
    -DCR_INTEGER_PRINTF \
    -D__MCUXPRESSO \
    -D__USE_CMSIS \
    -DNDEBUG \
    -DMCUX_DBI_LEGACY=0 \
    -DILI9341_USE_DBI_IFACE=1 \
    -Os \
    -fno-common \
    -fmerge-constants \
    -mcpu=cortex-m33+nodsp -ffunction-sections -fdata-sections -fno-builtin -imacros \"${ProjDirPath}/source/wifi_config.h\" \
    -fstack-usage \
    -mcpu=cortex-m33+nodsp \
    -mthumb \
")

set(CMAKE_CXX_FLAGS_RELEASE " \
    ${CMAKE_CXX_FLAGS_RELEASE} \
    ${FPU} \
    ${DEBUG_CONSOLE_CONFIG} \
    -fno-common \
    -fmerge-constants \
    -Wall \
    -fstack-usage \
    -mcpu=cortex-m33+nodsp \
    -mthumb \
")

set(CMAKE_EXE_LINKER_FLAGS_RELEASE " \
    ${CMAKE_EXE_LINKER_FLAGS_RELEASE} \
    ${FPU} \
    ${SPECS} \
    -nostdlib \
    -Xlinker \
    -no-warn-rwx-segments \
    -Xlinker \
    -Map=output.map \
    -Xlinker \
    --gc-sections \
    -Xlinker \
    -print-memory-usage \
    -Xlinker \
    --sort-section=alignment \
    -Xlinker \
    --cref \
    -mcpu=cortex-m33+nodsp \
    -mthumb \
    -T\"${ProjDirPath}/frdmrw612_wifi_sensing_Release.ld\" \
")
