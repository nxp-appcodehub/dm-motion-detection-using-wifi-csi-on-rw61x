/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

///////////////////////////////////////////////////////////////////////////////
//  Includes
///////////////////////////////////////////////////////////////////////////////

// SDK Included Files
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "wlan_bt_fw.h"
#include "wlan.h"
#include "wifi.h"
#include "wm_net.h"
#include <osa.h>
#include "dhcp-server.h"
#include "cli.h"
#include "wifi_ping.h"
#include "iperf.h"
#ifndef RW610
#include "wifi_bt_config.h"
#else
#include "fsl_power.h"
#include "fsl_ocotp.h"
#endif
#include "cli_utils.h"
#if CONFIG_HOST_SLEEP
#include "host_sleep.h"
#endif

#include "lvgl_support.h"
#include "pin_mux.h"
#include "lvgl.h"
#include "demos/lv_demos.h"

#include "fsl_inputmux.h"
#include "fsl_dma.h"
#include "fsl_gpio.h"
#include "display_support.h"

#include "fwk_platform.h"
#include "app_display.h"
#include "wlan.h"
#include "wlan_tests.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
static volatile bool s_lvgl_initialized = false;

#ifndef AP_SSID
//#define AP_SSID "iPhone"
#define AP_SSID "csi_test"
//#define AP_SSID "nxp_matter_hotspot_org"
#endif

#ifndef AP_PASSWORD
//#define AP_PASSWORD "nxp12345"
#define AP_PASSWORD "1234567890"
//#define AP_PASSWORD "Wireless123"
#endif

#ifndef AP_CHANNEL
#define AP_CHANNEL  149
#endif

#define WIFI_NETWORK_LABEL "my_wifi"

static bool sta_connected;
extern wlan_csi_config_params_t g_csi_params;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
int wlan_driver_init(void);
int wlan_driver_deinit(void);
int wlan_driver_reset(void);
int wlan_reset_cli_init(void);

extern int csi_data_recv_user(void *buffer, size_t data_len);
extern void set_csi_filter(t_u8 pkt_type, t_u8 subtype, t_u8 flags, int op_index, t_u8 *mac);
extern void set_csi_param_header(t_u8 bss_type,
                          t_u16 csi_enable,
                          t_u32 head_id,
                          t_u32 tail_id,
                          t_u8 chip_id,
                          t_u8 band_config,
                          t_u8 channel,
                          t_u8 csi_monitor_enable,
                          t_u8 ra4us);

/*******************************************************************************
 * Code
 ******************************************************************************/

#if CONFIG_WPS2
#define MAIN_TASK_STACK_SIZE 6000
#else
#define MAIN_TASK_STACK_SIZE 4096
#endif
#define DISPLAY_TASK_STACK_SIZE 4096

static void main_task(osa_task_param_t arg);
static void display_task(osa_task_param_t arg);

static OSA_TASK_DEFINE(main_task, WLAN_TASK_PRI_LOW, 1, MAIN_TASK_STACK_SIZE, 0);
static OSA_TASK_DEFINE(display_task, WLAN_TASK_PRI_NORMAL, 1, DISPLAY_TASK_STACK_SIZE, 0);

OSA_TASK_HANDLE_DEFINE(main_task_Handle);
OSA_TASK_HANDLE_DEFINE(display_task_Handle);

OSA_EVENT_HANDLE_DEFINE(display_event_Handle);

static void printSeparator(void)
{
    PRINTF("========================================\r\n");
}

/* Callback Function passed to WLAN Connection Manager. The callback function
 * gets called when there are WLAN Events that need to be handled by the
 * application.
 */
