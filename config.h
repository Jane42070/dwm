/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx    = 3;        /* border pixel of windows */
static const unsigned int gappx       = 20;        /* gaps between windows */
static const unsigned int snap        = 10;       /* snap pixel */
static const int showbar              = 1;        /* 0 means no bar */
static const int topbar               = 1;        /* 0 means bottom bar */
static const Bool viewontag           = True;     /* Switch view on tag switch */
static const char *fonts[]            = {
	"Operator Mono SSm Book:pixelsize=23",
	"JoyPixels:pixelsize=23",
	"Noto Color Emoji:pixelsize=23",
	"OPPOSans:pixelsize=23",
};
static const char dmenufont[]         = "Operator Mono SSm Book:pixelsize=23";

static const char col_gray1[] = "#222222";
static const char col_gray2[] = "#444444";
static const char col_gray3[] = "#bbbbbb";
static const char col_gray4[] = "#eeeeee";
static const char col_cyan[]  = "#005577";
static const char col1[] = "#ffffff";
static const char col2[] = "#ffffff";
static const char col3[] = "#ffffff";
static const char col4[] = "#ffffff";
static const char col5[] = "#ffffff";
static const char col6[] = "#ffffff";

enum { SchemeNorm, SchemeCol1, SchemeCol2, SchemeCol3, SchemeCol4,
       SchemeCol5, SchemeCol6, SchemeSel, SchemeHid }; /* color schemes */

static const unsigned int baralpha    = 0xd0;
static const unsigned int borderalpha = OPAQUE;
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeCol1] = { col1,      col_gray1, col_gray2 },
	[SchemeCol2] = { col2,      col_gray1, col_gray2 },
	[SchemeCol3] = { col3,      col_gray1, col_gray2 },
	[SchemeCol4] = { col4,      col_gray1, col_gray2 },
	[SchemeCol5] = { col5,      col_gray1, col_gray2 },
	[SchemeCol6] = { col6,      col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4,  col_cyan, col_cyan  },
	[SchemeHid]  = { col_gray1, col_gray1, col_cyan  },
};
static const unsigned int alphas[][3]      = {
	/*               fg      bg        border     */
	[SchemeNorm] = { OPAQUE, baralpha, borderalpha },
	[SchemeSel]  = { OPAQUE, baralpha, borderalpha },
};

/* tagging */
static const char *tags[] = {"\uf120", "\uf7ae", "\uf04b", "\uf121", "\ue62e", "\u266C", "\ue727", "\uf537", "\uf684"};

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	//	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.5; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

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


/* key definitions */
#define MODKEY Mod1Mask
#define WINKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
{ MODKEY|ControlMask,           KEY,      toggleview,      {.ui = 1 << TAG} }, \
{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2]         = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[]   = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]    = { "st", NULL };
static const char *browsercmd[] = { "brave", NULL };
static const char *clipboard[]  = { "clipmenu", NULL };

/* Perpare to add screenshot function
 *
 * Wishlist
 * 1. Use Shortcut to take screenshot instead of printscreen, because my bluetooth-keyboard don't have this key
 * 2. Use xclip to save screenshot instead of save to disk
 * */

/* Volume control: amixer set Master percent(-/+)
 * Example: amixer set Master 10%-
 * */
static const char *volumeup[]     = { "amixer", "set", "Master", "5%+", NULL };
static const char *volumedown[]   = { "amixer", "set", "Master", "5%-", NULL };
static const char *volumemute[]   = { "amixer", "-q", "set", "Master", "toggle", NULL };
static const char *backlightinc[] = { "xbacklight", "-inc", "10", NULL};
static const char *backlightdec[] = { "xbacklight", "-dec", "10", NULL};
static const char *screentools[]  = { "screentools", NULL};
static const char *flameshot[]    = { "sh", "~/github/scripts/screenshot.sh", NULL};
static const char *lockscreen[]   = { "slock", NULL};
// static const char *shutdown[]     = { "prompt", "Are you sure you want shutdown?", "shutdown -h now", NULL };
static const char *reboot[]       = { "prompt", "Are you sure you want reboot?", "reboot", NULL };

static Key keys[] = {
	/* modifier                     key		   function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd     } },
	{ MODKEY|ShiftMask,             XK_g,      spawn,          {.v = browsercmd   } },
	{ MODKEY|ShiftMask,        XK_Return,      spawn,          {.v = termcmd      } },
	// { MODKEY,                       XK_d,      spawn,          {.v = shutdown     } },
	{ MODKEY|ShiftMask,             XK_d,      spawn,          {.v = reboot       } },
	{ MODKEY,                      XK_F1,      spawn,          {.v = volumemute   } }, // 静音
	{ MODKEY,                      XK_F2,      spawn,          {.v = volumedown   } }, // 音量减小
	{ MODKEY,                      XK_F3,      spawn,          {.v = volumeup     } }, // 音量增加
	{ MODKEY,                      XK_F5,      spawn,          {.v = backlightdec } }, // screen light
	{ MODKEY,                      XK_F6,      spawn,          {.v = backlightinc } }, // screen light
	{ WINKEY|ShiftMask,             XK_5,      spawn,          {.v = screentools  } }, // screentools
	{ MODKEY|ShiftMask,         XK_Print,      spawn,          {.v = flameshot    } }, // screenshot to clipboard
	{ MODKEY,               XK_backslash,      spawn,          {.v = lockscreen   } },
	{ MODKEY|ShiftMask,             XK_v,      spawn,          {.v = clipboard    } },
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
	{ MODKEY,                   XK_minus,      setgaps,        {.i = -1 } },
	{ MODKEY,                   XK_equal,      setgaps,        {.i = +1 } },
	{ MODKEY|ShiftMask,         XK_equal,      setgaps,        {.i = 0  } },
	{ MODKEY,                  XK_Return,      zoom,           {0} },
	{ MODKEY,                     XK_Tab,      view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
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
	{ ClkWinTitle,          0,              Button1,        toggleview,      {0} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button1,        sigdwmblocks,   {.i = 1} },
	{ ClkStatusText,        0,              Button1,        sigdwmblocks,   {.i = 2} },
	{ ClkStatusText,        0,              Button1,        sigdwmblocks,   {.i = 3} },
	{ ClkStatusText,        0,              Button1,        sigdwmblocks,   {.i = 4} },
	{ ClkStatusText,        0,              Button1,        sigdwmblocks,   {.i = 5} },
	{ ClkStatusText,        0,              Button1,        sigdwmblocks,   {.i = 6} },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
