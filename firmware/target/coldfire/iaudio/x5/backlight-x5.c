/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2006 by Linus Nielsen Feltzing
 *
 * All files in this archive are subject to the GNU General Public License.
 * See the file COPYING in the source tree root for full license agreement.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/
#include "config.h"
#include "cpu.h"
#include "system.h"
#include "backlight.h"
#include "backlight-target.h"
#include "pcf50606.h"
#include "lcd.h"

bool _backlight_init(void)
{
    _backlight_set_brightness(DEFAULT_BRIGHTNESS_SETTING);
    _backlight_on();
    
    return true; /* Backlight always ON after boot. */
}

void _backlight_on(void)
{
    int level;
    lcd_enable(true);
#ifndef BOOTLOADER
    _lcd_sleep_timer = 0; /* LCD should be awake already */
#endif
    level = set_irq_level(HIGHEST_IRQ_LEVEL);
    pcf50606_write(0x38, 0xb0); /* Backlight ON, GPO1INV=1, GPO1ACT=011 */
    set_irq_level(level);
}

void _backlight_off(void)
{
    int level = set_irq_level(HIGHEST_IRQ_LEVEL);
    pcf50606_write(0x38, 0x80); /* Backlight OFF, GPO1INV=1, GPO1ACT=000 */
    set_irq_level(level);
    lcd_enable(false);
#ifndef BOOTLOADER
    /* Start LCD sleep countdown */
    if (_lcd_sleep_timeout < 0)
    {
        _lcd_sleep_timer = 0; /* Setting == Always */
        lcd_sleep();
    }
    else
        _lcd_sleep_timer = _lcd_sleep_timeout;
#endif
}

/* set brightness by changing the PWM */
void _backlight_set_brightness(int val)
{
    /* disable IRQs while bitbanging */
    int old_irq_level = set_irq_level(HIGHEST_IRQ_LEVEL);
    pcf50606_write(0x35, (val << 1) | 0x01); /* 512Hz, Enable PWM */
    /* enable IRQs again */
    set_irq_level(old_irq_level);
}

void _remote_backlight_on(void)
{
    and_l(~0x00200000, &GPIO_OUT);
}

void _remote_backlight_off(void)
{
    or_l(0x00200000, &GPIO_OUT);
}