int wlan_event_callback(enum wlan_event_reason reason, void *data)
{
    int ret;
    struct wlan_ip_config addr;
    char ssid[IEEEtypes_SSID_SIZE + 1] = {0};
    char ip[16];
    static int auth_fail                      = 0;
    wlan_uap_client_disassoc_t *disassoc_resp = data;

    switch (reason)
    {
        case WLAN_REASON_INITIALIZED:
            PRINTF("app_cb: WLAN initialized\r\n");
            printSeparator();

            ret = wlan_basic_cli_init();
            if (ret != WM_SUCCESS)
            {
                PRINTF("Failed to initialize BASIC WLAN CLIs\r\n");
                return 0;
            }

            ret = wlan_cli_init();
            if (ret != WM_SUCCESS)
            {
                PRINTF("Failed to initialize WLAN CLIs\r\n");
                return 0;
            }
            PRINTF("WLAN CLIs are initialized\r\n");
            printSeparator();

            ret = wlan_enhanced_cli_init();
            if (ret != WM_SUCCESS)
            {
                PRINTF("Failed to initialize WLAN CLIs\r\n");
                return 0;
            }
            PRINTF("ENHANCED WLAN CLIs are initialized\r\n");
            printSeparator();
#ifdef RW610
#if CONFIG_HOST_SLEEP
            ret = host_sleep_cli_init();
            if (ret != WM_SUCCESS)
            {
                PRINTF("Failed to initialize WLAN CLIs\r\n");
                return 0;
            }
            PRINTF("HOST SLEEP CLIs are initialized\r\n");
            printSeparator();
#endif
#endif
            ret = ping_cli_init();
            if (ret != WM_SUCCESS)
            {
                PRINTF("Failed to initialize PING CLI\r\n");
                return 0;
            }

            ret = iperf_cli_init();
            if (ret != WM_SUCCESS)
            {
                PRINTF("Failed to initialize IPERF CLI\r\n");
                return 0;
            }

            ret = dhcpd_cli_init();
            if (ret != WM_SUCCESS)
            {
                PRINTF("Failed to initialize DHCP Server CLI\r\n");
                return 0;
            }

        #if 0
            PRINTF("CLIs Available:\r\n");
            printSeparator();
            help_command(0, NULL);
            printSeparator();
        #endif
            break;
        case WLAN_REASON_INITIALIZATION_FAILED:
            PRINTF("app_cb: WLAN: initialization failed\r\n");
            break;
        case WLAN_REASON_AUTH_SUCCESS:
            PRINTF("app_cb: WLAN: authenticated to network\r\n");
            break;
        case WLAN_REASON_SUCCESS:
            PRINTF("app_cb: WLAN: connected to network\r\n");
            ret = wlan_get_address(&addr);
            if (ret != WM_SUCCESS)
            {
                PRINTF("failed to get IP address\r\n");
                return 0;
            }

            net_inet_ntoa(addr.ipv4.address, ip);

            ret = wlan_get_current_network_ssid(ssid);
            if (ret != WM_SUCCESS)
            {
                PRINTF("Failed to get External AP network\r\n");
                return 0;
            }

            PRINTF("Connected to following BSS:\r\n");
            PRINTF("SSID = [%s]\r\n", ssid);
            if (addr.ipv4.address != 0U)
            {
                PRINTF("IPv4 Address: [%s]\r\n", ip);
                //Bob++
                (void)OSA_EventSet((osa_event_handle_t)display_event_Handle, DIS_EVENT_SENSING);
                sta_connected = true;
                //Bob++
            }
#if CONFIG_IPV6
            int i;
            for (i = 0; i < CONFIG_MAX_IPV6_ADDRESSES; i++)
            {
                if (ip6_addr_isvalid(addr.ipv6[i].addr_state))
                {
                    (void)PRINTF("IPv6 Address: %-13s:\t%s (%s)\r\n",
                                 ipv6_addr_type_to_desc((struct net_ipv6_config *)&addr.ipv6[i]),
                                 inet6_ntoa(addr.ipv6[i].address), ipv6_addr_state_to_desc(addr.ipv6[i].addr_state));
                }
            }
            (void)PRINTF("\r\n");
#endif
            auth_fail = 0;
            break;
        case WLAN_REASON_CONNECT_FAILED:
            PRINTF("app_cb: WLAN: connect failed\r\n");
            break;
        case WLAN_REASON_NETWORK_NOT_FOUND:
            PRINTF("app_cb: WLAN: network not found\r\n");
            break;
        case WLAN_REASON_NETWORK_AUTH_FAILED:
            PRINTF("app_cb: WLAN: network authentication failed\r\n");
            auth_fail++;
            if (auth_fail >= 3)
            {
                PRINTF("Authentication Failed. Disconnecting ... \r\n");
                wlan_disconnect();
                auth_fail = 0;
            }
            break;
        case WLAN_REASON_ADDRESS_SUCCESS:
            PRINTF("network mgr: DHCP new lease\r\n");
            break;
        case WLAN_REASON_ADDRESS_FAILED:
            PRINTF("app_cb: failed to obtain an IP address\r\n");
            break;
        case WLAN_REASON_USER_DISCONNECT:
            PRINTF("app_cb: disconnected\r\n");
            auth_fail = 0;
            break;
        case WLAN_REASON_LINK_LOST:
            PRINTF("app_cb: WLAN: link lost\r\n");
            break;
        case WLAN_REASON_CHAN_SWITCH:
            PRINTF("app_cb: WLAN: channel switch\r\n");
            break;
        case WLAN_REASON_UAP_SUCCESS:
            PRINTF("app_cb: WLAN: UAP Started\r\n");
            ret = wlan_get_current_uap_network_ssid(ssid);

            if (ret != WM_SUCCESS)
            {
                PRINTF("Failed to get Soft AP network\r\n");
                return 0;
            }

            printSeparator();
            PRINTF("Soft AP \"%s\" started successfully\r\n", ssid);
            printSeparator();
            if (dhcp_server_start(net_get_uap_handle()))
                PRINTF("Error in starting dhcp server\r\n");

            PRINTF("DHCP Server started successfully\r\n");
            printSeparator();
            break;
        case WLAN_REASON_UAP_CLIENT_ASSOC:
            PRINTF("app_cb: WLAN: UAP a Client Associated\r\n");
            printSeparator();
            PRINTF("Client => ");
            print_mac((const char *)data);
            PRINTF("Associated with Soft AP\r\n");
            printSeparator();
            break;
        case WLAN_REASON_UAP_CLIENT_CONN:
            PRINTF("app_cb: WLAN: UAP a Client Connected\r\n");
            printSeparator();
            PRINTF("Client => ");
            print_mac((const char *)data);
            PRINTF("Connected with Soft AP\r\n");
            printSeparator();
            break;
        case WLAN_REASON_UAP_CLIENT_DISSOC:
            printSeparator();
            PRINTF("app_cb: WLAN: UAP a Client Dissociated:");
            PRINTF(" Client MAC => ");
            print_mac((const char *)(disassoc_resp->sta_addr));
            PRINTF(" Reason code => ");
            PRINTF("%d\r\n", disassoc_resp->reason_code);
            printSeparator();
            break;
        case WLAN_REASON_UAP_STOPPED:
            PRINTF("app_cb: WLAN: UAP Stopped\r\n");
            printSeparator();
            PRINTF("Soft AP stopped successfully\r\n");
            printSeparator();

            dhcp_server_stop();

            PRINTF("DHCP Server stopped successfully\r\n");
            printSeparator();
            break;
        case WLAN_REASON_PS_ENTER:
            break;
        case WLAN_REASON_PS_EXIT:
            break;
#if CONFIG_SUBSCRIBE_EVENT_SUPPORT
        case WLAN_REASON_RSSI_HIGH:
        case WLAN_REASON_SNR_LOW:
        case WLAN_REASON_SNR_HIGH:
        case WLAN_REASON_MAX_FAIL:
        case WLAN_REASON_BEACON_MISSED:
        case WLAN_REASON_DATA_RSSI_LOW:
        case WLAN_REASON_DATA_RSSI_HIGH:
        case WLAN_REASON_DATA_SNR_LOW:
        case WLAN_REASON_DATA_SNR_HIGH:
        case WLAN_REASON_LINK_QUALITY:
        case WLAN_REASON_PRE_BEACON_LOST:
            break;
#endif
#if CONFIG_WIFI_IND_DNLD
        case WLAN_REASON_FW_HANG:
        case WLAN_REASON_FW_RESET:
            break;
#endif
        default:
            PRINTF("app_cb: WLAN: Unknown Event: %d\r\n", reason);
    }
    return 0;
}

