/* See LICENSE file for copyright and license details. */

/* appearance */
static unsigned int borderpx  = 3;        /* border pixel of windows */
static unsigned int snap      = 32;       /* snap pixel */
static int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static unsigned int gappih    = 20;       /* horiz inner gap between windows */
static unsigned int gappiv    = 10;       /* vert inner gap between windows */
static unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static unsigned int gappov    = 30;       /* vert outer gap between windows and screen edge */
static unsigned int dpi       = 96;       /* monitor display dpi */
static int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static int showbar            = 1;        /* 0 means no bar */
static int topbar             = 1;        /* 0 means bottom bar */
static char font[]            = "monospace:size=10";
static const char *fonts[]    = {
	font,
	"JoyPixels:size=12",
	"Noto Color Emoji:size=12",
	"WenQuanYi Zen Hei Mono:size=12"
};
static char dmenufont[]       = "monospace:size=10";
static char normbgcolor[]     = "#222222";
static char normbordercolor[] = "#444444";
static char normfgcolor[]     = "#bbbbbb";
static char selfgcolor[]      = "#eeeeee";
static char selbordercolor[]  = "#005577";
static char selbgcolor[]      = "#005577";
static char col1[]            = "#ffffff";
static char col2[]            = "#ffffff";
static char col3[]            = "#ffffff";
static char col4[]            = "#ffffff";
static char col5[]            = "#ffffff";
static char col6[]            = "#ffffff";

enum { SchemeNorm, SchemeCol1, SchemeCol2, SchemeCol3, SchemeCol4,
       SchemeCol5, SchemeCol6, SchemeSel }; /* color schemes */
static char *colors[][3]      = {
	/*               fg           bg           border   */
	[SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
	[SchemeCol1] = { col1,        normbgcolor, normbordercolor },
	[SchemeCol2] = { col2,        normbgcolor, normbordercolor },
	[SchemeCol3] = { col3,        normbgcolor, normbordercolor },
	[SchemeCol4] = { col4,        normbgcolor, normbordercolor },
	[SchemeCol5] = { col5,        normbgcolor, normbordercolor },
	[SchemeCol6] = { col6,        normbgcolor, normbordercolor },
	[SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class     instance  title           tags mask  isfloating  isterminal  noswallow  monitor */
	{ "Gimp",      NULL,     NULL,           0,         1,          0,           0,        -1 },
	{ "librewolf", NULL,     NULL,           0,         0,          0,           1,        -1 },
	{ "St",        NULL,     NULL,           0,         0,          1,           0,        -1 },
	{ NULL,        NULL,     "Event Tester", 0,         0,          0,           1,        -1 }, /* xev */
};

/* layout(s) */
static float mfact     = 0.5;  /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include <X11/XF86keysym.h>
#include "vanitygaps.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "Tile",     tile },    /* first entry is default */
	{ "TTT",      bstack },  /* Master on top, slaves on bottom */

	{ "Mono",     monocle },
	{ "H[]",      deck },

	{ "[@]",      spiral },
	{ "[\\]",     dwindle },

	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },

	{ "><>",      NULL },    /* no layout function means floating behavior */

	{ "===",      bstackhoriz },
	{ "HHH",      grid },
	{ "###",      nrowgrid },
	{ "---",      horizgrid },
	{ ":::",      gaplessgrid },
	{ NULL,       NULL },
};

/* key definitions */
// super key as main key
#define MODKEY Mod4Mask
#define ALTKEY Mod1Mask

