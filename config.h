/* See LICENSE file for copyright and license details. */

/* key definitions */
#include <X11/XF86keysym.h>

/* Programs definitions */
#define TERMINAL "st"
#define BROWSER "librewolf"
#define VIDEO "mpv"
#define MUSIC "ncmpcpp"
#define SCREENS "screenshot"
#define SCREENLOCK "slock"

/* appearance */
static unsigned int borderpx = 5;        /* border pixel of windows */
static unsigned int snap     = 22;       /* snap pixel */
static int showbar           = 1;        /* 0 means no bar */
static int topbar            = 1;        /* 0 means bottom bar */
static Bool viewontag        = True;     /* Switch view on tag switch */
static const char *fonts[]         = {
	"monospace:size=12",
	"JoyPixels:size=12",
	"Noto Color Emoji:size=12",
	"WenQuanYi Zen Hei Mono:size=12"
};
static char dmenufont[]       = "Cascadia Mono:size=12";
static char normbgcolor[]     = "#222222";
static char normbordercolor[] = "#444444";
static char normfgcolor[]     = "#bbbbbb";
static char selfgcolor[]      = "#eeeeee";
static char selbordercolor[]  = "#005577";
static char selbgcolor[]      = "#005577";
static char col1[] = "#ffffff";
static char col2[] = "#ffffff";
static char col3[] = "#ffffff";
static char col4[] = "#ffffff";
static char col5[] = "#ffffff";
static char col6[] = "#ffffff";

enum { SchemeNorm, SchemeCol1, SchemeCol2, SchemeCol3, SchemeCol4,
       SchemeCol5, SchemeCol6, SchemeSel }; /* color schemes */

static const unsigned int baralpha    = 0xd0;
static const unsigned int borderalpha = OPAQUE;
static char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
	[SchemeCol1] = { col1,      normbgcolor, normbordercolor },
	[SchemeCol2] = { col2,      normbgcolor, normbordercolor },
	[SchemeCol3] = { col3,      normbgcolor, normbordercolor },
	[SchemeCol4] = { col4,      normbgcolor, normbordercolor },
	[SchemeCol5] = { col5,      normbgcolor, normbordercolor },
	[SchemeCol6] = { col6,      normbgcolor, normbordercolor },
	[SchemeSel]  = { selfgcolor,  selbordercolor, selbgcolor },
};
static const unsigned int alphas[][3]      = {
	/*               fg      bg        border     */
	[SchemeNorm] = { OPAQUE, baralpha, borderalpha },
	[SchemeSel]  = { OPAQUE, baralpha, borderalpha },
};

/* tagging */
// static const char *tags[] = {"\uf120", "\uf7ae", "\uf04b", "\uf121", "\ue62e", "\u266C", "\ue727", "\uf537", "\uf684"};
static const char *tags[] = {"1", "2", "3", "4", "5", "6"};

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "wps",         NULL,       NULL,       0,            1,           -1 },
	{ "Gimp",        NULL,       NULL,       0,            1,           -1 },
	{ "kicad",       NULL,       NULL,       0,            1,           -1 },
	{ "Etcher",      NULL,       NULL,       0,            1,           -1 },
	{ "chromium",    NULL,       NULL,       1 << 8,       0,           -1 },
	{ "Telegram",    NULL,       NULL,       0,            1,           -1 },
	{ "icalingua",   NULL,       NULL,       0,            1,           -1 },
};

/* layout(s) */
static float mfact     = 0.5; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

// #include "layouts.c"
#include "fibonacci.c"
static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "Tile", tile },    /* first entry is default */
	{ "Mono", monocle },
	{ "Floa", NULL },    /* no layout function means floating behavior */
	{  "|M|", centeredmaster },
	{  "[@]", spiral },
	{  ">M>", centeredfloatingmaster },
	{ "[\\]", dwindle },
};

// Alt
#define MODKEY Mod4Mask
// Win
#define ALTKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/zsh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2]         = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[]   = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbordercolor, "-sf", selfgcolor, NULL };
static const char *termcmd[]    = { TERMINAL, NULL};
static const char *browsercmd[] = { BROWSER, NULL };

/* Perpare to add screenshot function
 *
 * Wishlist
 * 1. Use Shortcut to take screenshot instead of printscreen, because my bluetooth-keyboard don't have this key
 * 2. Use xclip to save screenshot instead of save to disk
 * */

/* Volume control: amixer set Master percent(-/+)
 * Example: amixer set Master 10%-
 * */