int wlan_driver_init(void)
{
    int result = 0;

    /* Initialize WIFI Driver */
    result = wlan_init(wlan_fw_bin, wlan_fw_bin_len);

    assert(0 == result);

    result = wlan_start(wlan_event_callback);

    assert(0 == result);

    return result;
}

#ifndef RW610
int wlan_driver_deinit(void)
{
    int result = 0;

    result = wlan_stop();
    assert(0 == result);
    wlan_deinit(0);

    return result;
}

static void wlan_hw_reset(void)
{
    BOARD_WIFI_BT_Enable(false);
    OSA_TimeDelay(10);
    BOARD_WIFI_BT_Enable(true);
}

int wlan_driver_reset(void)
{
    int result = 0;

    result = wlan_driver_deinit();
    assert(0 == result);

    wlan_hw_reset();

    result = wlan_driver_init();
    assert(0 == result);

    return result;
}

static void test_wlan_reset(int argc, char **argv)
{
    (void)wlan_driver_reset();
}

#if CONFIG_HOST_SLEEP
static void test_mcu_suspend(int argc, char **argv)
{
    (void)mcu_suspend();
}
#endif

static struct cli_command reset_commands[] = {
    {"wlan-reset", NULL, test_wlan_reset},
#if CONFIG_HOST_SLEEP
    {"mcu-suspend", NULL, test_mcu_suspend},
#endif
};

