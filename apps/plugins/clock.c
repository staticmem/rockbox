/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id: clock.c,v 2.0 2003/12/8
 *
 * Copyright (C) 2003 Zakk Roberts
 *
 * All files in this archive are subject to the GNU General Public License.
 * See the file COPYING in the source tree root for full license agreement.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/

/********************************
RELEASE NOTES *******************
*********************************

*********************************
VERSION 2.22 * STABLE ***********
*********************************
Fixed two bugs:
-Digital settings are now independent of LCD settings
-12/24h "Analog" settings are now displayed correctly.

*********************************
VERSION 2.21 * STABLE ***********
*********************************
Changed the behaviour of F2

********************************
VERSION 2.2 * STABLE ***********
********************************
Few small bugs taken care of.
Release version.

Features:
-New mode, "Binary"
-New mode selector - access via PLAY
-New feature, "Counter" - F2 to start/stop, Hold F2 to reset.
-New redesigned 9-page help screen.
-Small bugs fixed.

*********************************
VERSION 2.1 * STABLE ************
*********************************
Even more bugs fixed. Almost bug-free :)

Features:
-Another new mode!! LCD imitation (similar to Digital)
-American/European date modes added
-lots of bugs fixed

*********************************
VERSION 2.0: * BETA *************
*********************************

Major update. Code pretty much rewritten. Lots of bugs fixed.

New Features: *******************
-New mode, Fullscreen!
-Seperate settings for each mode
-Credits at F2
-Options at F3
-Startup / Shutdown logo (choose one by hitting ON)
-CLEANER code!

Analog specific changes: ********
-removed outer border option
-added 12/24h readout (and am/pm bitmaps for 12h mode)

Digital specific changes: *******
-now centers date and second readouts
-second readout options, "Digital" (text), "Bar" (progressbar), "Inverse" (inverts the display as the seconds go by)
-12/24h modes as well here (and of course the am/pm bitmaps)

********************************
VERSION 1.0 * "ALPHA" (STABLE) *
********************************
Release Version.

Features analog / digital modes,
and a few options.
********************************/

#include "plugin.h"
#include "time.h"

#ifdef HAVE_LCD_BITMAP

#define CLOCK_VERSION "2.22"

/* prototypes */
void show_logo(bool animate, bool show_clock_text);
void exit_logo(void);

/* used in help screen */
int screen = 1;

/* counter misc */
int start_tick = 0;
int passed_time = 0;
int counter = 0;
int displayed_value = 0;
int count_h, count_m, count_s;
char count_text[8];
bool counting = false;

static struct plugin_api* rb;

/***********************************************************
 * Used for hands to define lengths at a given time - ANALOG
 ***********************************************************/
static unsigned char xminute[] = {
56,59,61,64,67,70,72,75,77,79,80,82,83,84,84,84,84,84,83,82,80,79,77,75,72,70,67,64,61,59,56,53,51,48,45,42,40,37,35,33,32,30,29,28,28,28,28,28,29,30,32,33,35,37,40,42,45,48,51,53,
};
static unsigned char yminute[] = {
55,54,54,53,53,51,50,49,47,45,43,41,39,36,34,32,30,28,25,23,21,19,17,15,14,13,11,11,10,10,9,10,10,11,11,13,14,15,17,19,21,23,25,28,30,32,34,36,39,41,43,45,47,49,50,51,53,53,54,54,
};
static unsigned char yhour[] = {
47,47,46,46,46,45,44,43,42,41,39,38,36,35,33,32,31,29,28,26,25,23,22,21,20,19,18,18,18,17,17,17,18,18,18,19,20,21,22,23,25,26,28,29,31,32,33,35,36,38,39,41,42,43,44,45,46,46,46,47,
};
static unsigned char xhour[] = {
56,58,59,61,63,65,67,68,70,71,72,73,74,74,75,75,75,74,74,73,72,71,70,68,67,65,63,61,59,58,56,54,53,51,49,47,45,44,42,41,40,39,38,38,37,37,37,38,38,39,40,41,42,44,45,47,49,51,53,54,
};

/**************************************************************
 * Used for hands to define lengths at a give time - FULLSCREEN
 **************************************************************/
static unsigned char xminute_full[] = {
56,58,61,65,69,74,79,84,91,100,110,110,110,110,110,110,110,110,110,110,110,100,91,84,79,74,69,65,61,58,56,54,51,47,43,38,33,28,21,12,1,1,1,1,1,1,1,1,1,1,1,12,21,28,33,38,43,47,51,54
};
static unsigned char yminute_full[] = {
62,62,62,62,62,62,62,62,62,62,62,53,45,40,36,32,28,24,19,11,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,11,19,24,28,32,36,40,45,53,62,62,62,62,62,62,62,62,62,62
};
static unsigned char xhour_full[] = {
56,58,60,63,66,69,73,78,84,91,100,100,100,100,100,100,100,100,100,100,100,91,84,78,73,69,66,63,60,58,56,54,52,49,46,43,39,34,28,21,12,12,12,12,12,12,12,12,12,12,12,21,28,34,39,43,46,49,52,54
};
static unsigned char yhour_full[] = {
52,52,52,52,52,52,52,52,52,52,52,46,41,37,34,32,30,27,23,18,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,18,23,27,30,32,34,37,41,46,52,52,52,52,52,52,52,52,52,52
};

/****************************
 * BITMAPS
 ****************************/
/****************************
 * "0" bitmap - for binary
 ****************************/
static unsigned char bitmap_0[] = {
0xc0, 0xf0, 0x3c, 0x0e, 0x06, 0x03, 0x03, 0x03, 0x03, 0x06, 0x0e, 0x3c, 0xf0,
0xc0, 0x00,
0x1f, 0x7f, 0xe0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xe0, 0x7f,
0x1f, 0x00,
0x00, 0x00, 0x01, 0x03, 0x03, 0x06, 0x06, 0x06, 0x06, 0x03, 0x03, 0x01, 0x00,
0x00, 0x00 };
/****************************
 * "1" bitmap - for binary
 ****************************/
static unsigned char bitmap_1[] = {
0xe0, 0x70, 0x38, 0x1c, 0x0e, 0x07, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00,
0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x07, 0x07, 0x06, 0x06, 0x06, 0x06, 0x06,
0x06, 0x00 };
/****************************
 * PM indicator (moon + text)
 ****************************/
static unsigned char pm[] = { 0xFF,0xFF,0x33,0x33,0x33,0x1E,0x0C,0x00,0xFF,0xFF,0x06,0x0C,0x06,0xFF,0xFF };
/****************************
 * AM Indicator (sun and text
 ****************************/
static unsigned char am[] = { 0xFE,0xFF,0x1B,0x1B,0xFF,0xFE,0x00,0x00,0xFF,0xFF,0x06,0x0C,0x06,0xFF,0xFF };

/*****************************************
 * We've got 4 different clock logos...
 * The currently used one is #4,
 * "clockbox" -- the others are kept here
 * in case the logo selector is ever
 * brought back or we want to switch..
 ****************************************/
/* Logo #1 - Standard (by Zakk Roberts) */
const unsigned char clocklogo_1[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0xe0, 0xf8, 0xfc, 0xfe, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0,
0xf8, 0xfc, 0xfe, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x80, 0xc0, 0xe0, 0xe0, 0xe0, 0xc0, 0x00,
0x80, 0xe0, 0xf0, 0xf8, 0xfc, 0xfc, 0xfe, 0xfe, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e,
0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x3e, 0x3e, 0x3e, 0x1e, 0x0e, 0x00, 0x00,
0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x80, 0xe0, 0xf0,
0xf8, 0xfc, 0xfc, 0x7e, 0x3e, 0x3e, 0x3e, 0x3e, 0xbe, 0xbe, 0x3e, 0x3e, 0x3e,
0x3e, 0x7e, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfc, 0x00, 0x00, 0x00, 0x80, 0xe0,
0xf0, 0xf8, 0xfc, 0xfc, 0xfe, 0xfe, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e,
0x7e, 0x7e, 0x7e, 0x7e, 0x3e, 0x3e, 0x3e, 0x1e, 0x0e, 0x00, 0x00, 0x00, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe,
0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x00,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x3f, 0x3f, 0xc0, 0xc0, 0xc0,
0xc0, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xef, 0xc7, 0x83, 0x01,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
0x00, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff,
0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0x00,
0x0f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1e, 0x1c, 0x00, 0x00,
0x00, 0x1f, 0x1f, 0x1f, 0x0f, 0x07, 0x01, 0x00, 0x00, 0x00, 0x0f, 0x1f, 0x1f,
0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
0x1f, 0x1f, 0x1f, 0x0f, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x1f,
0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1e, 0x1c, 0x00, 0x00, 0x00, 0x1f,
0x1f, 0x1f, 0x0f, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x01, 0x03, 0x07, 0x0f, 0x1f, 0x1f, 0x1f, 0x1f
};

/* Logo #2 - Digital Segments (by Zakk Roberts) */
const unsigned char clocklogo_2[] = {
0x00, 0x00, 0x00, 0x00, 0xfe, 0xfd, 0xfb, 0xf7, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00,
0xfe, 0xfc, 0xf8, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xfd, 0xfb, 0xf7,
0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0xf7, 0xfb,
0xfd, 0xfe, 0x00, 0x00, 0x00, 0xfe, 0xfd, 0xfb, 0xf7, 0x0f, 0x0f, 0x0f, 0x0f,
0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00,
0x00, 0xfe, 0xfc, 0xf8, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
0xe0, 0xf0, 0x70, 0x10, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0xbf, 0x1f, 0x0f, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0xbf, 0x1f, 0x0f, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbf, 0x1f, 0x0f, 0x07,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0f,
0x1f, 0xbf, 0x00, 0x00, 0x00, 0xbf, 0x1f, 0x0f, 0x07, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0xbf, 0x5f, 0xef, 0xf7, 0xf0, 0xf0, 0xf0, 0xe0, 0x40, 0x00, 0x06, 0x07,
0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xfe, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0xff, 0xff, 0xfe, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xfe, 0xfc,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xfe,
0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xfe, 0xfc, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0xff, 0xff, 0xfe, 0xfd, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x0c, 0x3c,
0xfc, 0xf0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
0xc0, 0x00, 0x40, 0x40, 0x4f, 0x17, 0x1b, 0xdd, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e,
0x1e, 0x1e, 0x1e, 0xde, 0x1e, 0x5e, 0x5c, 0x58, 0x10, 0xc0, 0x00, 0x00, 0xc0,
0x0f, 0x57, 0x5b, 0x5d, 0x1e, 0x1e, 0x1e, 0x1e, 0xde, 0x1e, 0x1e, 0x1e, 0x9e,
0x1e, 0x1e, 0x1c, 0x18, 0x10, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xd7, 0x1b, 0x5d,
0x5e, 0x5e, 0x1e, 0xde, 0x1e, 0x1e, 0xde, 0x1e, 0x5e, 0x5e, 0x5e, 0x1d, 0xdb,
0x17, 0x0f, 0xc0, 0x00, 0x40, 0x4f, 0x57, 0x1b, 0x1d, 0x1e, 0x1e, 0xde, 0x1e,
0x1e, 0x1e, 0x9e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1c, 0x18, 0x10, 0x00, 0x00, 0x00,
0x00, 0x0f, 0x07, 0x03, 0x01, 0x40, 0x40, 0x40, 0x00, 0xc0, 0x00, 0x00, 0x00,
0x00, 0xc1, 0x01, 0x41, 0x40, 0x40, 0x00, 0xc0,
0x1d, 0x00, 0x10, 0x10, 0x10, 0x00, 0x00, 0x1d, 0x00, 0x10, 0x10, 0x10, 0x00,
0x00, 0x00, 0x00, 0x1d, 0x00, 0x10, 0x10, 0x10, 0x00, 0x1d, 0x00, 0x00, 0x1d,
0x00, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x02, 0x00, 0x0d,
0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x02,
0x00, 0x02, 0x05, 0x18, 0x00, 0x00, 0x1d, 0x00, 0x10, 0x10, 0x10, 0x00, 0x1d,
0x00, 0x00, 0x1d, 0x00, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x1d, 0x00,
0x02, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x1c, 0x00, 0x12, 0x10, 0x12, 0x00, 0x01, 0x00, 0x10, 0x10,
0x00, 0x1d, 0x00, 0x14, 0x10, 0x11, 0x00, 0x1d
};