static const char *volumeup[]         = { "sound", "4", NULL };
static const char *volumedown[]       = { "sound", "6", NULL };
static const char *volumemute[]       = { "sound", "2", NULL };
static const char *micmute[]          = { "pamixer", "--default-source", "-t", NULL };
static const char *backlightinc[]     = { "xbacklight", "-inc", "10", NULL};
static const char *backlightdec[]     = { "xbacklight", "-dec", "10", NULL};
static const char *screentools[]      = { "screentools", NULL};
static const char *screenshot[]       = { "screenshot", NULL };
static const char *webcam[]           = { VIDEO, "/dev/video0", NULL};
static const char *touchpadtoggle[]   = { "touchpadtoggle", NULL};
static const char *lockscreen[]       = { SCREENLOCK, NULL};

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
		// { "font",               STRING,  &font },
		{ "dmenufont",          STRING,  &dmenufont },
		{ "normbgcolor",        STRING,  &normbgcolor },
		{ "normbordercolor",    STRING,  &normbordercolor },
		{ "normfgcolor",        STRING,  &normfgcolor },
		{ "selbgcolor",         STRING,  &selbgcolor },
		{ "selbordercolor",     STRING,  &selbordercolor },
		{ "selfgcolor",         STRING,  &selfgcolor },
		{ "borderpx",          	INTEGER, &borderpx },
		{ "snap",          		INTEGER, &snap },
		{ "showbar",          	INTEGER, &showbar },
		{ "topbar",          	INTEGER, &topbar },
		{ "nmaster",          	INTEGER, &nmaster },
		{ "resizehints",       	INTEGER, &resizehints },
		{ "mfact",      	 	FLOAT,   &mfact },
};

static Key keys[] = {
	/* modifier                     key		   function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd     } },
	{ MODKEY|ShiftMask,             XK_g,      spawn,          {.v = browsercmd   } },
	{ MODKEY|ShiftMask,        XK_Return,      spawn,          {.v = termcmd      } },
	{ MODKEY|ShiftMask,             XK_d,      spawn,          SHCMD("prompts 'shutdown or reboot?' 'shutdown' 'reboot' 'no' 'sudo poweroff' 'sudo reboot' 'echo no'")},
	{ 0,                XF86XK_AudioMute,      spawn,          {.v = volumemute   } }, // 静音
	{ 0,             XF86XK_AudioMicMute,      spawn,          {.v = micmute      } }, // 静音
	{ 0,         XF86XK_AudioLowerVolume,      spawn,          {.v = volumedown   } }, // 音量减小
	{ 0,         XF86XK_AudioRaiseVolume,      spawn,          {.v = volumeup     } }, // 音量增加
	{ 0,        XF86XK_MonBrightnessDown,      spawn,          {.v = backlightdec } }, // screen light
	{ 0,          XF86XK_MonBrightnessUp,      spawn,          {.v = backlightinc } }, // screen light
	{ 0,              XF86XK_ScreenSaver,      spawn,          {.v = lockscreen   } }, // screen light
	{ 0,                   XF86XK_WebCam,      spawn,          {.v = webcam       } }, // screen light
	{ 0,           XF86XK_TouchpadToggle,      spawn,          {.v = touchpadtoggle     } }, // screen light
	{ 0,                        XK_Print,      spawn,          {.v = screenshot   } }, // screenshot to clipboard
	{ 0,                        XK_Print,      spawn,          {.v = screenshot   } }, // screenshot to clipboard
	{ ALTKEY|ShiftMask,             XK_5,      spawn,          {.v = screentools  } }, // screentools
	{ MODKEY,               XK_backslash,      spawn,          {.v = lockscreen   } },
	// { 0,                    XF86LockScreen,    spawn,          {.v = lockscreen   } },
	{ ALTKEY|ShiftMask,             XK_v,      spawn,          SHCMD("clipmenu")},
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_c,      setlayout,      {.v = &layouts[3]} },
	{ MODKEY,                       XK_o,      setlayout,      {.v = &layouts[4]} },
	{ MODKEY|ShiftMask,             XK_m,      setlayout,      {.v = &layouts[5]} },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                   XK_comma,      focusmon,       {.i = -1 } },
	{ MODKEY,                  XK_period,      focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,         XK_comma,      tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,        XK_period,      tagmon,         {.i = +1 } },
	{ MODKEY,                  XK_Return,      zoom,           {0} },
	{ MODKEY,                     XK_Tab,      view,           {0} },
	{ MODKEY,                       XK_q,      killclient,     {0} },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY|ShiftMask,             XK_f,      fullscreen,     {0} },
	{ MODKEY,                   XK_space,      setlayout,      {0} },
	{ MODKEY|ShiftMask,         XK_space,      togglefloating, {0} },
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button1,        toggleview,     {0} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button1,        sigdwmblocks,   {.i = 1} },
	{ ClkStatusText,        0,              Button2,        sigdwmblocks,   {.i = 2} },
	{ ClkStatusText,        0,              Button3,        sigdwmblocks,   {.i = 3} },
	{ ClkStatusText,        ControlMask,    Button1,        sigdwmblocks,   {.i = 4} },
	{ ClkStatusText,        ControlMask,    Button2,        sigdwmblocks,   {.i = 5} },
	{ ClkStatusText,        ControlMask,    Button3,        sigdwmblocks,   {.i = 6} },
	{ ClkClientWin,         MODKEY,                   Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY|ShiftMask,         Button1,        resizemouse,    {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