int wlan_reset_cli_init(void)
{
    unsigned int i;

    for (i = 0; i < sizeof(reset_commands) / sizeof(struct cli_command); i++)
    {
        if (cli_register_command(&reset_commands[i]) != 0)
        {
            return -1;
        }
    }

    return 0;
}
#endif

static void main_task(osa_task_param_t arg)
{
    int32_t result = 0;
    (void)result;

    PRINTF("Initialize CLI\r\n");
    printSeparator();

    result = cli_init();

    assert(WM_SUCCESS == result);

#ifndef RW610
    result = wlan_reset_cli_init();

    assert(WM_SUCCESS == result);
#endif

#if CONFIG_HOST_SLEEP
#ifndef RW610
    hostsleep_init(wlan_hs_pre_cfg, wlan_hs_post_cfg);
#else
    hostsleep_init();
#endif
#endif

    PRINTF("Initialize WLAN Driver\r\n");
    printSeparator();

    /* Initialize WIFI Driver */
    result = wlan_driver_init();

    assert(WM_SUCCESS == result);

#ifndef RW610
    result = wlan_reset_cli_init();

    assert(WM_SUCCESS == result);
#endif

    (void)OSA_EventSet((osa_event_handle_t)display_event_Handle, DIS_EVENT_WIFI_INIT);

    OSA_TimeDelay(2000);

    /* Add and connect Wi-Fi network */
#if 1
    {
        struct wlan_network network;
        int ret;
        t_u8 bss_type           = 0;
        t_u16 csi_enable        = 0;
        t_u32 head_id           = 0;
        t_u32 tail_id           = 0;
        t_u8 chip_id            = 0;
        t_u8 band_config        = 0;
        t_u8 channel            = 0;
        t_u8 csi_monitor_enable = 0;
        t_u8 ra4us              = 0;
        t_u8 raw_mac[MLAN_MAC_ADDR_LENGTH];
        t_u8 pkt_type = 0;
        t_u8 subtype  = 0;
        t_u8 flags    = 0;
        int op_index  = 0;
        struct wlan_network cur_network;

        (void)memset(&network, 0, sizeof(struct wlan_network));

        (void)memcpy(network.name, WIFI_NETWORK_LABEL, strlen(WIFI_NETWORK_LABEL));
        network.role = WLAN_BSS_ROLE_STA;
        network.ip.ipv4.addr_type = ADDR_TYPE_DHCP;
        network.channel = AP_CHANNEL;
        network.security.mfpc = -1;
        network.security.mfpr = -1;
        network.security.type = WLAN_SECURITY_WPA2;
        network.security.key_mgmt |= WLAN_KEY_MGMT_PSK;
        (void)memcpy(network.ssid, AP_SSID, strlen(AP_SSID));
        (void)strcpy(network.security.psk, AP_PASSWORD);
        network.security.psk_len = strlen(AP_PASSWORD);
        ret  = wlan_add_network(&network);
        if(!ret)
            (void)PRINTF("Added network \"%s\"\r\n", network.name);
        else    
            (void)PRINTF("Fail to add \"%s\"\r\n", network.name);
        
        ret = wlan_connect(WIFI_NETWORK_LABEL);
        if(ret)
            (void)PRINTF("Fail to connect network \"%s\"\r\n", WIFI_NETWORK_LABEL);
        else
            (void)PRINTF("Connecting network \"%s\"\r\n", network.name);

        while (!sta_connected)
        {
            OSA_TimeDelay(1000);
        }

        /* Config CSI */
        ret = wlan_register_csi_user_callback(csi_data_recv_user);
        // ret = wlan_register_csi_user_callback(wifi_process_csi_data);
        if (ret != WM_SUCCESS)
        {
            PRINTF("Error during register csi user callback\r\n");
        }

        bss_type           = 0; //STA mode
        csi_enable         = 1;
        head_id            = 66051;
        tail_id            = 66051;
        chip_id            = 170;
        //band_config        = 1;
        //channel            = AP_CHANNEL;
        csi_monitor_enable = 1;
        ra4us              = 1;
        //memcpy((void *)&g_csi_params, (void *)wlan_get_csi_cfg_param_default(), sizeof(wlan_csi_config_params_t));
        //set_csi_param_header(bss_type, csi_enable, head_id, tail_id, chip_id, band_config, channel, csi_monitor_enable,
        //                     ra4us);
        //wlan_set_csi_cfg_param_default(&g_csi_params);

        ret = wlan_get_current_network(&cur_network);
        if (ret == WM_SUCCESS)
        {
            memcpy(raw_mac, cur_network.bssid, IEEEtypes_ADDRESS_SIZE);
            channel = cur_network.channel;
            if (channel <= 14)
                band_config = 0;
            else
                band_config = 1;
        }
        else
            (void)PRINTF("Failed to get AP MAC\r\n");

        memcpy((void *)&g_csi_params, (void *)wlan_get_csi_cfg_param_default(), sizeof(wlan_csi_config_params_t));
        set_csi_param_header(bss_type, csi_enable, head_id, tail_id, chip_id, band_config, channel, csi_monitor_enable, ra4us);
        wlan_set_csi_cfg_param_default(&g_csi_params);

        pkt_type = 255;
        subtype  = 8;
        flags    = 0;
        op_index = CSI_FILTER_OPT_ADD;
        memcpy((void *)&g_csi_params, (void *)wlan_get_csi_cfg_param_default(), sizeof(wlan_csi_config_params_t));
        set_csi_filter(pkt_type, subtype, flags, op_index, raw_mac);
        wlan_set_csi_cfg_param_default(&g_csi_params);

        ret = wlan_csi_cfg(&g_csi_params);
        if (ret != WM_SUCCESS)
        {
            (void)PRINTF("Failed to send csi cfg\r\n");
        }
        else
            (void)PRINTF("Wi-Fi CSI is enabled\r\n");
    }
#endif

    while (1)
    {
        /* wait for interface up */
        //(void)OSA_EventSet((osa_event_handle_t)display_event_Handle, DIS_EVENT_SENSING);
        //OSA_TimeDelay(2000);
        //(void)OSA_EventSet((osa_event_handle_t)display_event_Handle, DIS_EVENT_FAR_DECT);
        //OSA_TimeDelay(1000);
        //(void)OSA_EventSet((osa_event_handle_t)display_event_Handle, DIS_EVENT_NEAR_DECT);
        OSA_TimeDelay(5000);
    }
}

