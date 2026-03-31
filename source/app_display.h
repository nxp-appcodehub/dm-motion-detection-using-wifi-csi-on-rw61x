/*
 *  Copyright 2020-2024 NXP
 *  All rights reserved.
 *
 *  SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_DISPLAY_H_
#define _APP_DISPLAY_H_

enum app_display_event_t
{
    DIS_EVENT_SENSING       = 1,
    DIS_EVENT_FAR_DECT      = 1 << 1,
    DIS_EVENT_NEAR_DECT     = 1 << 2,
    DIS_EVENT_WIFI_INIT     = 1 << 3,
} ;

void lv_demo_widgets_display(uint8_t event);

#endif /* _APP_DISPLAY_H_ */