/* Logo #3 - Electroplate (by Adam S. & Zakk Roberts)
 * Definition: (N) any artifact that has been plated with a thin coat of metal by electrolysis */
const unsigned char clocklogo_3[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x1c, 0x3c, 0xfc,
0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0x5c, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xfc, 0xf8,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x0c, 0x1c, 0x3c, 0xfc, 0xfc, 0xfc,
0xfc, 0xf8, 0xe8, 0xb8, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0xe0, 0xf0, 0xf0, 0xf0, 0xf0, 0xb0,
0xf0, 0xd0, 0x70, 0x50, 0x70, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0x55, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x80, 0xc0, 0xe0, 0xe0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf8, 0xff, 0xff, 0xff,
0xf8, 0xf0, 0xf0, 0xf0, 0xa0, 0xe0, 0x40, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0xe0, 0xf0, 0xf0, 0xf0, 0x70, 0xf0,
0xb0, 0xf0, 0x70, 0x70, 0x70, 0x60, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xaa, 0xff, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0xe0,
0xe0, 0xe0, 0x60, 0xe0, 0x20, 0x00, 0x00, 0x00,
0x00, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xaf, 0xfb, 0x0e, 0x01, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0x55, 0xff, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xfe,
0xff, 0xff, 0xff, 0xff, 0xff, 0x6b, 0x0f, 0x02, 0x01, 0x00, 0xff, 0xff, 0xff,
0xe0, 0xe0, 0xe1, 0xe3, 0xef, 0xff, 0xff, 0xff, 0xfd, 0xf6, 0xf8, 0xe0, 0xe0,
0xc0, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xb7, 0xfd, 0x0f, 0x01, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xaa, 0xff, 0xf8, 0xfc, 0xfe, 0xff, 0xff, 0xff, 0x5f, 0xf7,
0x3f, 0x05, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x03, 0x1f, 0x3f, 0x7f, 0xff, 0xff, 0xfe, 0xfb, 0xee, 0xf8, 0xd0,
0xe0, 0xa0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f,
0xff, 0xff, 0xff, 0xff, 0x7f, 0xd5, 0xff, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07,
0x1f, 0x3f, 0x7f, 0xff, 0xff, 0xfd, 0xf6, 0xf8, 0xf0, 0xf0, 0xf0, 0xf1, 0xf1,
0xf1, 0xf1, 0xf1, 0xb9, 0xfd, 0x5f, 0x7f, 0x2f, 0x3f, 0x0b, 0x07, 0x01, 0x01,
0x00, 0x00, 0x03, 0x1f, 0x3f, 0x7f, 0xff, 0xff, 0xfe, 0xfb, 0xee, 0xf8, 0xd0,
0xe0, 0xe0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff,
0xff, 0x7f, 0xff, 0xaa, 0x7f, 0x03, 0x03, 0x0f, 0x3f, 0x7f, 0x7f, 0xff, 0xfd,
0xf6, 0xfc, 0xd8, 0xe0, 0x40, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* Logo #4 - "Clockbox" (by Adam S.) */
const unsigned char clocklogo_4[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0xe0, 0xf0, 0xf0,
0x78, 0x78, 0x3c, 0x3c, 0xfc, 0xfc, 0xfc, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e,
0x1e, 0x3c, 0x3c, 0x3c, 0x7c, 0x7c, 0xf8, 0xf8, 0xf0, 0x30, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0xe0, 0xf8, 0xfc, 0xfe, 0xff, 0x3f, 0x3f, 0x7f, 0x73, 0xf1, 0xe0,
0xe0, 0xc0, 0xc0, 0x80, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0xe0,
0xfe, 0xff, 0xff, 0x7f, 0x03, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
0x80, 0xf0, 0xf8, 0xfc, 0xfe, 0x3e, 0x0e, 0x0f, 0x07, 0x07, 0x0f, 0x1f, 0xfe,
0xfe, 0xfc, 0xf8, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xf0, 0xf8, 0xfc, 0x7e, 0x1e,
0x0f, 0x0f, 0x07, 0x0f, 0x0f, 0x0f, 0x1e, 0x1e, 0x00, 0x00, 0x00, 0x00, 0xf0,
0xff, 0xff, 0xff, 0xff, 0xe1, 0xf0, 0xf8, 0x7c, 0x3e, 0x1f, 0x0f, 0x07, 0x03,
0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x01, 0x01, 0x0f, 0x1f, 0x3f, 0x3f, 0x3f, 0x3f, 0x0e, 0x00, 0x40, 0x7c, 0x7f,
0x7f, 0x7f, 0x77, 0x70, 0x70, 0x70, 0x70, 0x70, 0x30, 0x00, 0x00, 0x00, 0x00,
0x0f, 0x3f, 0x7f, 0x7f, 0xfd, 0xf0, 0xe0, 0xe0, 0xf0, 0xf0, 0xf8, 0xff, 0xff,
0xdf, 0xcf, 0xc3, 0xc0, 0xc0, 0x80, 0x80, 0x1f, 0x3f, 0x7f, 0x7f, 0xf8, 0xf0,
0xf0, 0xf0, 0xf0, 0xf0, 0xf8, 0xf8, 0xc0, 0xc0, 0xc0, 0xc0, 0xe0, 0xfe, 0xff,
0x7f, 0x3f, 0x07, 0x03, 0x0f, 0x3f, 0x7f, 0xfe, 0xfc, 0xf0, 0xc0, 0x00, 0x00,
0x00, 0x00, 0x80, 0xc0, 0xc0, 0xc0, 0xc0, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x03, 0x1f, 0x3f, 0x7f, 0xff, 0xff, 0xfc, 0xf0, 0xe0, 0xc0, 0x80, 0x80, 0x80,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0xc0, 0xc0,
0xc0, 0xe0, 0xf0, 0xf0, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xfe, 0xff, 0xff, 0x7f, 0x73,
0x71, 0x71, 0xf9, 0xff, 0xff, 0xff, 0x8f, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xfc,
0xfe, 0xff, 0xff, 0x07, 0x03, 0x01, 0x01, 0x01, 0x01, 0xc3, 0xff, 0xff, 0xff,
0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0xc7, 0xff, 0xff, 0xff, 0xfc,
0xfe, 0xdf, 0x0f, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x07, 0x07, 0x07, 0x07,
0x07, 0x07, 0x0f, 0x0f, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x03,
0x03, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x1f, 0x1f, 0x1f, 0x1f, 0x1c, 0x1c,
0x1c, 0x1e, 0x1f, 0x0f, 0x0f, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x07,
0x0f, 0x1f, 0x1f, 0x1e, 0x1c, 0x1c, 0x1c, 0x1e, 0x1f, 0x0f, 0x07, 0x07, 0x01,
0x00, 0x00, 0x00, 0x10, 0x1c, 0x1e, 0x1f, 0x0f, 0x07, 0x03, 0x01, 0x01, 0x07,
0x1f, 0x1f, 0x1f, 0x1c, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* settings saved to this location */
static char default_filename[] = "/.rockbox/rocks/.clock_settings";

/* names of contributors */
char* credits[] = {
"Zakk Roberts",
"Linus N. Feltzing",
"BlueChip",
"T.P. Diffenbach",
"David McIntyre",
"Justin D. Young",
"Lee Pilgrim",
"top_bloke",
"Adam S.",
"Scott Myran",
"Tony Kirk",
"Jason Tye"
};

/* ...and how they helped */
char* jobs[] = {
"Code",
"Code",
"Code",
"Code",
"Code",
"Code",
"Code",
"Code",
"Logo",
"Design",
"Design",
"Design"
};

bool done = false; /* This bool is used for most of the while loops */
int logo = 4; /* must be set before we show the logo */

/************************************
 * This is saved to default_filename
 ************************************/
struct saved_settings
{
    /* general */
    int clock; /* 1: analog, 2: digital led, 3: digital lcd, 4: full, 5: binary */
    bool backlight_on;

    /* analog */
    bool analog_digits;
    int analog_date; /* 0: off, 1: american, 2: european */
    bool analog_secondhand;
    int analog_time; /* 0: off, 1: 24h, 2: 12h */

    /* digital */
    int digital_seconds; /* 0: off, 1: digital, 2: bar, 3: fullscreen */
    int digital_date; /* 0: off, 1: american, 2: european */
    bool digital_blinkcolon;
    bool digital_12h;

    /* LCD */
    int lcd_seconds; /* 0: off, 1: lcd, 2: bar, 3: fullscreen */
    int lcd_date; /* 0: off, 1: american, 2: european */
    bool lcd_blinkcolon;
    bool lcd_12h;

    /* fullscreen */
    bool fullscreen_border;
    bool fullscreen_secondhand;
    bool fullscreen_invertseconds;
} settings;

/*************************
 * Setting default values
 *************************/
void reset_settings(void)
{
    /* general */
    settings.clock = 1; /* 1: analog, 2: digital led, 3: digital lcd, 4: full, 5: binary */
    settings.backlight_on = true;

    /* analog */
    settings.analog_digits = false;
    settings.analog_date = 0; /* 0: off, 1: american, 2: european */
    settings.analog_secondhand = true;
    settings.analog_time = false; /* 0: off, 1: 24h, 2: 12h */

    /* digital */
    settings.digital_seconds = 1; /* 0: off, 1: digital, 2: bar, 3: fullscreen */
    settings.digital_date = 1; /* 0: off, 1: american, 2: european */
    settings.digital_blinkcolon = false;
    settings.digital_12h = true;

    /* LCD */
    settings.lcd_seconds = 1; /* 0: off, 1: lcd, 2: bar, 3: fullscreen */
    settings.lcd_date = 1; /* 0: off, 1: american, 2: european */
    settings.lcd_blinkcolon = false;
    settings.lcd_12h = true;

    /* fullscreen */
    settings.fullscreen_border = true;
    settings.fullscreen_secondhand = true;
    settings.fullscreen_invertseconds = false;
}

/*********************************
 * Saves "saved_settings" to disk
 *********************************/
void save_settings(void)
{
    int fd;

    rb->lcd_clear_display();

    show_logo(true, true);
    rb->lcd_puts(0, 7, "Saving...");

    rb->lcd_update();

    fd = rb->creat(default_filename, O_WRONLY);

    if(fd >= 0)
    {
        rb->write (fd, &settings, sizeof(struct saved_settings));
        rb->close(fd);
        rb->lcd_puts(0, 7, "Save Succeeded");
    }
    else
        rb->lcd_puts(0, 7, "Save Failed");

    rb->lcd_update();

    rb->sleep(HZ);

    exit_logo();
}

/***********************************
 * Loads "saved_settings" from disk
 ***********************************/
void load_settings(void)
{
    int fd;
    fd = rb->open(default_filename, O_RDONLY);

    rb->lcd_setfont(FONT_SYSFIXED);

    rb->lcd_puts(0, 6, "Clock " CLOCK_VERSION);

    if(fd >= 0) /* does file exist? */
    {
        if(rb->filesize(fd) == sizeof(struct saved_settings)) /* if so, is it the right size? */
        {
            rb->lcd_puts(0, 7, "Loading...");
            rb->read(fd, &settings, sizeof(struct saved_settings));
            rb->close(fd);
            show_logo(true, true);
            rb->lcd_puts(0, 6, "Clock " CLOCK_VERSION);
            rb->lcd_puts(0, 7, "Loaded settings");
        }
        else /* bail out */
        {
            rb->lcd_puts(0, 7, "Old settings file");
            reset_settings();
        }
    }
    else /* bail out */
    {
        rb->lcd_puts(0, 7, "Can't find settings");
        reset_settings();
    }

    rb->lcd_update();

    rb->sleep(HZ*2);

    exit_logo();
}

/**************************************************************
 * 7-Segment LED/LCD imitation code, by Linus Nielsen Feltzing
 **************************************************************/
/*
       a     0    b
        #########c
       #         #`
       #         #
      1#         #2
       #         #
       #    3    #
      c ######### d
       #         #
       #         #
      4#         #5
       #         #
       #    6    #
      e ######### f
*/
static unsigned int point_coords[6][2] =
{
    {0, 0}, /* a */
    {1, 0}, /* b */
    {0, 1}, /* c */
    {1, 1}, /* d */
    {0, 2}, /* e */
    {1, 2}  /* f */
};

/*********************************************
 * The end points (a-f) for each segment line
 *********************************************/
static unsigned int seg_points[7][2] =
{
    {0,1}, /* a to b */
    {0,2}, /* a to c */
    {1,3}, /* b to d */
    {2,3}, /* c to d */
    {2,4}, /* c to e */
    {3,5}, /* d to f */
    {4,5}  /* e to f */
};

/***********************************************************************
 * Lists that tell which segments (0-6) to enable for each digit (0-9),
 * the list is terminated with -1
 ***********************************************************************/
static int digit_segs[10][8] =
{
    {0,1,2,4,5,6, -1},   /* 0 */
    {2,5, -1},           /* 1 */
    {0,2,3,4,6, -1},     /* 2 */
    {0,2,3,5,6, -1},     /* 3 */
    {1,2,3,5, -1},       /* 4 */
    {0,1,3,5,6, -1},     /* 5 */
    {0,1,3,4,5,6, -1},   /* 6 */
    {0,2,5, -1},         /* 7 */
    {0,1,2,3,4,5,6, -1}, /* 8 */
    {0,1,2,3,5,6, -1}    /* 9 */
};

/************************************
 * Draws one segment - LED imitation
 ************************************/
void draw_seg_led(int seg, int x, int y, int width, int height)
{
    int p1 = seg_points[seg][0];
    int p2 = seg_points[seg][1];
    int x1 = point_coords[p1][0];
    int y1 = point_coords[p1][1];
    int x2 = point_coords[p2][0];
    int y2 = point_coords[p2][1];

    /* It draws parallel lines of different lengths for thicker segments */
    if(seg == 0 || seg == 3 || seg == 6)
    {
        rb->lcd_drawline(x + x1 * width + 1, y + y1 * height / 2,
                         x + x2 * width - 1 , y + y2 * height / 2);

        rb->lcd_drawline(x + x1 * width + 2, y + y1 * height / 2 - 1,
                         x + x2 * width - 2, y + y2 * height / 2 - 1);
        rb->lcd_drawline(x + x1 * width + 2, y + y1 * height / 2 + 1,
                         x + x2 * width - 2, y + y2 * height / 2 + 1);

        rb->lcd_drawline(x + x1 * width + 3, y + y1 * height / 2 - 2,
                         x + x2 * width - 3, y + y2 * height / 2 - 2);
        rb->lcd_drawline(x + x1 * width + 3, y + y1 * height / 2 + 2,
                         x + x2 * width - 3, y + y2 * height / 2 + 2);
    }
    else
    {
        rb->lcd_drawline(x + x1 * width, y + y1 * height / 2 + 1,
                         x + x2 * width , y + y2 * height / 2 - 1);

        rb->lcd_drawline(x + x1 * width - 1, y + y1 * height / 2 + 2,
                         x + x2 * width - 1, y + y2 * height / 2 - 2);
        rb->lcd_drawline(x + x1 * width + 1, y + y1 * height / 2 + 2,
                         x + x2 * width + 1, y + y2 * height / 2 - 2);

        rb->lcd_drawline(x + x1 * width - 2, y + y1 * height / 2 + 3,
                         x + x2 * width - 2, y + y2 * height / 2 - 3);

        rb->lcd_drawline(x + x1 * width + 2, y + y1 * height / 2 + 3,
                         x + x2 * width + 2, y + y2 * height / 2 - 3);
    }
}

/************************************
 * Draws one segment - LCD imitation
 ************************************/
void draw_seg_lcd(int seg, int x, int y, int width, int height)
{
    int p1 = seg_points[seg][0];
    int p2 = seg_points[seg][1];
    int x1 = point_coords[p1][0];
    int y1 = point_coords[p1][1];
    int x2 = point_coords[p2][0];
    int y2 = point_coords[p2][1];

    if(seg == 0)
    {
        rb->lcd_drawline(x + x1 * width,     y + y1 * height / 2 - 1,
                         x + x2 * width,     y + y2 * height / 2 - 1);
        rb->lcd_drawline(x + x1 * width + 1, y + y1 * height / 2,
                         x + x2 * width - 1, y + y2 * height / 2);
        rb->lcd_drawline(x + x1 * width + 2, y + y1 * height / 2 + 1,
                         x + x2 * width - 2, y + y2 * height / 2 + 1);
        rb->lcd_drawline(x + x1 * width + 3, y + y1 * height / 2 + 2,
                         x + x2 * width - 3, y + y2 * height / 2 + 2);
    }
    else if(seg == 3)
    {
        rb->lcd_drawline(x + x1 * width + 1, y + y1 * height / 2,
                         x + x2 * width - 1, y + y2 * height / 2);
        rb->lcd_drawline(x + x1 * width + 2, y + y1 * height / 2 - 1,
                         x + x2 * width - 2, y + y2 * height / 2 - 1);
        rb->lcd_drawline(x + x1 * width + 2, y + y1 * height / 2 + 1,
                         x + x2 * width - 2, y + y2 * height / 2 + 1);
        rb->lcd_drawline(x + x1 * width + 3, y + y1 * height / 2 - 2,
                         x + x2 * width - 3, y + y2 * height / 2 - 2);
        rb->lcd_drawline(x + x1 * width + 3, y + y1 * height / 2 + 2,
                         x + x2 * width - 3, y + y2 * height / 2 + 2);
    }
    else if(seg == 6)
    {
        rb->lcd_drawline(x + x1 * width,     y + y1 * height / 2 + 1,
                         x + x2 * width,     y + y2 * height / 2 + 1);
        rb->lcd_drawline(x + x1 * width + 1, y + y1 * height / 2,
                         x + x2 * width - 1, y + y2 * height / 2);
        rb->lcd_drawline(x + x1 * width + 2, y + y1 * height / 2 - 1,
                         x + x2 * width - 2, y + y2 * height / 2 - 1);
        rb->lcd_drawline(x + x1 * width + 3, y + y1 * height / 2 - 2,
                         x + x2 * width - 3, y + y2 * height / 2 - 2);

    }
    else if(seg == 1 || seg == 4)
    {
        rb->lcd_drawline(x + x1 * width - 1, y + y1 * height / 2,
                         x + x2 * width - 1, y + y2 * height / 2);
        rb->lcd_drawline(x + x1 * width,     y + y1 * height / 2 + 1,
                         x + x2 * width,     y + y2 * height / 2 - 1);
        rb->lcd_drawline(x + x1 * width + 1, y + y1 * height / 2 + 2,
                         x + x2 * width + 1, y + y2 * height / 2 - 2);
        rb->lcd_drawline(x + x1 * width + 2, y + y1 * height / 2 + 3,
                         x + x2 * width + 2, y + y2 * height / 2 - 3);
    }
    else if(seg == 2 || seg == 5)
    {
        rb->lcd_drawline(x + x1 * width + 1, y + y1 * height / 2,
                         x + x2 * width + 1, y + y2 * height / 2);
        rb->lcd_drawline(x + x1 * width,     y + y1 * height / 2 + 1,
                         x + x2 * width,     y + y2 * height / 2 - 1);
        rb->lcd_drawline(x + x1 * width - 1, y + y1 * height / 2 + 2,
                         x + x2 * width - 1, y + y2 * height / 2 - 2);
        rb->lcd_drawline(x + x1 * width - 2, y + y1 * height / 2 + 3,
                         x + x2 * width - 2, y + y2 * height / 2 - 3);
    }
}

/******************
 * Draws one digit
 ******************/
void draw_7seg_digit(int digit, int x, int y, int width, int height, bool lcd_display)
{
    int i;
    int c;

    for(i = 0;digit_segs[digit][i] >= 0;i++)
    {
        c = digit_segs[digit][i];

        if(!lcd_display)
            draw_seg_led(c, x, y, width, height);
        else
            draw_seg_lcd(c, x, y, width, height);
    }
}

/******************************************************
 * Draws the entire 7-segment hour-minute time display
 ******************************************************/
void draw_7seg_time(int hour, int minute, int x, int y, int width, int height,
bool colon, bool lcd)
{
    int xpos = x;

    /* Draw PM indicator */
	if(settings.clock == 2)
	{
		if(settings.digital_12h)
		{
			if(hour > 12)
				rb->lcd_bitmap(pm, 97, 55, 15, 8, true);
			else
				rb->lcd_bitmap(am, 1, 55, 15, 8, true);
		}
	}
    else
    {
		if(settings.lcd_12h)
		{
			if(hour > 12)
				rb->lcd_bitmap(pm, 97, 55, 15, 8, true);
			else
				rb->lcd_bitmap(am, 1, 55, 15, 8, true);
		}
	}

    /* Now change to 12H mode if requested */
    if(settings.clock == 2)
    {
		if(settings.digital_12h)
			if(hour > 12)
				hour -= 12;
	}
	else
	{
		if(settings.lcd_12h)
			if(hour > 12)
				hour -= 12;
	}

    draw_7seg_digit(hour / 10, xpos, y, width, height, lcd);
    xpos += width + 6;
    draw_7seg_digit(hour % 10, xpos, y, width, height, lcd);
    xpos += width + 6;

    if(colon)
    {
        rb->lcd_drawline(xpos, y + height/3 + 2,
                         xpos, y + height/3 + 2);
        rb->lcd_drawline(xpos+1, y + height/3 + 1,
                         xpos+1, y + height/3 + 3);
        rb->lcd_drawline(xpos+2, y + height/3,
                         xpos+2, y + height/3 + 4);
        rb->lcd_drawline(xpos+3, y + height/3 + 1,
                         xpos+3, y + height/3 + 3);
        rb->lcd_drawline(xpos+4, y + height/3 + 2,
                         xpos+4, y + height/3 + 2);

        rb->lcd_drawline(xpos, y + height-height/3 + 2,
                         xpos, y + height-height/3 + 2);
        rb->lcd_drawline(xpos+1, y + height-height/3 + 1,
                         xpos+1, y + height-height/3 + 3);
        rb->lcd_drawline(xpos+2, y + height-height/3,
                         xpos+2, y + height-height/3 + 4);
        rb->lcd_drawline(xpos+3, y + height-height/3 + 1,
                         xpos+3, y + height-height/3 + 3);
        rb->lcd_drawline(xpos+4, y + height-height/3 + 2,
                         xpos+4, y + height-height/3 + 2);
    }

    xpos += 12;

    draw_7seg_digit(minute / 10, xpos, y, width, height, lcd);
    xpos += width + 6;
    draw_7seg_digit(minute % 10, xpos, y, width, height, lcd);
    xpos += width + 6;
}

/**************
 * Binary mode
 **************/
void binary(int hour, int minute, int second)
{
    /* temporary modifiable versions of ints */
    int temphour = hour;
    int tempmin = minute;
    int tempsec = second;

    rb->lcd_clear_display();

    /*****************
     * HOUR
     *****************/
    if(temphour >= 32)
    {
        rb->lcd_bitmap(bitmap_1, 0, 1, 15, 20, true);
        temphour -= 32;
    }
    else
        rb->lcd_bitmap(bitmap_0, 0, 1, 15, 20, true);
    if(temphour >= 16)
    {
        rb->lcd_bitmap(bitmap_1, 19, 1, 15, 20, true);
        temphour -= 16;
    }
    else
        rb->lcd_bitmap(bitmap_0, 19, 1, 15, 20, true);
    if(temphour >= 8)
    {
        rb->lcd_bitmap(bitmap_1, 38, 1, 15, 20, true);
        temphour -= 8;
    }
    else
        rb->lcd_bitmap(bitmap_0, 38, 1, 15, 20, true);
    if(temphour >= 4)
    {
        rb->lcd_bitmap(bitmap_1, 57, 1, 15, 20, true);
        temphour -= 4;
    }
    else
        rb->lcd_bitmap(bitmap_0, 57, 1, 15, 20, true);
    if(temphour >= 2)
    {
         rb->lcd_bitmap(bitmap_1, 76, 1, 15, 20, true);
        temphour -= 2;
    }
    else
        rb->lcd_bitmap(bitmap_0, 76, 1, 15, 20, true);
    if(temphour >= 1)
    {
        rb->lcd_bitmap(bitmap_1, 95, 1, 15, 20, true);
        temphour -= 1;
    }
    else
        rb->lcd_bitmap(bitmap_0, 95, 1, 15, 20, true);

    /****************
     * MINUTES
     ****************/
    if(tempmin >= 32)
    {
        rb->lcd_bitmap(bitmap_1, 0, 21, 15, 20, true);
        tempmin -= 32;
    }
    else
        rb->lcd_bitmap(bitmap_0, 0, 21, 15, 20, true);
    if(tempmin >= 16)
    {
         rb->lcd_bitmap(bitmap_1, 19, 21, 15, 20, true);
        tempmin -= 16;
    }
    else
        rb->lcd_bitmap(bitmap_0, 19, 21, 15, 20, true);
    if(tempmin >= 8)
    {
         rb->lcd_bitmap(bitmap_1, 38, 21, 15, 20, true);
        tempmin -= 8;
    }
    else
        rb->lcd_bitmap(bitmap_0, 38, 21, 15, 20, true);
    if(tempmin >= 4)
    {
         rb->lcd_bitmap(bitmap_1, 57, 21, 15, 20, true);
        tempmin -= 4;
    }
    else
        rb->lcd_bitmap(bitmap_0, 57, 21, 15, 20, true);
    if(tempmin >= 2)
    {
        rb->lcd_bitmap(bitmap_1, 76, 21, 15, 20, true);
        tempmin -= 2;
    }
    else
        rb->lcd_bitmap(bitmap_0, 76, 21, 15, 20, true);
    if(tempmin >= 1)
    {
        rb->lcd_bitmap(bitmap_1, 95, 21, 15, 20, true);
        tempmin -= 1;
    }
    else
        rb->lcd_bitmap(bitmap_0, 95, 21, 15, 20, true);

    /****************
     * SECONDS
     ****************/
    if(tempsec >= 32)
    {
         rb->lcd_bitmap(bitmap_1, 0, 42, 15, 20, true);
        tempsec -= 32;
    }
    else
        rb->lcd_bitmap(bitmap_0, 0, 42, 15, 20, true);
    if(tempsec >= 16)
    {
        rb->lcd_bitmap(bitmap_1, 19, 42, 15, 20, true);
        tempsec -= 16;
    }
    else
        rb->lcd_bitmap(bitmap_0, 19, 42, 15, 20, true);
    if(tempsec >= 8)
    {
        rb->lcd_bitmap(bitmap_1, 38, 42, 15, 20, true);
        tempsec -= 8;
    }
    else
        rb->lcd_bitmap(bitmap_0, 38, 42, 15, 20, true);
    if(tempsec >= 4)
    {
         rb->lcd_bitmap(bitmap_1, 57, 42, 15, 20, true);
        tempsec -= 4;
    }
    else
        rb->lcd_bitmap(bitmap_0, 57, 42, 15, 20, true);
    if(tempsec >= 2)
    {
         rb->lcd_bitmap(bitmap_1, 76, 42, 15, 20, true);
        tempsec -= 2;
    }
    else
        rb->lcd_bitmap(bitmap_0, 76, 42, 15, 20, true);
    if(tempsec >= 1)
    {
        rb->lcd_bitmap(bitmap_1, 95, 42, 15, 20, true);
        tempsec -= 1;
    }
    else
        rb->lcd_bitmap(bitmap_0, 95, 42, 15, 20, true);

    rb->lcd_update();
}

/*****************
 * Shows the logo
 *****************/
void show_logo(bool animate, bool show_clock_text)
{
    int x_position;
    unsigned char *clogo = 0;

    /* decide which logo we're using - usually #4 */
    if(logo == 1)
        clogo = (unsigned char *)clocklogo_1;
    else if(logo == 2)
        clogo = (unsigned char *)clocklogo_2;
    else if(logo == 3)
        clogo = (unsigned char *)clocklogo_3;
    else
        clogo = (unsigned char *)clocklogo_4;

    /* animate logo */
    if(animate)
    {
        /* move down the screen */
        for(x_position = 0; x_position <= 25; x_position++)
        {
            rb->lcd_clearline(0, x_position/2-1, 111, x_position/2-1);
            rb->lcd_clearline(0, x_position/2+38, 111, x_position/2+38);
            rb->lcd_bitmap(clogo, 0, x_position/2, 112, 37, true);
            if(show_clock_text)
                rb->lcd_puts(0, 6, "Clock " CLOCK_VERSION);
            rb->lcd_update();
        }
        /* bounce back up a little */
        for(x_position = 25; x_position >= 18; x_position--)
        {
            rb->lcd_clearline(0, x_position/2-1, 111, x_position/2-1);
            rb->lcd_clearline(0, x_position/2+38, 111, x_position/2+38);
            rb->lcd_bitmap(clogo, 0, x_position/2, 112, 37, true);
            if(show_clock_text)
                rb->lcd_puts(0, 6, "Clock " CLOCK_VERSION);
            rb->lcd_update();
        }
        /* and go back down again */
        for(x_position = 18; x_position <= 20; x_position++)
        {
            rb->lcd_clearline(0, x_position/2-1, 111, x_position/2-1);
            rb->lcd_clearline(0, x_position/2+38, 111, x_position/2+38);
            rb->lcd_bitmap(clogo, 0, x_position/2, 112, 37, true);
            if(show_clock_text)
                rb->lcd_puts(0, 6, "Clock " CLOCK_VERSION);
            rb->lcd_update();
        }
    }
    else /* don't animate, just show */
    {
        rb->lcd_bitmap(clogo, 0, 10, 112, 37, true);
        rb->lcd_update();
    }
}

/*********************
 * Logo flies off lcd
 *********************/
void exit_logo(void)
{
    int x_position;

    unsigned char *clogo = 0;

    if(logo == 1)
        clogo = (unsigned char *)clocklogo_1;
    else if(logo == 2)
        clogo = (unsigned char *)clocklogo_2;
    else if(logo == 3)
        clogo = (unsigned char *)clocklogo_3;
    else
        clogo = (unsigned char *)clocklogo_4;

    /* fly downwards */
    for(x_position = 20; x_position <= 128; x_position++)
    {
        rb->lcd_clearline(0, x_position/2-1, 111, x_position/2-1);
        rb->lcd_bitmap(clogo, 0, x_position/2, 112, 37, true);
        rb->lcd_update();
    }
}

/********************
 * Rolls the credits
 ********************/
/* The following function is pretty confusing, so
 * it's extra well commented. */
bool roll_credits(void)
{
    int j, namepos, jobpos; /* namepos/jobpos are x coords for strings of text */
    int btn;
    int numnames = 12; /* amount of people in the credits array */
    int pause;

    /* used to center the text */
    char name[20];
    char job[15];
    int name_w, name_h;
    int job_w, job_h;

    /* Shows "[Credits] X/X" */
    char elapsednames[16];

    for(j=0; j < numnames; j++)
    {
        rb->lcd_clear_display();

        show_logo(false, false);

        rb->snprintf(elapsednames, sizeof(elapsednames), "[Credits] %d/%d", j+1, numnames);
        rb->lcd_puts(0, 0, elapsednames);

        /* used to center the text */
        rb->snprintf(name, sizeof(name), "%s", credits[j]);
        rb->snprintf(job, sizeof(job), "%s", jobs[j]);
        rb->lcd_getstringsize(name, &name_w, &name_h);
        rb->lcd_getstringsize(job, &job_w, &job_h);

        /* line 1 flies in */
        for (namepos=0; namepos < LCD_WIDTH/2-name_w/2; namepos++)
        {
            rb->lcd_clearrect(0, 48, 112, 8); /* clear any trails left behind */
            rb->lcd_putsxy(namepos, 48, name); /* show their name */
            rb->lcd_update();

            /* exit on keypress */
            btn = rb->button_get(false);
            if (btn !=  BUTTON_NONE && !(btn & BUTTON_REL))
                return false;
        }

        /* now line 2 flies in */
        for(jobpos=LCD_WIDTH; jobpos > LCD_WIDTH/2-(job_w+2)/2; jobpos--) /* we use (job_w+2) to ensure it fits on the LCD */
        {
            rb->lcd_clearrect(0, 56, 112, 8); /* clear trails */
            rb->lcd_putsxy(jobpos, 56, job); /* show their job */
            rb->lcd_update();

            /* exit on keypress */
            btn = rb->button_get(false);
            if (btn !=  BUTTON_NONE && !(btn & BUTTON_REL))
                return false;
        }

        /* pause (2s) and scan for button presses */
        for (pause = 0; pause < 10; pause++)
        {
            rb->sleep((HZ*2)/10); /* wait a moment */

            btn = rb->button_get(false);
            if (btn !=  BUTTON_NONE && !(btn & BUTTON_REL))
                return false;
        }

        /* fly out both lines at same time */
        namepos=(LCD_WIDTH/2-name_w/2)+5;
        jobpos=(LCD_WIDTH/2-(job_w+2)/2)-5;
        while(namepos<LCD_WIDTH || jobpos > 0-job_w)
        {
            rb->lcd_clearrect(0, 48, 112, 16); /* clear trails */
            rb->lcd_putsxy(namepos, 48, name);
            rb->lcd_putsxy(jobpos, 56, job);
            rb->lcd_update();

            /* exit on keypress */
            btn = rb->button_get(false);
            if (btn !=  BUTTON_NONE && !(btn & BUTTON_REL))
                return false;

            namepos++;
            jobpos--;
        }

        /* pause (.5s) */
        rb->sleep(HZ/2);

        /* and scan for button presses */
        btn = rb->button_get(false);
        if ( (!btn) && (!(btn&BUTTON_REL)) )
             return false;
    }
    return true;
}

/*****************************************
 * Shows the logo, then rolls the credits
 *****************************************/
bool show_credits(void)
{
    int j = 0;
    int btn;

    rb->lcd_clear_display();

    /* show the logo with an animation and the clock version text */
    show_logo(true, true);

    rb->lcd_puts(0, 7, "Credit Roll...");

    rb->lcd_update();

    /* pause while button scanning */
    for (j = 0; j < 10; j++)
    {
        rb->sleep((HZ*2)/10);

        btn = rb->button_get(false);
        if (btn !=  BUTTON_NONE && !(btn & BUTTON_REL))
            return false;
    }

    /* then roll the credits */
    roll_credits();

    return false;
}

/*******************
 * F1 Screen - HELP
 *******************/
bool f1_screen(void)
{
    done = false;

    while (!done)
    {
        rb->lcd_clear_display();

        /* page one */
        if(screen == 1)
        {
            rb->lcd_puts(0, 0, "Using Clock " CLOCK_VERSION ":");
            rb->lcd_puts(0, 1, "To navigate this");
            rb->lcd_puts(0, 2, "help, use LEFT and");
            rb->lcd_puts(0, 3, "RIGHT. F1 returns");
            rb->lcd_puts(0, 4, "you to the clock.");
            rb->lcd_puts(0, 6, "");
            rb->lcd_puts(0, 7, "<< ---- 1/9 NEXT >>");
        }
        else if(screen == 2)
        {
            rb->lcd_puts(0, 0, "At any mode, [F1]");
            rb->lcd_puts(0, 1, "will show you this");
            rb->lcd_puts(0, 2, "help screen. [F2]");
            rb->lcd_puts(0, 3, "will start/stop");
            rb->lcd_puts(0, 4, "the Counter. Hold");
            rb->lcd_puts(0, 5, "F2 to reset the");
            rb->lcd_puts(0, 6, "Counter.");
            rb->lcd_puts(0, 7, "<< BACK 2/9 NEXT >>");
        }
        else if(screen == 3)
        {
            rb->lcd_puts(0, 0, "At any mode, [F3]");
            rb->lcd_puts(0, 1, "opens the Options");
            rb->lcd_puts(0, 2, "screen. In Options");
            rb->lcd_puts(0, 3, "move the cursor");
            rb->lcd_puts(0, 4, "with UP/DOWN and");
            rb->lcd_puts(0, 5, "use PLAY to toggle");
            rb->lcd_puts(0, 6, "selected option.");
            rb->lcd_puts(0, 7, "<< BACK 3/9 NEXT >>");
        }
        else if(screen == 4)
        {
            rb->lcd_puts(0, 0, "[ON] at any mode");
            rb->lcd_puts(0, 1, "will present you");
            rb->lcd_puts(0, 2, "with a credits roll");
            rb->lcd_puts(0, 3, "[PLAY] from any");
            rb->lcd_puts(0, 4, "mode will show the");
            rb->lcd_puts(0, 5, "MODE SELECTOR. Use");
            rb->lcd_puts(0, 6, "UP/DOWN to select");
            rb->lcd_puts(0, 7, "<< BACK 4/9 NEXT >>");
        }
        else if(screen == 5)
        {
            rb->lcd_puts(0, 0, "a mode and PLAY to");
            rb->lcd_puts(0, 1, "go to it.");
            rb->lcd_puts(0, 2, "_-=CLOCK MODES=-_");
            rb->lcd_puts(0, 3, "*ANALOG: Shows a");
            rb->lcd_puts(0, 4, "small round clock");
            rb->lcd_puts(0, 5, "in the center of");
            rb->lcd_puts(0, 6, "LCD. Options appear");
            rb->lcd_puts(0, 7, "<< BACK 5/9 NEXT >>");
        }
        else if(screen == 6)
        {
            rb->lcd_puts(0, 0, "around it.");
            rb->lcd_puts(0, 1, "*DIGITAL: Shows an");
            rb->lcd_puts(0, 2, "LCD imitation with");
            rb->lcd_puts(0, 3, "virtual 'segments'.");
            rb->lcd_puts(0, 4, "*LCD: Shows another");
            rb->lcd_puts(0, 5, "imitation of an");
            rb->lcd_puts(0, 6, "LCD display.");
            rb->lcd_puts(0, 7, "<< BACK 6/9 NEXT >>");
        }
        else if(screen == 7)
        {
            rb->lcd_puts(0, 0, "*FULLSCREEN: Like");
            rb->lcd_puts(0, 1, "Analog mode, but");
            rb->lcd_puts(0, 2, "uses the whole LCD.");
            rb->lcd_puts(0, 3, "Less options are");
            rb->lcd_puts(0, 4, "available in this");
            rb->lcd_puts(0, 5, "mode.");
            rb->lcd_puts(0, 6, "*BINARY: Shows a");
            rb->lcd_puts(0, 7, "<< BACK 7/9 NEXT >>");
        }
        else if(screen == 8)
        {
            rb->lcd_puts(0, 0, "binary clock. For");
            rb->lcd_puts(0, 1, "help on reading");
            rb->lcd_puts(0, 2, "binary, see:");
            rb->lcd_puts_scroll(0, 3, "http://en.wikipedia.org/wiki/Binary_numeral_system");
            rb->lcd_puts(0, 4, "_-=OTHER KEYS=-_");
            rb->lcd_puts(0, 5, "[DWN] will disable");
            rb->lcd_puts(0, 6, "Rockbox's idle");
            rb->lcd_puts(0, 7, "<< BACK 8/9 NEXT >>");
        }
        else if(screen == 9)
        {
            rb->lcd_puts(0, 0, "poweroff feature.");
            rb->lcd_puts(0, 1, "[UP] will reenable");
            rb->lcd_puts(0, 2, "it. [LEFT] will");
            rb->lcd_puts(0, 3, "turn off the back-");
            rb->lcd_puts(0, 4, " light, [RIGHT]");
            rb->lcd_puts(0, 5, "will turn it on.");
            rb->lcd_puts(0, 6, "[OFF] exits plugin.");
            rb->lcd_puts(0, 7, "<< BACK 9/9 ---- >>");
        }

        rb->lcd_update();

        switch (rb->button_get(true))
        {
            case BUTTON_F1: /* exit */
            case BUTTON_OFF:
                done = true;
                break;

            case BUTTON_LEFT:
                if(screen > 1)
                    screen --;
                break;

            case BUTTON_RIGHT:
                if(screen < 9)
                    screen++;
                break;

            case SYS_USB_CONNECTED: /* leave for usb */
                return PLUGIN_USB_CONNECTED;
                rb->usb_screen();
                break;
        }
    }
    return true;
}

/**************************
 * F2 Screen - Pick a Logo
 * [[ NO LONGER USED ]]
 **************************/
void f2_screen(void)
{
    rb->lcd_clear_display();
    rb->splash(HZ, true, "LEFT/RIGHT to choose logo");
    rb->lcd_update();
    rb->lcd_clear_display();
    rb->splash(HZ, true, "F2 to select and exit");
    rb->lcd_update();
    rb->lcd_clear_display();

    /* let's make sure that the logo isn't invalid */
    if(logo > 4)
        logo = 1;

    done = false;

    while(!done)
    {
        show_logo(false, false);

        rb->lcd_clearrect(0, 48, 112, 16);

        if(logo == 1)
        {
            rb->lcd_puts(0, 0, "1) 'Default'");
            rb->lcd_puts(0, 6, "By Zakk Roberts");
        }
        else if(logo == 2)
        {
            rb->lcd_puts(0, 0, "2) 'Digital'");
            rb->lcd_puts(0, 6, "By Zakk Roberts");
        }
        else if(logo == 3)
        {
            rb->lcd_puts(0, 0, "3) 'Electroplate'");
            rb->lcd_puts(0, 6, "By Adam S. and");
            rb->lcd_puts(0, 7, "Zakk Roberts");
        }
        else
        {
            rb->lcd_puts(0, 0, "4) 'Clockbox'");
            rb->lcd_puts(0, 6, "By Adam S.");
        }

        rb->lcd_update();

        switch(rb->button_get_w_tmo(HZ/4))
        {
            case BUTTON_F2:
            case BUTTON_OFF:
                done = true;
                break;

            case BUTTON_RIGHT:
                if(logo < 4)
                    logo++;
                break;

            case BUTTON_LEFT:
                if(logo > 1)
                    logo--;
                break;
        }
    }
}

/**********************
 * F3 Screen - OPTIONS
 **********************/
bool f3_screen(void)
{
    int invert_analog = 1;
    int invert_digital = 1;
    int invert_lcd = 1;
    int invert_fullscreen = 1;

    done = false;

    while (!done)
    {
        rb->lcd_clear_display();

        if(settings.clock == 1)
        {
            rb->lcd_puts(0, 0, "OPTIONS (Analog)");
            rb->lcd_puts(0, 1, "UP/DOWN & PLAY");
            rb->lcd_puts(0, 2, "F3/OFF: Done");
            rb->lcd_puts(2, 4, "Digits");

            if(settings.analog_date == 0)
                rb->lcd_puts(2, 5, "Date");
            else if(settings.analog_date == 1)
                rb->lcd_puts(2, 5, "Date: American");
            else
                rb->lcd_puts(2, 5, "Date: European");

            if(settings.analog_time == 0)
                rb->lcd_puts(2, 6, "Time Readout");
            else if(settings.analog_time == 1)
                rb->lcd_puts(2, 6, "Time Readout 24h");
            else
                rb->lcd_puts(2, 6, "Time Readout 12h");

            rb->lcd_puts(2, 7, "Second Hand");

            /* Draw checkboxes using the new checkbox() function */
            rb->checkbox(1, 33, 8, 6, settings.analog_digits);
            if(settings.analog_date != 0)
                rb->checkbox(1, 41, 8, 6, true);
            else
                rb->checkbox(1, 41, 8, 6, false);
            rb->checkbox(1, 49, 8, 6, settings.analog_time);
            rb->checkbox(1, 57, 8, 6, settings.analog_secondhand);

            /* Draw line selector */
            switch(invert_analog)
            {
                case 1:
                    rb->lcd_invertrect(0, 32, 112, 8);    break;

                case 2:
                    rb->lcd_invertrect(0, 40, 112, 8);    break;

                case 3:
                    rb->lcd_invertrect(0, 48, 112, 8);    break;

                case 4:
                    rb->lcd_invertrect(0, 56, 112, 8);    break;
            }

            rb->lcd_update();

            switch(rb->button_get_w_tmo(HZ/4))
            {
                case BUTTON_UP:
                    if(invert_analog > 1)
                        invert_analog--;
                    else
                        invert_analog = 4;
                    break;

                case BUTTON_DOWN:
                    if(invert_analog < 4)
                        invert_analog++;
                    else
                        invert_analog = 1;
                    break;

                case BUTTON_PLAY:
                    if(invert_analog == 1)
                        settings.analog_digits = !settings.analog_digits;
                    else if(invert_analog == 2)
                    {
                        if(settings.analog_date < 2)
                            settings.analog_date++;
                        else
                            settings.analog_date = 0;
                    }
                    else if(invert_analog == 3)
                    {
                        if(settings.analog_time < 2)
                            settings.analog_time++;
                        else
                            settings.analog_time = 0;
                    }
                    else
                        settings.analog_secondhand = !settings.analog_secondhand;
                    break;

                case BUTTON_F3:
                case BUTTON_OFF:
                    done = true;
                    break;
            }
        }
        else if(settings.clock == 2)
        {
            rb->lcd_puts(0, 0, "OPTIONS (Digital)");
            rb->lcd_puts(0, 1, "UP/DOWN & PLAY");
            rb->lcd_puts(0, 2, "F3/OFF: Done");

            if(settings.digital_date == 0)
                rb->lcd_puts(2, 4, "Date");
            else if(settings.digital_date == 1)
                rb->lcd_puts(2, 4, "Date: American");
            else
                rb->lcd_puts(2, 4, "Date: European");

            if(settings.digital_seconds == 0)
                rb->lcd_puts(2, 5, "Seconds");
            else if(settings.digital_seconds == 1)
                rb->lcd_puts(2, 5, "Seconds: DIGITAL");
            else if(settings.digital_seconds == 2)
                rb->lcd_puts(2, 5, "Seconds: BAR");
            else
                rb->lcd_puts(2, 5, "Seconds: INVERSE");

            rb->lcd_puts(2, 6, "Blinking Colon");
            rb->lcd_puts(2, 7, "12-Hour Format");

            /* Draw checkboxes */
            if(settings.digital_date != 0)
                rb->checkbox(1, 33, 8, 6, true);
            else
                rb->checkbox(1, 33, 8, 6, false);
            if(settings.digital_seconds != 0)
                rb->checkbox(1, 41, 8, 6, true);
            else
                rb->checkbox(1, 41, 8, 6, false);
            rb->checkbox(1, 49, 8, 6, settings.digital_blinkcolon);
            rb->checkbox(1, 57, 8, 6, settings.digital_12h);

            /* Draw a line selector */
            switch(invert_digital)
            {
                case 1:
                    rb->lcd_invertrect(0, 32, 112, 8);    break;

                case 2:
                    rb->lcd_invertrect(0, 40, 112, 8);    break;

                case 3:
                    rb->lcd_invertrect(0, 48, 112, 8);    break;

                case 4:
                    rb->lcd_invertrect(0, 56, 112, 8);    break;
            }

            rb->lcd_update();

            switch(rb->button_get_w_tmo(HZ/4))
            {
                case BUTTON_UP:
                    if(invert_digital == 1)
                        invert_digital = 4;
                    else
                        invert_digital--;
                    break;

                case BUTTON_DOWN:
                    if(invert_digital == 4)
                        invert_digital = 1;
                    else
                        invert_digital++;
                    break;

                case BUTTON_PLAY:
                    if(invert_digital == 1)
                    {
                        if(settings.digital_date < 2)
                            settings.digital_date++;
                        else
                            settings.digital_date = 0;
                    }
                    else if(invert_digital == 2)
                    {
                        if(settings.digital_seconds < 3)
                            settings.digital_seconds++;
                        else
                            settings.digital_seconds = 0;
                    }
                    else if(invert_digital == 3)
                        settings.digital_blinkcolon = !settings.digital_blinkcolon;
                    else
                        settings.digital_12h = !settings.digital_12h;
                    break;

                case BUTTON_F3:
                case BUTTON_OFF:
                    done = true;
                    break;
            }
        }
        else if(settings.clock == 3)
        {
            rb->lcd_puts(0, 0, "OPTIONS (LCD)");
            rb->lcd_puts(0, 1, "UP/DOWN & PLAY");
            rb->lcd_puts(0, 2, "F3/OFF: Done");

            if(settings.lcd_date == 0)
                rb->lcd_puts(2, 4, "Date");
            else if(settings.lcd_date == 1)
                rb->lcd_puts(2, 4, "Date: American");
            else
                rb->lcd_puts(2, 4, "Date: European");

            if(settings.lcd_seconds == 0)
                rb->lcd_puts(2, 5, "Seconds");
            else if(settings.lcd_seconds == 1)
                rb->lcd_puts(2, 5, "Seconds: DIGITAL");
            else if(settings.lcd_seconds == 2)
                rb->lcd_puts(2, 5, "Seconds: BAR");
            else
                rb->lcd_puts(2, 5, "Seconds: INVERSE");

            rb->lcd_puts(2, 6, "Blinking Colon");
            rb->lcd_puts(2, 7, "12-Hour Format");

            /* Draw checkboxes */
            if(settings.lcd_date != 0)
                rb->checkbox(1, 33, 8, 6, true);
            else
                rb->checkbox(1, 33, 8, 6, false);
            if(settings.lcd_seconds != 0)
                rb->checkbox(1, 41, 8, 6, true);
            else
                rb->checkbox(1, 41, 8, 6, false);
            rb->checkbox(1, 49, 8, 6, settings.lcd_blinkcolon);
            rb->checkbox(1, 57, 8, 6, settings.lcd_12h);

            /* Draw a line selector */
            switch(invert_lcd)
            {
                case 1:
                    rb->lcd_invertrect(0, 32, 112, 8);    break;

                case 2:
                    rb->lcd_invertrect(0, 40, 112, 8);    break;

                case 3:
                    rb->lcd_invertrect(0, 48, 112, 8);    break;

                case 4:
                    rb->lcd_invertrect(0, 56, 112, 8);    break;
            }

            rb->lcd_update();

            switch(rb->button_get_w_tmo(HZ/4))
            {
                case BUTTON_UP:
                    if(invert_lcd == 1)
                        invert_lcd = 4;
                    else
                        invert_lcd--;
                    break;

                case BUTTON_DOWN:
                    if(invert_lcd == 4)
                        invert_lcd = 1;
                    else
                        invert_lcd++;
                    break;

                case BUTTON_PLAY:
                    if(invert_lcd == 1)
                    {
                        if(settings.lcd_date < 2)
                            settings.lcd_date++;
                        else
                            settings.lcd_date = 0;
                    }
                    else if(invert_lcd == 2)
                    {
                        if(settings.lcd_seconds < 3)
                            settings.lcd_seconds++;
                        else
                            settings.lcd_seconds = 0;
                    }
                    else if(invert_lcd == 3)
                        settings.lcd_blinkcolon = !settings.lcd_blinkcolon;
                    else
                        settings.lcd_12h = !settings.lcd_12h;
                    break;

                case BUTTON_F3:
                case BUTTON_OFF:
                    done = true;
                    break;
            }
        }
        else if(settings.clock == 4)
        {
            rb->lcd_puts(0, 0, "OPTIONS (Full)");
            rb->lcd_puts(0, 1, "UP/DOWN & PLAY");
            rb->lcd_puts(0, 2, "F3/OFF: Done");

            rb->lcd_puts(2, 4, "Border");
            rb->lcd_puts(2, 5, "Second Hand");
            rb->lcd_puts(2, 6, "Invert Seconds");

            rb->checkbox(1, 33, 8, 6, settings.fullscreen_border);
            rb->checkbox(1, 41, 8, 6, settings.fullscreen_secondhand);
            rb->checkbox(1, 49, 8, 6, settings.fullscreen_invertseconds);

            /* Draw a line selector
             * There are 4 values here in case we decide to "up" the amount of settings */
            switch(invert_fullscreen)
            {
                case 1:
                    rb->lcd_invertrect(0, 32, 112, 8);    break;

                case 2:
                    rb->lcd_invertrect(0, 40, 112, 8);    break;

                case 3:
                    rb->lcd_invertrect(0, 48, 112, 8);    break;

                case 4:
                    rb->lcd_invertrect(0, 56, 112, 8);    break;
            }

            rb->lcd_update();

            switch(rb->button_get_w_tmo(HZ/4))
            {
                case BUTTON_UP:
                    if(invert_fullscreen == 1)
                        invert_fullscreen = 3;
                    else
                        invert_fullscreen--;
                    break;

                case BUTTON_DOWN:
                    if(invert_fullscreen == 3)
                        invert_fullscreen = 1;
                    else
                        invert_fullscreen++;
                    break;

                case BUTTON_PLAY:
                    if(invert_fullscreen == 1)
                        settings.fullscreen_border = !settings.fullscreen_border;
                    else if(invert_fullscreen == 2)
                        settings.fullscreen_secondhand = !settings.fullscreen_secondhand;
                    else
                        settings.fullscreen_invertseconds = !settings.fullscreen_invertseconds;
                    break;

                case BUTTON_F3:
                case BUTTON_OFF:
                    done = true;
                    break;
            }
        }
        else
        {
            rb->lcd_puts(0, 0, "OPTIONS (Binary)");
            rb->lcd_puts(0, 1, "UP/DOWN & PLAY");
            rb->lcd_puts(0, 2, "F3/OFF: Done");

            rb->lcd_puts(2, 4, "-- NO OPTIONS --");
            rb->lcd_puts(1, 6, "F3/OFF to return");

            rb->lcd_update();

            switch(rb->button_get_w_tmo(HZ/4))
            {

                case BUTTON_F3:
                case BUTTON_OFF:
                    done = true;
                    break;
            }
        }
    }
    return true;
}

/*****************************************
 * Draws the extras, IE border, digits...
 *****************************************/
void draw_extras(int year, int day, int month, int hour, int minute, int second)
{
    char buf[11];
    int w, h;
    int i;
    int tempyear = 0;

    struct tm* current_time = rb->get_time();

    int fill = LCD_WIDTH * second / 60;

    char moday[8];
    char dateyr[6];
    char tmhrmin[7];
    char tmsec[3];

    if(year == 2004)
        tempyear = 04;

    /* american date readout */
    if(settings.analog_date == 1)
        rb->snprintf(moday, sizeof(moday), "%02d/%02d", month, day);
    else
        rb->snprintf(moday, sizeof(moday), "%02d.%02d", day, month);
    rb->snprintf(dateyr, sizeof(dateyr), "%d", year);
    rb->snprintf(tmhrmin, sizeof(tmhrmin), "%02d:%02d", hour, minute);
    rb->snprintf(tmsec, sizeof(tmsec), "%02d", second);

    /* Analog Extras */
    if(settings.clock == 1)
    {
        /* DIGITS around the face */
        if (settings.analog_digits)
        {
            rb->lcd_putsxy((LCD_WIDTH/2)-6, 0, "12");
            rb->lcd_putsxy(20, (LCD_HEIGHT/2)-4, "9");
            rb->lcd_putsxy((LCD_WIDTH/2)-4, 56, "6");
            rb->lcd_putsxy(86, (LCD_HEIGHT/2)-4, "3");
        }

        /* Digital readout */
        if (settings.analog_time != 0)
        {
            /* HH:MM */
            rb->lcd_putsxy(1, 4, tmhrmin);
            /* SS */
            rb->lcd_putsxy(10, 12, tmsec);

            /* AM/PM indicator */
            if(settings.analog_time == 2)
            {
                if(current_time->tm_hour > 12) /* PM */
                    rb->lcd_bitmap(pm, 96, 1, 15, 8, true);
                else /* AM */
                    rb->lcd_bitmap(am, 96, 1, 15, 8, true);
            }
        }

        /* Date readout */
        if(settings.analog_date != 0)
        {
            /* MM-DD (or DD.MM) */
            rb->lcd_putsxy(1, 48, moday);
            rb->lcd_putsxy(3, 56, dateyr);
        }
    }
    else if(settings.clock == 2)
    {
        /* Date readout */
        if(settings.digital_date == 1) /* american mode */
        {
            rb->snprintf(buf, sizeof(buf), "%d/%d/%d", month, day, year);
            rb->lcd_getstringsize(buf, &w, &h);
            rb->lcd_putsxy((LCD_WIDTH/2)-(w/2), 56, buf);
        }
        else if(settings.digital_date == 2) /* european mode */
        {
            rb->snprintf(buf, sizeof(buf), "%d.%d.%d", day, month, year);
            rb->lcd_getstringsize(buf, &w, &h);
            rb->lcd_putsxy((LCD_WIDTH/2)-(w/2), 56, buf);
        }

        /* Second readout */
        if(settings.digital_seconds == 1)
        {
            rb->snprintf(buf, sizeof(buf), "%d", second);
            rb->lcd_getstringsize(buf, &w, &h);
            rb->lcd_putsxy((LCD_WIDTH/2)-(w/2), 5, buf);
        }

        /* Second progressbar */
        if(settings.digital_seconds == 2)
        {
            rb->scrollbar(0, 0, 112, 4, 60, 0, second, HORIZONTAL);
        }

        /* Invert the whole LCD as the seconds go */
        if(settings.digital_seconds == 3)
        {
            rb->lcd_invertrect(0, 0, fill, 64);
        }
    }
    else if(settings.clock == 3) /* LCD mode */
    {
        /* Date readout */
        if(settings.lcd_date == 1) /* american mode */
        {
            rb->snprintf(buf, sizeof(buf), "%d/%d/%d", month, day, year);
            rb->lcd_getstringsize(buf, &w, &h);
            rb->lcd_putsxy((LCD_WIDTH/2)-(w/2), 56, buf);
        }
        else if(settings.lcd_date == 2) /* european mode */
        {
            rb->snprintf(buf, sizeof(buf), "%d.%d.%d", day, month, year);
            rb->lcd_getstringsize(buf, &w, &h);
            rb->lcd_putsxy((LCD_WIDTH/2)-(w/2), 56, buf);
        }

        /* Second readout */
        if(settings.lcd_seconds == 1)
        {
            rb->snprintf(buf, sizeof(buf), "%d", second);
            rb->lcd_getstringsize(buf, &w, &h);
            rb->lcd_putsxy((LCD_WIDTH/2)-(w/2), 5, buf);
        }

        /* Second progressbar */
        if(settings.lcd_seconds == 2)
        {
            rb->scrollbar(0, 0, 112, 4, 60, 0, second, HORIZONTAL);
        }

        /* Invert the whole LCD as the seconds go */
        if(settings.lcd_seconds == 3)
        {
            rb->lcd_invertrect(0, 0, fill, 64);
        }
    }
    else if(settings.clock == 4) /* Fullscreen mode */
    {
        if(settings.fullscreen_border)
        {
            /* Draw the circle */
            for(i=0; i < 60; i+=5)
            {
                rb->lcd_drawpixel(xminute_full[i],
                                  yminute_full[i]);
                rb->lcd_drawrect(xminute_full[i]-1,
                                 yminute_full[i]-1,
                                 3, 3);
            }
        }
        if(settings.fullscreen_invertseconds)
        {
            rb->lcd_invertrect(0, 0, fill, 64);
        }
    }
}

/****************
 * Select a mode
 ****************/
void select_mode(void)
{
    int cursorpos = settings.clock;
    bool a = false; /* (a)nalog */
    bool d = false; /* (d)igital */
    bool l = false; /* (l)cd */
    bool f = false; /* (f)ullscreen */
    bool b = false; /* (b)inary */

    done = false;

    while(!done)
    {
        rb->lcd_clear_display();

        rb->lcd_puts(0, 0, "MODE SELECTOR");
        rb->lcd_puts(2, 1, "Analog");
        rb->lcd_puts(2, 2, "Digital");
        rb->lcd_puts(2, 3, "LCD");
        rb->lcd_puts(2, 4, "Fullscreen");
        rb->lcd_puts(2, 5, "Binary");
        rb->lcd_puts(0, 6, "UP/DOWN: Choose");
        rb->lcd_puts(0, 7, "PLAY:Go|OFF:Cancel");

        switch(settings.clock)
        {
            case 1: a=true; d=l=f=b=false; break;
            case 2: d=true; a=l=f=b=false; break;
            case 3: l=true; a=d=f=b=false; break;
            case 4: f=true; a=d=l=b=false; break;
            case 5: b=true; a=d=l=f=false; break;
        }

        /* draw a checkbox next to current mode */
        rb->checkbox(1, 9, 8, 6, a);
        rb->checkbox(1, 17, 8, 6, d);
        rb->checkbox(1, 25, 8, 6, l);
        rb->checkbox(1, 33, 8, 6, f);
        rb->checkbox(1, 41, 8, 6, b);

        /* draw line selector */
        switch(cursorpos)
        {
            case 1: rb->lcd_invertrect(0, 8, 112, 8);  break;
            case 2: rb->lcd_invertrect(0, 16, 112, 8); break;
            case 3: rb->lcd_invertrect(0, 24, 112, 8); break;
            case 4: rb->lcd_invertrect(0, 32, 112, 8); break;
            case 5: rb->lcd_invertrect(0, 40, 112, 8); break;
        }

        rb->lcd_update();

        switch(rb->button_get_w_tmo(HZ/4))
        {
            case BUTTON_UP:
                if(cursorpos > 1)
                    cursorpos--;
                else
                    cursorpos = 5;
                break;

            case BUTTON_DOWN:
                if(cursorpos < 5)
                    cursorpos++;
                else
                    cursorpos = 1;
                break;

            case BUTTON_PLAY:
                settings.clock = cursorpos;
                done = true;
                break;

            case BUTTON_OFF:
                done = true;
                break;
        }
    }
}

void show_counter(void)
{
    if(counting)
    {
        passed_time = *rb->current_tick - start_tick;
    }
    else
        passed_time = 0;

    displayed_value = counter + passed_time;
    displayed_value = displayed_value / HZ;

    count_s = displayed_value % 60;
    count_m = displayed_value % 3600 / 60;
    count_h = displayed_value / 3600;

    rb->snprintf(count_text, sizeof(count_text), "%d:%02d:%02d", count_h, count_m, count_s);

    if(settings.clock == 1)
        rb->lcd_puts(11, 7, count_text);
    else if(settings.clock == 2)
        rb->lcd_putsxy(1, 5, count_text);
    else if(settings.clock == 3)
        rb->lcd_putsxy(1, 5, count_text);
    else if(settings.clock == 4)
        rb->lcd_puts(6, 6, count_text);
}

/**********************************************************************
 * Plugin starts here
 **********************************************************************/
enum plugin_status plugin_start(struct plugin_api* api, void* parameter)
{
    /* time ints */
    int i;
    int hour;
    int minute;
    int second;
    int temphour;
    int last_second = -1;
    int pos = 0;

    /* date ints */
    int year;
    int day;
    int month;

    /* poweroff activated or not? */
    bool reset_timer = false;

    bool f2_held = false;

    struct tm* current_time;

    TEST_PLUGIN_API(api);
    (void)parameter;
    rb = api;

    load_settings();

    /* universal font */
    rb->lcd_setfont(FONT_SYSFIXED);

    /* set backlight timeout */
    rb->backlight_set_timeout(settings.backlight_on);

    while (!PLUGIN_OK)
    {
        /*********************
         * Time info
         *********************/
        current_time = rb->get_time();
        hour = current_time->tm_hour;
        minute = current_time->tm_min;
        second = current_time->tm_sec;
        temphour = current_time->tm_hour;

        /*********************
         * Date info
         *********************/
        year = current_time->tm_year + 1900;
        day = current_time->tm_mday;
        month = current_time->tm_mon + 1;

        done = false;

        if(second != last_second)
        {
            rb->lcd_clear_display();

            show_counter();

            /**************
             * Analog Mode
             **************/
            if(settings.clock == 1)
            {
                /* Second hand */
                if(settings.analog_secondhand)
                {
                    pos = 90-second;
                    if(pos >= 60)
                        pos -= 60;

                    rb->lcd_drawline((LCD_WIDTH/2), (LCD_HEIGHT/2),
                                     xminute[pos], yminute[pos]);
                }

                pos = 90-minute;
                if(pos >= 60)
                    pos -= 60;

                /* Minute hand, thicker than the second hand */
                rb->lcd_drawline(LCD_WIDTH/2, LCD_HEIGHT/2,
                                 xminute[pos], yminute[pos]);
                rb->lcd_drawline(LCD_WIDTH/2-1, LCD_HEIGHT/2-1,
                                 xminute[pos], yminute[pos]);
                rb->lcd_drawline(LCD_WIDTH/2+1, LCD_HEIGHT/2+1,
                                 xminute[pos], yminute[pos]);
                rb->lcd_drawline(LCD_WIDTH/2-1, LCD_HEIGHT/2+1,
                                 xminute[pos], yminute[pos]);
                rb->lcd_drawline(LCD_WIDTH/2+1, LCD_HEIGHT/2-1,
                                 xminute[pos], yminute[pos]);

                if(hour > 12)
                    hour -= 12;

                hour = hour*5 + minute/12;;
                pos = 90-hour;
                if(pos >= 60)
                    pos -= 60;

                /* Hour hand, thick as the minute hand but shorter */
                rb->lcd_drawline(LCD_WIDTH/2, LCD_HEIGHT/2, xhour[pos], yhour[pos]);
                rb->lcd_drawline(LCD_WIDTH/2-1, LCD_HEIGHT/2-1,
                                 xhour[pos], yhour[pos]);
                rb->lcd_drawline(LCD_WIDTH/2+1, LCD_HEIGHT/2+1,
                                 xhour[pos], yhour[pos]);
                rb->lcd_drawline(LCD_WIDTH/2-1, LCD_HEIGHT/2+1,
                                 xhour[pos], yhour[pos]);
                rb->lcd_drawline(LCD_WIDTH/2+1, LCD_HEIGHT/2-1,
                                 xhour[pos], yhour[pos]);

                /* Draw the circle */
                for(i=0; i < 60; i+=5)
                {
                    rb->lcd_drawpixel(xminute[i],
                                      yminute[i]);
                    rb->lcd_drawrect(xminute[i]-1,
                                     yminute[i]-1,
                                     3, 3);
                }

                /* Draw the cover over the center */
                rb->lcd_drawline((LCD_WIDTH/2)-1, (LCD_HEIGHT/2)+3,
                                 (LCD_WIDTH/2)+1, (LCD_HEIGHT/2)+3);
                rb->lcd_drawline((LCD_WIDTH/2)-3, (LCD_HEIGHT/2)+2,
                                 (LCD_WIDTH/2)+3, (LCD_HEIGHT/2)+2);
                rb->lcd_drawline((LCD_WIDTH/2)-4, (LCD_HEIGHT/2)+1,
                                 (LCD_WIDTH/2)+4, (LCD_HEIGHT/2)+1);
                rb->lcd_drawline((LCD_WIDTH/2)-4, LCD_HEIGHT/2,
                                 (LCD_WIDTH/2)+4, LCD_HEIGHT/2);
                rb->lcd_drawline((LCD_WIDTH/2)-4, (LCD_HEIGHT/2)-1,
                                 (LCD_WIDTH/2)+4, (LCD_HEIGHT/2)-1);
                rb->lcd_drawline((LCD_WIDTH/2)-3, (LCD_HEIGHT/2)-2,
                                 (LCD_WIDTH/2)+3, (LCD_HEIGHT/2)-2);
                rb->lcd_drawline((LCD_WIDTH/2)-1, (LCD_HEIGHT/2)-3,
                                 (LCD_WIDTH/2)+1, (LCD_HEIGHT/2)-3);
                rb->lcd_drawpixel(LCD_WIDTH/2, LCD_HEIGHT/2);


            }
            /***************
             * Digital mode
             ***************/
            else if(settings.clock == 2)
            {
                if(settings.digital_blinkcolon)
                    draw_7seg_time(hour, minute, 8, 16, 16, 32, second & 1, false);
                else
                    draw_7seg_time(hour, minute, 8, 16, 16, 32, true, false);
            }
            /***********
             * LCD mode
             ***********/
            else if(settings.clock == 3)
            {
                if(settings.lcd_blinkcolon)
                    draw_7seg_time(hour, minute, 8, 16, 16, 32, second & 1, true);
                else
                    draw_7seg_time(hour, minute, 8, 16, 16, 32, true, true);
            }
            /******************
             * Fullscreen mode
             ******************/
            else if(settings.clock == 4)
            {
                /* Second hand */
                if(settings.fullscreen_secondhand)
                {
                    pos = 90-second;
                    if(pos >= 60)
                        pos -= 60;

                    rb->lcd_drawline((LCD_WIDTH/2), (LCD_HEIGHT/2),
                                     xminute_full[pos], yminute_full[pos]);
                }

                pos = 90-minute;
                if(pos >= 60)
                    pos -= 60;

                /* Minute hand, thicker than the second hand */
                rb->lcd_drawline(LCD_WIDTH/2, LCD_HEIGHT/2,
                                 xminute_full[pos], yminute_full[pos]);
                rb->lcd_drawline(LCD_WIDTH/2-1, LCD_HEIGHT/2-1,
                                 xminute_full[pos], yminute_full[pos]);
                rb->lcd_drawline(LCD_WIDTH/2+1, LCD_HEIGHT/2+1,
                                 xminute_full[pos], yminute_full[pos]);
                rb->lcd_drawline(LCD_WIDTH/2-1, LCD_HEIGHT/2+1,
                                 xminute_full[pos], yminute_full[pos]);
                rb->lcd_drawline(LCD_WIDTH/2+1, LCD_HEIGHT/2-1,
                                 xminute_full[pos], yminute_full[pos]);

                if(hour > 12)
                    hour -= 12;

                hour = hour*5 + minute/12;
                pos = 90-hour;
                if(pos >= 60)
                    pos -= 60;

                /* Hour hand, thick as the minute hand but shorter */
                rb->lcd_drawline(LCD_WIDTH/2, LCD_HEIGHT/2, xhour_full[pos], yhour_full[pos]);
                rb->lcd_drawline(LCD_WIDTH/2-1, LCD_HEIGHT/2-1,
                                 xhour_full[pos], yhour_full[pos]);
                rb->lcd_drawline(LCD_WIDTH/2+1, LCD_HEIGHT/2+1,
                                 xhour_full[pos], yhour_full[pos]);
                rb->lcd_drawline(LCD_WIDTH/2-1, LCD_HEIGHT/2+1,
                                 xhour_full[pos], yhour_full[pos]);
                rb->lcd_drawline(LCD_WIDTH/2+1, LCD_HEIGHT/2-1,
                                 xhour_full[pos], yhour_full[pos]);

                /* Draw the cover over the center */
                rb->lcd_drawline((LCD_WIDTH/2)-1, (LCD_HEIGHT/2)+3,
                                 (LCD_WIDTH/2)+1, (LCD_HEIGHT/2)+3);
                rb->lcd_drawline((LCD_WIDTH/2)-3, (LCD_HEIGHT/2)+2,
                                 (LCD_WIDTH/2)+3, (LCD_HEIGHT/2)+2);
                rb->lcd_drawline((LCD_WIDTH/2)-4, (LCD_HEIGHT/2)+1,
                                 (LCD_WIDTH/2)+4, (LCD_HEIGHT/2)+1);
                rb->lcd_drawline((LCD_WIDTH/2)-4, LCD_HEIGHT/2,
                                 (LCD_WIDTH/2)+4, LCD_HEIGHT/2);
                rb->lcd_drawline((LCD_WIDTH/2)-4, (LCD_HEIGHT/2)-1,
                                 (LCD_WIDTH/2)+4, (LCD_HEIGHT/2)-1);
                rb->lcd_drawline((LCD_WIDTH/2)-3, (LCD_HEIGHT/2)-2,
                                 (LCD_WIDTH/2)+3, (LCD_HEIGHT/2)-2);
                rb->lcd_drawline((LCD_WIDTH/2)-1, (LCD_HEIGHT/2)-3,
                                 (LCD_WIDTH/2)+1, (LCD_HEIGHT/2)-3);
                rb->lcd_drawpixel(LCD_WIDTH/2, LCD_HEIGHT/2);
            }
            /**************
             * Binary mode
             **************/
            else
            {
                binary(hour, minute, second);
            }
        }

        if(settings.analog_time == 2 && temphour > 12)
            temphour -= 12;

        draw_extras(year, day, month, temphour, minute, second);

        if(reset_timer)
            rb->reset_poweroff_timer();

        rb->lcd_update();

        /**************************
         * Scan for button presses
         **************************/
        switch (rb->button_get_w_tmo(HZ/10))
        {
            case BUTTON_OFF: /* save and exit */
                save_settings();
                rb->backlight_set_timeout(rb->global_settings->backlight_timeout);
                return PLUGIN_OK;
                break;

            case BUTTON_ON: /* credit roll */
                show_credits();
                break;

            case BUTTON_F1: /* help */
                f1_screen();
                break;

            case BUTTON_F2|BUTTON_REL: /* start/stop counter */
                if(settings.clock != 5)
                {
                    /* Ignore if the counter was reset */
                    if(!f2_held)
                    {
                        if(counting)
                        {
                            counting = false;
                            counter += passed_time;
                        }
                        else
                        {
                            counting = true;
                            start_tick = *rb->current_tick;
                        }
                    }
                    f2_held = false;
                }
                break;

            case BUTTON_F2 | BUTTON_REPEAT: /* reset counter */
                f2_held = true;  /* Ignore the release event */
                counter = 0;
                start_tick = *rb->current_tick;
                break;

            case BUTTON_F3: /* options */
                f3_screen();
                break;

            case BUTTON_UP: /* enable idle poweroff */
                reset_timer = false;
                rb->splash(HZ*2, true, "Idle Poweroff ENABLED");
                break;

            case BUTTON_DOWN: /* disable idle poweroff */
                reset_timer = true;
                rb->splash(HZ*2, true, "Idle Poweroff DISABLED");
                break;

            case BUTTON_LEFT: /* backlight off */
                settings.backlight_on = false;
                rb->splash(HZ, true, "Backlight OFF");
                rb->backlight_set_timeout(settings.backlight_on);
                break;

            case BUTTON_RIGHT: /* backlight on */
                settings.backlight_on = true;
                rb->splash(HZ, true, "Backlight ON");
                rb->backlight_set_timeout(settings.backlight_on);
                break;

            case BUTTON_PLAY: /* select a mode */
                select_mode();
                break;

            case SYS_USB_CONNECTED: /* usb plugged? */
                rb->usb_screen();
                return PLUGIN_USB_CONNECTED;
                break;
        }
    }
}
#endif