#if LV_USE_LOG
static void print_cb(const char *buf)
{
    PRINTF("\r%s\n", buf);
}
#endif

static void AppTask(void *param)
{
#if LV_USE_LOG
    lv_log_register_print_cb(print_cb);
#endif

    PRINTF("lvgl widgets demo started\r\n");

    lv_port_pre_init();
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();

    s_lvgl_initialized = true;

    lv_demo_widgets();
    //(void)OSA_EventSet((osa_event_handle_t)display_event_Handle, DIS_EVENT_WIFI_INIT);

    for (;;)
    {
        lv_task_handler();
        //vTaskDelay(5);
        vTaskDelay(100);
    }
}

static void display_task(osa_task_param_t arg)
{
    osa_event_flags_t flagsToWait = DIS_EVENT_SENSING | DIS_EVENT_FAR_DECT | DIS_EVENT_NEAR_DECT | DIS_EVENT_WIFI_INIT;
    osa_event_flags_t pSetFlags;

    for (;;)
    {
        (void)OSA_EventWait((osa_event_handle_t)display_event_Handle, flagsToWait, false, osaWaitForever_c,
                                &pSetFlags);

        if (pSetFlags & DIS_EVENT_SENSING)
        {
            PRINTF("Display Sensing\r\n");
            lv_demo_widgets_display(DIS_EVENT_SENSING);
            //Keep for 2s
            OSA_TimeDelay(10000);
            OSA_EventClear((osa_event_handle_t)display_event_Handle, DIS_EVENT_SENSING);
        }

        if (pSetFlags & DIS_EVENT_NEAR_DECT)
        {
            PRINTF("Display Near Detection\r\n");
            lv_demo_widgets_display(DIS_EVENT_NEAR_DECT);
            //Keep for 10s
            OSA_TimeDelay(10000);
            OSA_EventClear((osa_event_handle_t)display_event_Handle, DIS_EVENT_NEAR_DECT);
        }

        if (pSetFlags & DIS_EVENT_FAR_DECT)
        {
            PRINTF("Display Far Detection\r\n");
            OSA_EventClear((osa_event_handle_t)display_event_Handle, DIS_EVENT_FAR_DECT);
            lv_demo_widgets_display(DIS_EVENT_FAR_DECT);
        }

        if (pSetFlags & DIS_EVENT_WIFI_INIT)
        {
            PRINTF("Display Wi-Fi Init\r\n");
            OSA_EventClear((osa_event_handle_t)display_event_Handle, DIS_EVENT_WIFI_INIT);
            lv_demo_widgets_display(DIS_EVENT_WIFI_INIT);
        }
    }
}