#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },
#define STACKKEYS(MOD,ACTION) \
	{ MOD, XK_j,     ACTION##stack, {.i = INC(+1) } }, \
	{ MOD, XK_k,     ACTION##stack, {.i = INC(-1) } }, \
	{ MOD, XK_grave, ACTION##stack, {.i = PREVSEL } }, \
	{ MOD, XK_q,     ACTION##stack, {.i = 0 } }, \
	{ MOD, XK_a,     ACTION##stack, {.i = 1 } }, \
	{ MOD, XK_z,     ACTION##stack, {.i = 2 } }, \
	{ MOD, XK_x,     ACTION##stack, {.i = -1 } },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }
/* default programs */
#define TERMINAL "st"
#define VIDEO "mpv"
#define SCREENLOCK "slock"
#define MUSIC "ncmpcpp"
#define BROWSER "microsoft-edge-stable"
/* commands */
static char dmenumon[2]             = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[]       = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbordercolor, "-sf", selfgcolor, NULL };
static const char *termcmd[]        = { "st", NULL };
static const char *browser[]        = { "env", "GTK_IM_MODULE=xim", "microsoft-edge-stable", NULL };
static const char *volumeup[]       = { "sound", "4", NULL };
static const char *volumedown[]     = { "sound", "6", NULL };
static const char *volumemute[]     = { "sound", "2", NULL };
static const char *songnext[]       = { "mpc", "next", NULL };
static const char *songprev[]       = { "mpc", "prev", NULL };
static const char *songtoggle[]     = { "mpc", "toggle", NULL };
static const char *micmute[]        = { "pamixer", "--default-source", "-t", NULL };
static const char *backlightinc[]   = { "xbacklight", "-inc", "10", NULL};
static const char *backlightdec[]   = { "xbacklight", "-dec", "10", NULL};
static const char *screentools[]    = { "screentools", NULL};
static const char *screenshot[]     = { "screenshot", NULL };
static const char *webcam[]         = { VIDEO, "/dev/video0", NULL};
static const char *touchpadtoggle[] = { "touchpadtoggle", NULL};
static const char *lockscreen[]     = { SCREENLOCK, NULL};
static const char *filebrowser[]    = { TERMINAL, "-e", "lfub", NULL};
static const char *taskmanager[]    = { TERMINAL, "-e", "btop", NULL};
static const char *music[]          = { TERMINAL, "-e", MUSIC, NULL};
static const char *email[]          = { TERMINAL, "-e", "neomutt", NULL};
static const char *audio[]          = { TERMINAL, "-e", "pulsemixer", NULL};
static const char *calendar[]       = { TERMINAL, "-e", "calcurse", NULL};
static const char *newsboat[]       = { TERMINAL, "-e", "newsboat", NULL};
// static const char *shutdownOr[]     = { "prompts", "shutdown or reboot?", "shutdown", "reboot", "no", "sudo poweroff", "sudo reboot", "echo no", NULL};
static const char *mountOption[]    = { "prompts", "mount local or cifs?", "local", "cifs", "no", "mounter", "dmenumountcifs", "echo no", NULL};
static const char *umountOption[]   = { "prompts", "unmount local or cifs?", "local", "cifs", "no", "unmounter", "dmenuumountcifs", "echo no", NULL};

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
		{ "font",        STRING,  &font },
		{ "font",        STRING,  &dmenufont },
		{ "color0",      STRING,  &normbgcolor },
		{ "color0",      STRING,  &normbordercolor },
		{ "color4",      STRING,  &normfgcolor },
		{ "color4",      STRING,  &selbgcolor },
		{ "color8",      STRING,  &selbordercolor },
		{ "color0",      STRING,  &selfgcolor },
		{ "dpi",         INTEGER, &dpi },
		{ "showbar",     INTEGER, &showbar },
		{ "topbar",      INTEGER, &topbar },
		{ "nmaster",     INTEGER, &nmaster },
		{ "resizehints", INTEGER, &resizehints },
		{ "mfact",       FLOAT,   &mfact },
};

/* dpi Xresources */
/**
 * TODO: add font dpi
 */
ResourcePref dpi_resources[] = {
		{ "borderpx",    INTEGER, &borderpx },
		{ "snap",        INTEGER, &snap },
		{ "gappih",      INTEGER, &gappih },
		{ "gappiv",      INTEGER, &gappiv },
		{ "gappoh",      INTEGER, &gappoh },
		{ "gappov",      INTEGER, &gappov },
};

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd     } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd      } },
	{ MODKEY|ShiftMask,             XK_g,      spawn,          {.v = browser      } },
	{ MODKEY|ShiftMask,             XK_p,      spawn,           SHCMD("sysact")     },
	{ 0,                XF86XK_AudioMute,      spawn,          {.v = volumemute   } },
	{ 0,             XF86XK_AudioMicMute,      spawn,          {.v = micmute      } },
	{ 0,         XF86XK_AudioLowerVolume,      spawn,          {.v = volumedown   } },
	{ 0,         XF86XK_AudioRaiseVolume,      spawn,          {.v = volumeup     } },
	{ 0,                XF86XK_AudioNext,      spawn,          {.v = songnext     } },
	{ 0,                XF86XK_AudioPlay,      spawn,          {.v = songtoggle   } },
	{ 0,                XF86XK_AudioPrev,      spawn,          {.v = songprev     } },
	{ 0,        XF86XK_MonBrightnessDown,      spawn,          {.v = backlightdec } },
	{ 0,          XF86XK_MonBrightnessUp,      spawn,          {.v = backlightinc } },
	{ 0,              XF86XK_ScreenSaver,      spawn,          {.v = lockscreen   } },
	{ 0,                   XF86XK_WebCam,      spawn,          {.v = webcam       } },
	{ 0,           XF86XK_TouchpadToggle,      spawn,          {.v = touchpadtoggle } }, 
	{ 0,                        XK_Print,      spawn,          {.v = screenshot   } },
	{ ALTKEY|ShiftMask,             XK_5,      spawn,          {.v = screentools  } },
	{ MODKEY,               XK_backslash,      spawn,          {.v = lockscreen   } },
	{ MODKEY,                       XK_e,      spawn,          {.v = filebrowser  } },
	{ MODKEY|ShiftMask,             XK_e,      spawn,          {.v = taskmanager  } },
	{ MODKEY,                       XK_m,      spawn,          {.v = music        } },
	{ MODKEY|ShiftMask,             XK_m,      spawn,          {.v = email        } },
	{ MODKEY,                       XK_c,      spawn,          {.v = calendar     } },
	{ MODKEY|ShiftMask,             XK_v,      spawn,          {.v = audio        } },
	{ MODKEY,                       XK_n,      spawn,          {.v = newsboat     } },
	{ ALTKEY|ShiftMask,             XK_v,      spawn,           SHCMD("clipmenu")   },
	{ ALTKEY|ShiftMask,             XK_p,      spawn,           SHCMD("passmenu")   },
	{ ALTKEY|ShiftMask,             XK_m,      spawn,          {.v = mountOption  } },
	{ ALTKEY|ShiftMask,             XK_n,      spawn,          {.v = umountOption } },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} }, /* Tile */
	{ MODKEY|ShiftMask,             XK_t,      setlayout,      {.v = &layouts[1]} }, /* bstack */
	{ MODKEY,                       XK_y,      setlayout,      {.v = &layouts[2]} }, /* monocle */
	{ MODKEY|ShiftMask,             XK_y,      setlayout,      {.v = &layouts[3]} }, /* deck */
	{ MODKEY,                       XK_o,      setlayout,      {.v = &layouts[4]} }, /* spiral */
	{ MODKEY|ShiftMask,             XK_o,      setlayout,      {.v = &layouts[5]} }, /* dwindle */
	{ MODKEY,                       XK_u,      setlayout,      {.v = &layouts[6]} }, /* centeredmaster */
	{ MODKEY|ShiftMask,             XK_u,      setlayout,      {.v = &layouts[7]} }, /* centeredfloatingmaster */
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[8]} }, /* float */
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	STACKKEYS(MODKEY,                          focus)
	STACKKEYS(MODKEY|ShiftMask,                push)
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY|Mod4Mask,              XK_0,      togglegaps,     {0} },
	{ MODKEY|Mod4Mask|ShiftMask,    XK_0,      defaultgaps,    {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_w,      killclient,     {0} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY|ShiftMask,             XK_f,      togglefullscr,  {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_r,      xrdb,           {.v = NULL } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_w,      quit,           {0} },
	// unuse
	// { MODKEY|Mod4Mask,              XK_u,      incrgaps,       {.i = +1 } },
	// { MODKEY|Mod4Mask|ShiftMask,    XK_u,      incrgaps,       {.i = -1 } },
	// { MODKEY|Mod4Mask,              XK_i,      incrigaps,      {.i = +1 } },
	// { MODKEY|Mod4Mask|ShiftMask,    XK_i,      incrigaps,      {.i = -1 } },
	// { MODKEY|Mod4Mask,              XK_o,      incrogaps,      {.i = +1 } },
	// { MODKEY|Mod4Mask|ShiftMask,    XK_o,      incrogaps,      {.i = -1 } },
	// { MODKEY|Mod4Mask,              XK_6,      incrihgaps,     {.i = +1 } },
	// { MODKEY|Mod4Mask|ShiftMask,    XK_6,      incrihgaps,     {.i = -1 } },
	// { MODKEY|Mod4Mask,              XK_7,      incrivgaps,     {.i = +1 } },
	// { MODKEY|Mod4Mask|ShiftMask,    XK_7,      incrivgaps,     {.i = -1 } },
	// { MODKEY|Mod4Mask,              XK_8,      incrohgaps,     {.i = +1 } },
	// { MODKEY|Mod4Mask|ShiftMask,    XK_8,      incrohgaps,     {.i = -1 } },
	// { MODKEY|Mod4Mask,              XK_9,      incrovgaps,     {.i = +1 } },
	// { MODKEY|Mod4Mask|ShiftMask,    XK_9,      incrovgaps,     {.i = -1 } },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,   0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,   0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,   0,              Button2,        zoom,           {0} },
	{ ClkWinTitle,   MODKEY,         Button1,        incrgaps,       {.i = +3} },
	{ ClkWinTitle,   MODKEY,         Button2,        defaultgaps,    { 0 } },
	{ ClkWinTitle,   MODKEY,         Button3,        incrgaps,       {.i = -3} },
	{ ClkStatusText, 0,              Button1,        sigdwmblocks,   {.i = 1} },
	{ ClkStatusText, 0,              Button2,        sigdwmblocks,   {.i = 2} },
	{ ClkStatusText, 0,              Button3,        sigdwmblocks,   {.i = 3} },
	{ ClkStatusText, ControlMask,    Button1,        sigdwmblocks,   {.i = 4} },
	{ ClkStatusText, ControlMask,    Button2,        sigdwmblocks,   {.i = 5} },
	{ ClkStatusText, ControlMask,    Button3,        sigdwmblocks,   {.i = 6} },
	{ ClkClientWin,  MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,  MODKEY|ShiftMask,Button1,       resizemouse,    {0} },
	{ ClkClientWin,  MODKEY,         Button2,        togglefloating, {0} },
	{ ClkTagBar,     0,              Button1,        view,           {0} },
	{ ClkTagBar,     0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,     MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,     MODKEY,         Button3,        toggletag,      {0} },
};

