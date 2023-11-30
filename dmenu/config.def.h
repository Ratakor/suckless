/* See LICENSE file for copyright and license details. */
/* Default settings; can be overriden by command line. */

static int topbar = 0;       /* -b  option; if 0, dmenu appears at bottom */
static int centered = 1;     /* -c option; centers dmenu on screen */
static int min_width = 450;  /* minimum width when centered */

/* -fn option overrides fonts[0]; default X11 font or font set */
static char font[] = "monospace:size=12";
static const char *fonts[] = {
	font,
	"monospace:size=12"
};
static const char *prompt    = NULL;  /* -p  option; prompt to the left of input field */
static char normfgcolor[]    = "#f8f8f2";
static char normbgcolor[]    = "#282a36";
static char selfgcolor[]     = "#f8f8f2";
static char selbgcolor[]     = "#6272a4";
static char outcolor[]       = "#bd93f9";
static char highlightcolor[] = "#ffb86c";
static char *colors[SchemeLast][2] = {
	/*                          fg              bg       */
	[SchemeNorm]            = { normfgcolor,    normbgcolor },
	[SchemeSel]             = { selfgcolor,     selbgcolor },
	[SchemeOut]             = { outcolor,       normbgcolor },
	[SchemeNormHighlight]   = { highlightcolor, normbgcolor },
	[SchemeSelHighlight]    = { highlightcolor, selbgcolor },
};
static const unsigned int alphas[SchemeLast][2] = {
	[SchemeNorm]            = { OPAQUE, 0xd9 },
	[SchemeSel]             = { OPAQUE, 0xd9 },
	[SchemeOut]             = { OPAQUE, 0xd9 },
	[SchemeNormHighlight]   = { OPAQUE, 0xd9 },
	[SchemeSelHighlight]    = { OPAQUE, 0xd9 },
};

/* -l option; if nonzero, dmenu uses vertical list with given number of lines */
static unsigned int lines      = 12;

/*
 * Characters not considered part of a word while deleting words
 * for example: " /?\"&[]"
 */
static const char worddelimiters[] = " ";

/* Size of the window border */
static unsigned int border_width = 1;

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
	{ "font",        STRING, &font },
	{ "normfgcolor", STRING, &normfgcolor },
	{ "normbgcolor", STRING, &normbgcolor },
	{ "selfgcolor",  STRING, &selfgcolor },
	{ "selbgcolor",  STRING, &selbgcolor },
	{ "color4",      STRING, &outcolor },
	{ "highlight",   STRING, &highlightcolor },
};