void display_csi_sensing(uint8_t state)
{
    switch(state)
    {
        case DIS_EVENT_NEAR_DECT:
            (void)OSA_EventSet((osa_event_handle_t)display_event_Handle, DIS_EVENT_NEAR_DECT);
            break;
        case DIS_EVENT_FAR_DECT:
            (void)OSA_EventSet((osa_event_handle_t)display_event_Handle, DIS_EVENT_FAR_DECT);
            break;
        case DIS_EVENT_SENSING:
            (void)OSA_EventSet((osa_event_handle_t)display_event_Handle, DIS_EVENT_SENSING);
            break;
        default:
            break;
    }
}

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
int main(void)
{
	BaseType_t stat;
    osa_status_t status;

    OSA_Init();

    BOARD_InitBootPins();
    if (BOARD_IS_XIP())
    {
        BOARD_BootClockLPR();
        CLOCK_EnableClock(kCLOCK_Otp);
        CLOCK_EnableClock(kCLOCK_Els);
        CLOCK_EnableClock(kCLOCK_ElsApb);
        RESET_PeripheralReset(kOTP_RST_SHIFT_RSTn);
        RESET_PeripheralReset(kELS_APB_RST_SHIFT_RSTn);
    }
    else
    {
        BOARD_InitBootClocks();
    }
    BOARD_InitDebugConsole();
    /* Reset GMDA */
    RESET_PeripheralReset(kGDMA_RST_SHIFT_RSTn);
    /* Keep CAU sleep clock here. */
    /* CPU1 uses Internal clock when in low power mode. */
    POWER_ConfigCauInSleep(false);
    BOARD_InitSleepPinConfig();
#ifdef RW610
    POWER_PowerOffBle();
#endif

    //LCD Init
#if (DEMO_PANEL == DEMO_PANEL_LCD_PAR_S035)
    BOARD_InitLcd1Pins();

    /* LCDIC clock. */
    CLOCK_AttachClk(kMAIN_CLK_to_LCD_CLK);
    CLOCK_SetClkDiv(kCLOCK_DivLcdClk, 13);
    RESET_PeripheralReset(kLCDIC_RST_SHIFT_RSTn);
#else
    BOARD_InitLcd0Pins();

    /* SPI clock. */
    CLOCK_AttachClk(kSFRO_to_FLEXCOMM1);
    RESET_PeripheralReset(kFC1_RST_SHIFT_RSTn);
#endif

    /* Use 16 MHz clock for the FLEXCOMM2 */
    CLOCK_AttachClk(kSFRO_to_FLEXCOMM2);

    /* DMA */
    DMA_Init(DMA0);
    /* GPIO. */
    GPIO_PortInit(GPIO, 0);
    GPIO_PortInit(GPIO, 1);

    /* inputmux */
    INPUTMUX_Init(INPUTMUX);
    RESET_PeripheralReset(kINPUTMUX_RST_SHIFT_RSTn);

    PLATFORM_InitTimerManager();

    printSeparator();
    PRINTF("wifi sensing demo\r\n");
    printSeparator();

    (void)OSA_TaskCreate((osa_task_handle_t)main_task_Handle, OSA_TASK(main_task), NULL);

    //Start LCD task
    stat = xTaskCreate(AppTask, "lvgl", configMINIMAL_STACK_SIZE + 800, NULL, tskIDLE_PRIORITY + 2, NULL);
    if (pdPASS != stat)
    {
    	PRINTF("Failed to create lvgl task");
        while (1)
        	;
    }

    (void)OSA_TaskCreate((osa_task_handle_t)display_task_Handle, OSA_TASK(display_task), NULL);
    status = OSA_EventCreate((osa_event_handle_t)display_event_Handle, 1);
    if (status != KOSA_StatusSuccess)
    {
        PRINTF("Create event handle failed");
        while (1)
        	;
    }

    OSA_Start();

    return 0;
}

/*!
 * @brief Malloc failed hook.
 */
void vApplicationMallocFailedHook(void)
{
    PRINTF("Malloc failed. Increase the heap size.");

    for (;;)
        ;
}

/*!
 * @brief FreeRTOS tick hook.
 */
//void vApplicationTickHook(void)
void Lvgl_ApplicationTickHook(void)
{
    if (s_lvgl_initialized)
    {
        lv_tick_inc(1);
    }
}

/*!
 * @brief Stack overflow hook.
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)pcTaskName;
    (void)xTask;

    for (;;)
        ;
}
