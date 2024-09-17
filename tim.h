/* about **********************************************************************/

// tim.h is a portable library to create simple terminal applications
// Demo video: https://asciinema.org/a/zn3p0dsVCOQOzwY1S9gDfyaxQ

/* quick start ****************************************************************/

// #include "tim.h"                                // one header, no lib
// int main(void) {                                //
//     while (tim_run(0)) {                        // event loop
//         scope (A, A, 24, 8) {                   // centered 24x8 scope
//             uint64_t c = 0x0a060f;              // three colors
//             frame(0, 0, ~0, ~0, c);             // draw frame for scope
//             label("Greetings!", A, 2, A, A, c); // label in top center
//             if (button("OK", A, ~1, 8, A, c))   // button in bottom center
//                 return 0;                       // exit on button click
//             if (is_key_press('q'))              // ctrl-c is masked
//                 return 0;                       // exit on 'q' press
//         }                                       //
//     }                                           // atexit cleanup
// }                                               //

/* layout *********************************************************************/

// The terminal's columns (x) and rows (y) are addressed by their coordinates,
// the origin is in the top left corner.
//
// Scopes are the primary layout mechanism. They are used to group and place
// multiple elements. Scopes can be nested.
//
// The root scope is the full terminal screen. The scope macro is constructed
// with a for loop, so statements like break or return inside the scope block
// will probably give you a bad time.
//
// Elements (widget, control, component) are elements of user interaction, such
// as a button or edit box. Most elements take x/y/w/h arguments to control
// placement. All positions are given in relation the element's parent scope.
//
// Automatic (A) width and height are either based on the element's content, or
// take the full available space from parent.
//
//  arg | value | placement
// -----|-------|---------------------------------
//   x  |  n    | n columns to left
//   x  | ~n    | n columns to right
//   x  |  A    | center horizontally
//   y  |  n    | n rows to top
//   y  | ~n    | n rows to bottom
//   y  |  A    | center vertically
//   w  |  n    | n columns wide
//   w  | ~n    | fit width to n columns to right
//   w  |  A    | automatic width
//   h  |  n    | n rows high
//   h  | ~n    | fit height n rows to bottom
//   h  |  A    | automatic height
//
// The layout automatically adopts to terminal window resize events.

/* colors *********************************************************************/

// Most elements have a uint64 color argument which holds up to eight colors.
// Typically byte 0 is the text color and byte 1 is the background color.
//
// For example 0x08040f encodes three colors. When used with a button the text
// is white (0f), the background is blue (04), and the frame is gray (08).
//
// The terminal should support xterm-256 colors. The TERM variable is ignored.
// The lower 16 colors vary across different terminals, so the upper 240 colors
// should be used if consistency is important.
//
// xterm-256 color chart
// https://upload.wikimedia.org/wikipedia/commons/1/15/Xterm_256color_chart.svg

/* events *********************************************************************/

// tim_run blocks until it observes an event. Mouse and key events are always
// immediately followed by a draw event in order to make changes visible.
//
// Some elements need to consume events, for example edit consumes the key
// event when focused in order to prevent other key handlers on acting on them.
//
// The current event is stored in tim.event.
//
//  event       | cause
// -------------|-----------------------
//  DRAW_EVENT  | input, timeout, resize
//  KEY_EVENT   | key press
//  MOUSE_EVENT | mouse click
//  VOID_EVENT  | consumed event

/* elements *******************************************************************/

// frame (x, y, w, h, color)
//
//     Draw an empty frame and fill area.
//
//     x/y/w/h see layout documentation
//     color   background, frame
//
// label (str, x, y, w, h, color)
//
//     Draw text label. Automatic width and height are supported. Strings
//     exceeding width or height are clipped.
//
//     str     zero terminated string
//     x/y/w/h see layout documentation
//     color   background, text
//
// button (str, x, y, w, h, color) -> bool
//
//     Draw button. Automatic width and height are supported. Strings exceeding
//     width or height are clipped. Returns true when clicked.
//
//     str     zero terminated string
//     x/y/w/h see layout documentation
//     color   frame, background, text
//
// edit (state, x, y, w, color) -> bool
//
//     Draw text edit. Output is stored in state.str. Receives input events when
//     focused by mouse click. Escape or return relinquish focus. Returns true
//     when return is pressed.
//
//     state   pointer to persistent edit state struct
//     x/y/w   see layout documentation
//     color   f   rame, background, text
//
// check (str, state, x, y, w, color) -> bool
//
//     Draw check box. State determines how the box is checked. [x] when state
//     is non-zero, [ ] when state is zero, [-] when state is -1. A mouse click
//     toggles the state between one and zero and returns true.
//
//     str     zero terminated string
//     state   pointer to persistent state variable
//     x/y/w   see layout documentation
//     color   check, background, text
//
// radio (str, state, v, x, y, w, color) -> bool
//
//     Draw radio box. If state equals v, the box is selected. Radios are
//     grouped through a shared state. Within that group, each v must be unique.
//     A mouse click assigns v to state and returns true.
//
//     str     zero terminated string
//     state   pointer to persistent state variable
//     v       unique state value
//     x/y/w   see layout documentation
//     color   radio, background, text

/* functions ******************************************************************/

// tim_run (fps) -> bool
//
//     Process events and render frame. Blocks until input is received or the
//     next frame is due. First call also initializes the terminal. When fps is
//     zero the function blocks until input is received. Key and mouse events
//     are immediately followed by a draw event, so the actual fps can be
//     significantly greater than requested. Always returns true. To reset the
//     terminal after a crash, run "reset".
//     The Ctrl-C interrupt is masked, so make sure to put an exit condition
//     like this at the end of the main loop:
//
//         if (is_key_press(ESCAPE_KEY))
//             exit(0);
//
//     fps     frames per second
//
// is_key_press (key) -> bool
//
//     Returns true if key was pressed.
//
//     key     char literal or one of the KEY constants, see constants
//
// time_us () -> int64
//
//     Returns monotonic clock value in microseconds. Not affected by summer
//     time or leap seconds.

/* useful links ***************************************************************/

// https://invisible-island.net/xterm/ctlseqs/ctlseqs.html
// https://learn.microsoft.com/en-us/windows/console/

/* bugs ***********************************************************************/

// - Double buffering is still new, set ENABLE_DBUF to 0 if you see glitches
// - Double width characters like 彁 are not fully supported. Terminals do not
//   handle these consistently and there is no portable way to reliably
//   determine character width. The renderer can deal with some of the problems
//   caused by this, but results may vary.
// - Decomposed (NFD) UTF-8 is not supported and will cause havoc
// - Zero width code points are not supported
// - Windows cmd.exe resize events may be delayed

/* license ********************************************************************/

// MIT License
//
// Copyright (c) MMXXIV Chu'vok <chuvok@maeppi.e4ward.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// The software is provided "as is", without warranty of any kind, express or
// implied, including but not limited to the warranties of merchantability,
// fitness for a particular purpose and noninfringement. In no event shall the
// authors or copyright holders be liable for any claim, damages or other
// liability, whether in an action of contract, tort or otherwise, arising from,
// out of or in connection with the software or the use or other dealings in the
// software.

/* includes *******************************************************************/

// unix-like
#if defined __unix__ || defined __unix || defined __APPLE__ || defined __ELF__
#define TIM_UNIX
#include <poll.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#endif

// windows
#ifdef _WIN32
#define TIM_WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <consoleapi.h>
#include <io.h>
#endif

// libc
#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* workaround *****************************************************************/

#ifdef __cplusplus
#error "C++ is not supported. Sorry."
#endif

#ifdef _MSC_VER
// disable integer conversion warnings
#pragma warning(disable:4244)
#endif

#ifdef _WIN32
// fix windows.h name clash, coincidentally they have the same values
#undef KEY_EVENT   // 0x0001
#undef MOUSE_EVENT // 0x0002
#endif

#ifdef __PCC__
// Guard to identify dynamic shared objects during global destruction. Not sure
// if this is a good idea. pcc and tcc may require this.
int __dso_handle;
#endif

/* constants ******************************************************************/

#define ENABLE_DBUF 1               // double buffering
#define MAX_SCOPE   20              // max scope nesting
#define MAX_CELLS   0x20000         // size of screen buffer
#define MAX_BUF     (MAX_CELLS * 4) // size of output buffer
#define A           INT_MAX         // auto center / width / height

// tim.event.type
enum {
    DRAW_EVENT,  // draw screen
    KEY_EVENT,   // a key was pressed
    MOUSE_EVENT, // mouse button, scroll or move
    VOID_EVENT,  // set when an event was consumed
};

// tim.event.key
enum {
    LEFT_BUTTON   = 1,
    BACKSPACE_KEY = 8,
    TAB_KEY       = 9,
    ENTER_KEY     = 13,
    ESCAPE_KEY    = 27,
    INSERT_KEY    = -1,
    DELETE_KEY    = -2,
    HOME_KEY      = -3,
    END_KEY       = -4,
    PAGEUP_KEY    = -5,
    PAGEDOWN_KEY  = -6,
    UP_KEY        = -7,
    DOWN_KEY      = -8,
    LEFT_KEY      = -9,
    RIGHT_KEY     = -10,
};

/* types **********************************************************************/

struct cell {
    uint8_t fg;     // foreground color
    uint8_t bg;     // background color
    uint8_t wide;   // wide or following wide character
    uint8_t n;      // number of bytes in buf
    uint8_t buf[4]; // utf8 code point
};

struct rect {
    int x; // x coordinate (left = 0)
    int y; // y coordinate (top = 0)
    int w; // width
    int h; // height
};

struct text {
    int size;  // size in bytes without terminator
    int width; // widest line
    int lines; // number of lines
};

struct line {
    const char* str;   // input and parse state
    const char* line;  // line strings, not terminated
    int         size;  // line size in bytes
    int         width; // line width in glyph
};

struct event {
    int     type;    // event type
    int32_t key;     // used by KEY_EVENT and MOUSE_EVENT
    int     x;       // used by MOUSE_EVENT
    int     y;       // used by MOUSE_EVENT
    char    str[32]; // string representation of key
};

struct edit {
    int  cursor;   // cursor position (utf8)
    int  length;   // string length (utf8)
    char str[256]; // zero terminated buffer
};

struct state {
    int          w;                 // screen width
    int          h;                 // screen height
    int          frame;             // frame counter
    struct event event;             // current event
    uintptr_t    focus;             // focused element
    int          loop_stage;        // loop stage
    bool         resized;           // screen was resized
    int          scope;             // current scope
    struct rect  scopes[MAX_SCOPE]; // scope stack
    struct cell* cells;             // screen buffer
    char*        buf;               // final output buffer
    int          buf_size;          // position in write buffer
    int64_t      start_us;          // render start time
    int          render_us;         // elapsed render time
#ifdef TIM_UNIX                     //
    struct termios attr;            // initial attributes
    int            signal_pipe[2];  // signal fifo pipe
#endif                              //
#ifdef TIM_WINDOWS                  //
    SMALL_RECT window;              // screen buffer window size
    DWORD      mode_in;             // initial input mode
    DWORD      mode_out;            // initial output mode
    UINT       cp_in;               // initial input code page
    UINT       cp_out;              // initial output code page
#endif
};

/* macros *********************************************************************/

#define MAX(a, b)     ((a) > (b) ? (a) : (b))      //
#define MIN(a, b)     ((a) < (b) ? (a) : (b))      //
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0])) // number of items in array
#define S(s)          (""s), (sizeof(s) - 1)       // expand to s, sizeof(s) - 1

/* global variables ***********************************************************/

// These buffers were part of tim struct but caused the linker to produce very
// large binaries.
static struct cell tim_cells[MAX_CELLS << ENABLE_DBUF]; // screen buffer
static char        tim_buf[MAX_BUF];                    // output buffer

// global state
#ifdef TIM_EXTERN_STATE
extern struct state tim;
#else
// Intentionally not declared as static to trigger a linker error when used in
// multiple compilation units. If that happens, #define TIM_EXTERN_STATE before
// including this header in all but one compilation unit.
struct state tim = {
    .cells = tim_cells,
    .buf   = tim_buf,
};
#endif

/* string *********************************************************************/

// like strlen, returns 0 on NULL or int overflow
static inline int ztrlen(const char* s) {
    size_t n = s ? strlen(s) : 0;
    return MAX((int)n, 0);
}

// bit scan reverse, count leading zeros
static inline int bsr8(uint8_t x) {
#if defined __GNUC__ || defined __clang__
    unsigned int b = x;
    b <<= sizeof(b) * CHAR_BIT - 8;
    b  |= 1 << (sizeof(b) * CHAR_BIT - 9);
    return __builtin_clz(b);
#elif defined _MSC_VER
    unsigned long n = 0;
    unsigned long b = x;
    b <<= sizeof(b) * CHAR_BIT - 8;
    b  |= 1 << (sizeof(b) * CHAR_BIT - 9);
    _BitScanReverse(&n, b);
    return n;
#else
    int n = 0;
    for (; n < 8 && !(x & 128); n++, x <<= 1) {}
    return n;
#endif
}

// decode one utf8 code point
static int32_t utfchr(const char* s) {
    s = s ? s : "";
    // use bit magic to mask out leading utf8 1s
    uint32_t c = s[0] & ((1 << (8 - bsr8(~s[0]))) - 1);
    for (int i = 1; s[0] && s[i] && i < 4; i++) {
        c = (c << 6) | (s[i] & 63);
    }
    return (int32_t)c;
}

// number of utf8 code points
static int utflen(const char* s) {
    int n = 0;
    for (int i = 0; s && s[i]; i++) {
        n += (s[i] & 192) != 128;
    }
    return n;
}

// index of utf8 code point at pos
static int utfpos(const char* s, int pos) {
    int i = 0;
    for (int n = 0; pos >= 0 && s && s[i]; i++) {
        n += (s[i] & 192) != 128;
        if (n == pos + 1) {
            return i;
        }
    }
    return i;
}

// scan string for width and lines
static struct text scan_str(const char* str) {
    const char* s = str ? str : "";
    struct text t = {
        .width = 0,
        .lines = (s[0] != 0),
    };
    int width = 0;
    for (t.size = 0; s[t.size]; t.size++) {
        char ch      = s[t.size];
        int  newline = (ch == '\n');
        width = newline ? 0 : width;
        width += (ch & 192) != 128 && (uint8_t)ch > 31;
        t.lines += newline;
        t.width = MAX(t.width, width);
    }
    return t;
}

// iterate through lines, false when end is reached
static bool next_line(struct line* l) {
    if (!l->str || !l->str[0]) {
        return false;
    }
    l->line  = l->str;
    l->size  = 0;
    l->width = 0;
    for (const char* s = l->str; s[0] && s[0] != '\n'; s++) {
        l->size  += 1;
        l->width += (s[0] & 192) != 128 && (uint8_t)s[0] > 31;
    }
    l->str += l->size + !!l->str[l->size];
    return true;
}

// true if utf8 code point could be wide
static bool is_wide_perhaps(const uint8_t* s, int n) {
    // Character width depends on character, terminal and font. There is no
    // reliable method, however most frequently used characters are narrow.
    // Zero with characters are ignored, and hope that user input is benign.
    if (n < 3 || s[0] < 225) {
        // u+0000 - u+1000, basic latin - tibetan
        return false;
    } else if (s[0] == 226 && s[1] >= 148 && s[1] < 152) {
        // u+2500 - u+2600 box drawing, block elements, geometric shapes
        return false;
    }
    return true;
}

/* unix ***********************************************************************/

// Unix-like terminal IO. Osx is missing ppoll and __unix__. Come on, fix it!

#ifdef TIM_UNIX

static void write_str(const char* s, int size) {
    ssize_t _ = write(STDOUT_FILENO, s, size);
    (void)_; // remove unused-result warning
}

static void signal_handler(int signal) {
    // signals are written into a fifo pipe and read by event loop
    ssize_t _ = write(tim.signal_pipe[1], &signal, sizeof(signal));
    (void)_; // remove unused-result warning
}

static void update_screen_size(void) {
    struct winsize ws = {0};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != 0) {
        return;
    }
    int w = ws.ws_col;
    int h = ws.ws_row;
    tim.resized = (unsigned)(w * h) <= MAX_CELLS && (w != tim.w || h != tim.h);
    if (tim.resized) {
        tim.w = tim.scopes[0].w = w;
        tim.h = tim.scopes[0].h = h;
    }
}

static void init_terminal(void) {
    tcgetattr(STDOUT_FILENO, &tim.attr);        // store attributes
    struct termios attr = tim.attr;             //
    cfmakeraw(&attr);                           // configure raw mode
    tcsetattr(STDOUT_FILENO, TCSADRAIN, &attr); // set new attributes
    write_str(S("\33[?2004l"));                 // reset bracketed paste mode
    write_str(S("\33[?1049h"));                 // use alternative buffer
    write_str(S("\33[?25l"));                   // hide cursor
    write_str(S("\33[?1000h"));                 // enable mouse
    write_str(S("\33[?1002h"));                 // enable button events
    write_str(S("\33[?1006h"));                 // use mouse sgr protocol
    update_screen_size();                       // get terminal size
    int err = pipe(tim.signal_pipe);            // create signal pipe
    if (!err) {                                 //
        signal(SIGWINCH, signal_handler);       // terminal size changed
    }
}

static void reset_terminal(void) {
    tcsetattr(STDOUT_FILENO, TCSADRAIN, &tim.attr); // restore attributes
    write_str(S("\33[?1000l"));                     // disable mouse
    write_str(S("\33[?1002l"));                     // disable mouse
    write_str(S("\33[m"));                          // reset colors
    write_str(S("\33[?25h"));                       // show cursor
    write_str(S("\33[?1049l"));                     // exit alternative buffer
}

// parse input stored in e->str
static bool parse_input(struct event* restrict e, int n) {
    char* s = e->str;

    if (n == 1 || s[0] != 27) {
        // regular key press
        e->type = KEY_EVENT;
        e->key  = s[0] == 127 ? BACKSPACE_KEY : utfchr(s);
        return true;
    }

    if (n >= 9 && !memcmp(s, S("\33[<"))) {
        // sgr mouse sequence
        e->type = MOUSE_EVENT;
        int btn = strtol(s + 3, &s, 10);
        e->x    = strtol(s + 1, &s, 10) - 1;
        e->y    = strtol(s + 1, &s, 10) - 1;
        if (btn == 0 && s[0] == 'M') {
            // left button pressed
            e->key = LEFT_BUTTON;
            return true;
        } 
        return false;
    }

    static struct {char s[4]; int k;} key_table[] = {
        {"[A" , UP_KEY},
        {"[B" , DOWN_KEY},
        {"[C" , RIGHT_KEY},
        {"[D" , LEFT_KEY},
        {"[2~", INSERT_KEY},
        {"[4h", INSERT_KEY},
        {"[3~", DELETE_KEY},
        {"[P" , DELETE_KEY},
        {"[H" , HOME_KEY},
        {"[F" , END_KEY},
        {"[4~", END_KEY},
        {"[5~", PAGEUP_KEY},
        {"[6~", PAGEDOWN_KEY},
    };

    if ((n == 3 || n == 4) && s[0] == 27) {
        // key sequence
        for (int i = 0; i < ARRAY_SIZE(key_table); i++) {
            if (!memcmp(s + 1, key_table[i].s, n - 1)) {
                e->type = KEY_EVENT;
                e->key  = key_table[i].k;
                return true;
            }
        }
    }

    return false;
}

static void read_event(int timeout_ms) {
    struct event* e = &tim.event;

    struct pollfd pfd[2] = {
        {.fd = tim.signal_pipe[0], .events = POLLIN},
        {.fd = STDIN_FILENO,       .events = POLLIN},
    };

    while (true) {
        memset(e, 0, sizeof(*e));

        int r = poll(pfd, 2, timeout_ms > 0 ? timeout_ms : -1);
        if (r < 0) {
            // poll error, EINTR or EAGAIN
            continue;
        } else if (r == 0) {
            // poll timeout
            e->type = DRAW_EVENT;
            return;
        }

        if (pfd[0].revents & POLLIN) {
            // received signal
            int sig = 0;
            int n   = read(tim.signal_pipe[0], &sig, sizeof(sig));
            if (n > 0 && sig == SIGWINCH) {
                // screen size changed
                e->type = DRAW_EVENT;
                update_screen_size();
                return;
            }
        }

        if (pfd[1].revents & POLLIN) {
            // received input
            int n = read(STDIN_FILENO, e->str, sizeof(e->str) - 1);
            if (parse_input(e, n)) {
                return;
            }
        }
    } // while
}

static inline int64_t time_us(void) {
    struct timespec ts = {};
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

#endif // TIM_UNIX

/* windows ********************************************************************/

// Windows terminal IO. Win32 is actually not that horrible as many say. Quirky
// but well documented.

#ifdef TIM_WINDOWS

static void write_str(const char* s, int size) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteFile(h, s, size, NULL, NULL);
    FlushFileBuffers(h);
}

static void update_screen_size(void) {
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi = {0};
    if (GetConsoleScreenBufferInfo(hout, &csbi) == 0) {
        return;
    }
    int w = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int h = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    tim.resized = (unsigned)(w * h) <= MAX_CELLS && (w != tim.w || h != tim.h);
    if (tim.resized) {
        tim.w      = tim.scopes[0].w = w;
        tim.h      = tim.scopes[0].h = h;
        tim.window = csbi.srWindow;
    }
}

static void init_terminal(void) {
    DWORD  mode = 0;
    HANDLE hin  = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(hin, &tim.mode_in);             // get current input mode
    mode = tim.mode_in;                            //
    mode &= ~ENABLE_ECHO_INPUT;                    // disable echo
    mode &= ~ENABLE_LINE_INPUT;                    // disable line buffer
    mode &= ~ENABLE_PROCESSED_INPUT;               // disable ctrl-c
    mode |= ENABLE_WINDOW_INPUT;                   // enable resize event
    mode |= ENABLE_MOUSE_INPUT;                    // enable mouse event
    mode |= ENABLE_EXTENDED_FLAGS;                 // for ENABLE_QUICK_EDIT
    mode &= ~ENABLE_QUICK_EDIT_MODE;               // disable select mode
    SetConsoleMode(hin, mode);                     // set input mode
                                                   //
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE); //
    GetConsoleMode(hout, &tim.mode_out);           // get current output mode
    mode = tim.mode_out;                           //
    mode |= ENABLE_PROCESSED_OUTPUT;               // enable ascii sequences
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;    // enable vt sequences
    SetConsoleMode(hout, mode);                    // set output mode
                                                   //
    tim.cp_in  = GetConsoleCP();                   // get current code page
    tim.cp_out = GetConsoleOutputCP();             //
    SetConsoleCP(CP_UTF8);                         // set utf8 in/out code page
    SetConsoleOutputCP(CP_UTF8);                   //
    write_str(S("\33[?25l"));                      // disable cursor
    update_screen_size();                          //
}

static void reset_terminal(void) {
    write_str(S("\33[m"));                         // reset colors
    write_str(S("\33[?25h"));                      // show cursor
    HANDLE hin  = GetStdHandle(STD_INPUT_HANDLE);  //
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE); //
    SetConsoleMode(hin, tim.mode_in);              // set original mode
    SetConsoleMode(hout, tim.mode_out);            //
    SetConsoleCP(tim.cp_in);                       // set original code page
    SetConsoleOutputCP(tim.cp_out);                //
}

static void read_event(int timeout_ms) {
    struct event* e = &tim.event;
    HANDLE        h = GetStdHandle(STD_INPUT_HANDLE);

    static const int8_t key_table[256] = {
        [VK_BACK]   = BACKSPACE_KEY,
        [VK_TAB]    = TAB_KEY,
        [VK_RETURN] = ENTER_KEY,
        [VK_ESCAPE] = ESCAPE_KEY,
        [VK_PRIOR]  = PAGEUP_KEY,
        [VK_NEXT]   = PAGEDOWN_KEY,
        [VK_END]    = END_KEY,
        [VK_HOME]   = HOME_KEY,
        [VK_LEFT]   = LEFT_KEY,
        [VK_UP]     = UP_KEY,
        [VK_RIGHT]  = RIGHT_KEY,
        [VK_DOWN]   = DOWN_KEY,
        [VK_INSERT] = INSERT_KEY,
        [VK_DELETE] = DELETE_KEY,
    };

    while (true) {
        memset(e, 0, sizeof(*e));
        DWORD r = WaitForSingleObject(h, timeout_ms);

        if (r == WAIT_TIMEOUT) {
            e->type = DRAW_EVENT;
            update_screen_size(); // workaround, see WINDOW_BUFFER_SIZE_EVENT
            return;
        } else if (r != WAIT_OBJECT_0) {
            continue;
        }

        // received input
        INPUT_RECORD rec = {0};
        DWORD        n   = 0;
        ReadConsoleInput(h, &rec, 1, &n);

        switch (rec.EventType) {
        case KEY_EVENT: {
            if (!rec.Event.KeyEvent.bKeyDown) {
                // only interested in key press
                continue;
            }
            int   key = key_table[(uint8_t)rec.Event.KeyEvent.wVirtualKeyCode];
            WCHAR chr = rec.Event.KeyEvent.uChar.UnicodeChar;
            if (!key && chr < ' ') {
                // non printable key
                continue;
            }
            update_screen_size(); // workaround, see WINDOW_BUFFER_SIZE_EVENT
            e->type = KEY_EVENT;
            if (key) {
                e->key = key;
                return;
            }
            e->key = chr;
            WideCharToMultiByte(CP_UTF8, 0, &chr, 1, e->str, sizeof(e->str),
                                NULL, NULL);
            return;
        }

        case MOUSE_EVENT: {
            bool move = rec.Event.MouseEvent.dwEventFlags & ~DOUBLE_CLICK;
            bool left = rec.Event.MouseEvent.dwButtonState &
                        FROM_LEFT_1ST_BUTTON_PRESSED;
            if (move || !left) {
                  // ignore move events and buttons other than left
                  continue;
            }
            update_screen_size(); // workaround, see WINDOW_BUFFER_SIZE_EVENT
            e->type = MOUSE_EVENT;
            e->key  = LEFT_BUTTON;
            e->x    = rec.Event.MouseEvent.dwMousePosition.X - tim.window.Left;
            e->y    = rec.Event.MouseEvent.dwMousePosition.Y - tim.window.Top;
            return;
        }

        case WINDOW_BUFFER_SIZE_EVENT:
            e->type = DRAW_EVENT;
            // cmd.exe screen buffer and window size are separate, making this
            // event a bit unreliable. Effectively it is only emitted when the
            // terminal width changes and not for the height. As a workaround
            // the screen size is updated every time an event is emitted.
            update_screen_size();
            // For cmd.exe the cursor has to be hidden after each resize event.
            write_str(S("\33[?25l"));
            return;
        }
    } // while
}

static inline int64_t time_us(void) {
    LARGE_INTEGER ticks = {0};
    LARGE_INTEGER freq  = {0};
    QueryPerformanceCounter(&ticks);
    QueryPerformanceFrequency(&freq);
    return 1000000 * ticks.QuadPart / freq.QuadPart;
}

#endif // TIM_WINDOWS

/* events *********************************************************************/

// returns true if event was of type and key
static inline bool is_event_key(int type, int32_t key) {
    return tim.event.type == type && tim.event.key == key;
}

// returns true if event was press of key
static bool inline is_key_press(int32_t key) {
    return is_event_key(KEY_EVENT, key);
}

// returns true if mouse event was over r
static inline bool is_mouse_over(struct rect r) {
    int x = tim.event.x;
    int y = tim.event.y;
    return x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h;
}

// returns true if event is mouse left-down and over r
static inline bool is_click_over(struct rect r) {
    return is_event_key(MOUSE_EVENT, LEFT_BUTTON) && is_mouse_over(r);
}

/* drawing ********************************************************************/

// create cell from utf8 code point with fg and bg colors
static inline struct cell cell(const char* s, uint8_t fg, uint8_t bg) {
    struct cell c = {.fg = fg, .bg  = bg, .n = 1, .buf = {s[0]}};
    while ((s[c.n] & 192) == 128 && c.n < sizeof(c.buf)) {
        c.buf[c.n] = s[c.n];
        c.n += 1;
    }
    return c;
}

// clear cell buffer
static void clear_cells(void) {
    size_t size = sizeof(tim.cells[0]) * tim.w * tim.h;
    memset(tim.cells, 0, size);
}

// draw cell at position
static void draw_chr(struct cell cell, int x, int y) {
    if (x >= 0 && x < tim.w && y >= 0 && y < tim.h) {
        tim.cells[x + y * tim.w] = cell;
    }
}

// draw row of cells
static void draw_row(struct cell cell, int x, int y, int w) {
    if (y >= 0 && y < tim.h && w > 0) {
        for (int i = MAX(x, 0); i < MIN(x + w, tim.w); i++) {
            tim.cells[i + y * tim.w] = cell;
        }
    }
}

// draw column of cells
static void draw_col(struct cell cell, int x, int y, int h) {
    if (x >= 0 && x < tim.w && h > 0) {
        for (int i = MAX(y, 0); i < MIN(y + h, tim.h); i++) {
            tim.cells[x + i * tim.w] = cell;
        }
    }
}

// fill lot (area) of cells
static void draw_lot(struct cell cell, int x, int y, int w, int h) {
    if (w > 0 && h > 0) {
        for (int iy = MAX(y, 0); iy < MIN(y + h, tim.h); iy++) {
            for (int ix = MAX(x, 0); ix < MIN(x + w, tim.w); ix++) {
                tim.cells[ix + iy * tim.w] = cell;
            }
        }
    }
}

// draw string to line, tags potential wide characters
static void draw_str(const char* s, int x, int y, int w,
                     uint8_t fg, uint8_t bg) {
    if (s && y >= 0 && x < tim.w && y < tim.h ) {
        int  end  = MIN(x + w, tim.w);
        bool wide = false;
        for (int i = 0; s[i] && x < end; x++) {
            struct cell c = cell(&s[i], fg, bg);
            wide = wide || is_wide_perhaps(c.buf, c.n);
            if (x >= 0) {
                c.wide = wide;
                tim.cells[x + y * tim.w] = c;
            }
            i += c.n;
        }
    }
}

// draw box of ascii cell characters
static void draw_box(int x, int y, int w, int h, uint8_t fg, uint8_t bg) {
    draw_chr(cell("┌", fg, bg), x        , y);
    draw_chr(cell("┐", fg, bg), x + w - 1, y);
    draw_chr(cell("└", fg, bg), x        , y + h - 1);
    draw_chr(cell("┘", fg, bg), x + w - 1, y + h - 1);
    draw_row(cell("─", fg, bg), x + 1    , y        , w - 2);
    draw_row(cell("─", fg, bg), x + 1    , y + h - 1, w - 2);
    draw_col(cell("│", fg, bg), x        , y + 1    , h - 2);
    draw_col(cell("│", fg, bg), x + w - 1, y + 1    , h - 2);
    draw_lot(cell(" ", fg, bg), x + 1    , y + 1    , w - 2, h - 2);
}

// invert fg and bg colors of line of cells
static void draw_invert(int x, int y, int w) {
    if (y >= 0 && y < tim.h && w > 0) {
        for (int i = MAX(x, 0); i < MIN(x + w, tim.w); i++) {
            struct cell c = tim.cells[i + y * tim.w];
            tim.cells[i + y * tim.w].fg = c.bg;
            tim.cells[i + y * tim.w].bg = c.fg;
        }
    }
}

/* scope **********************************************************************/

// enter layout scope
#define scope(x, y, w, h) \
    for (int _i = enter_scope((x), (y), (w), (h)); _i; _i = exit_scope())

// convert relative (scoped) to absolute (screen) coordinates
static struct rect abs_xywh(int x, int y, int w, int h) {
    struct rect p = tim.scopes[tim.scope]; // parent scope

    x = (x == A && w == A) ? 0 : x; // special cases
    y = (y == A && h == A) ? 0 : y; //
    w = (w == A) ? ~0 : w;          //
    h = (h == A) ? ~0 : h;          //
                                    //
    if (w < 0) {                    //
        w += p.w - x + 1;           // get w from parent
    }                               //
    if (h < 0) {                    //
        h += p.h - y + 1;           // get h from parent
    }                               //
    if (x == A) {                   //
        x = p.x + (p.w - w) / 2;    // center x on parent
    } else {                        //
        if (x < 0) {                //
            x += p.w - w + 1;       // anchor x to right
        }                           //
        x += p.x;                   // anchor x to left
    }                               //
    if (y == A) {                   //
        y = p.y + (p.h - h) / 2;    // center y on parent
    } else {                        //
        if (y < 0) {                //
            y += p.h - h + 1;       // anchor y to bottom
        }                           //
        y += p.y;                   // anchor y to top
    }

    return (struct rect){x, y, w, h};
}

// enter scope and push coordinates on stack
static inline int enter_scope(int x, int y, int w, int h) {
    if (tim.scope + 1 >= MAX_SCOPE) {
         return 0;
    }
    struct rect r = abs_xywh(x, y, w, h);
    tim.scope += 1;
    tim.scopes[tim.scope] = r;
    return 1;
}

// exit scope and pop stack
static inline int exit_scope(void) {
    tim.scope -= (tim.scope > 0);
    return 0;
}

/* frame **********************************************************************/

// frame
// color: background, frame
static inline void frame(int x, int y, int w, int h, uint64_t color) {
    if (tim.event.type == DRAW_EVENT) {
        struct rect r = abs_xywh(x, y, w, h);
        draw_box(r.x, r.y, r.w, r.h, color, color >> 8);
    }
}

/* label **********************************************************************/

// text label
// str  : text - supports multiple lines
// color: background, text
static inline void label(const char* str, int x, int y, int w, int h,
                         uint64_t color) {
    if (tim.event.type == DRAW_EVENT) {
        struct text s = scan_str(str);
        w = (w == A) ? s.width : w;
        h = (h == A) ? s.lines : h;
        struct rect r = abs_xywh(x, y, w, h);
        struct cell c = cell(" ", color, color >> 8);
        draw_lot(c, r.x, r.y, r.w, r.h);
        struct line l = {str};
        for (int i = 0; next_line(&l); i++) {
            draw_str(l.line, r.x, r.y + i, l.width, c.fg, c.bg);
        }
    }
}

/* button *********************************************************************/

// button - returns true on click
// color: frame, background, text
static inline bool button(const char* txt, int x, int y, int w, int h,
                          uint64_t color) {
    int tw = utflen(txt);
    w      = (w == A) ? (tw + 4) : w;
    h      = (h == A) ? 3 : h;
    struct rect r = abs_xywh(x, y, w, h);

    if (tim.event.type == DRAW_EVENT) {
        draw_box(r.x, r.y, r.w, r.h, color >> 16, color >> 8);
        draw_str(txt, r.x + (w - tw) / 2, r.y + h / 2, w, color, color >> 8);
    }
    return is_click_over(r);
}

/* edit ***********************************************************************/

static void edit_insert(struct edit* e, const char* s) {
    int dst_size = ztrlen(e->str);
    int src_size = ztrlen(s);
    if (dst_size + src_size + 1 < sizeof(e->str)) {
        int len = utflen(s); // usually 1, except when smashing keys
        int cur = utfpos(e->str, e->cursor);
        memmove(e->str + cur + src_size, e->str + cur, dst_size - cur);
        memmove(e->str + cur, s, src_size);
        e->str[dst_size + src_size + 1] = 0;
        e->length += len;
        e->cursor += len;
    }
}

static void edit_delete(struct edit* e) {
    int size = ztrlen(e->str);
    int cur  = utfpos(e->str, e->cursor);
    int len  = utfpos(e->str + cur, 1);
    if (size - cur > 0) {
        memmove(e->str + cur, e->str + cur + len, size - cur);
        e->length -= 1;
    }
}

static bool edit_event(struct edit* e, struct rect r) {
    if (is_click_over(r)) {
        // take focus
        tim.focus = (uintptr_t)e;
        return false;
    }

    if (tim.focus != (uintptr_t)e || tim.event.type != KEY_EVENT) {
        // not focused or no key press
        return false;
    }
    tim.event.type = VOID_EVENT; // consume event

    switch (tim.event.key) {
    case ENTER_KEY:
        tim.focus = 0; // release focus
        return true;
    case DELETE_KEY:
        edit_delete(e);
        break;
    case BACKSPACE_KEY:
        if (e->cursor > 0) {
            e->cursor -= 1;
            edit_delete(e);
        }
        break;
    case LEFT_KEY:
        e->cursor -= (e->cursor > 0);
        break;
    case RIGHT_KEY:
        e->cursor += (e->cursor < e->length);
        break;
    case HOME_KEY:
        e->cursor = 0;
        break;
    case END_KEY:
        e->cursor = e->length;
        break;
    case ESCAPE_KEY:
        tim.focus = 0; // release focus
        break;
    default:
        if (tim.event.key >= ' ') {
            edit_insert(e, tim.event.str);
        }
        break;
    }

    return false;
}

// text edit - value in state
// e    : persistent edit state
// color: frame, background, text
static inline bool edit(struct edit* e, int x, int y, int w, uint64_t color) {
    struct rect r = abs_xywh(x, y, w, 3);

    // uninitialized
    if (e->str[0] && e->cursor == 0 && e->length == 0) {
        e->length = utflen(e->str);
        e->cursor = e->length;
    }

    draw_box(r.x, r.y, r.w, r.h, color >> 16, color >> 8);

    if (tim.focus == (uintptr_t)e) {
        int cur = MIN(r.w - 4, e->cursor);
        int off = utfpos(e->str, e->cursor - r.w + 4);
        draw_str(e->str + off, r.x + 2, r.y + 1, r.w - 3, color, color >> 8);
        draw_invert(r.x + cur + 2, r.y + 1, 1);
    } else {
        draw_str(e->str, r.x + 2, r.y + 1, r.w - 3, color, color >> 8);
    }

    return edit_event(e, r);
}

/* check **********************************************************************/

// check box - returns true when clicked
// txt  : text label
// state: persistent state, 0 unchecked, -1 semi checked, !0: checked
// color: check, background, text
static inline bool check(const char* txt, int* state, int x, int y, int w,
                         uint64_t color) {
    w = (w == A) ? utflen(txt) + 4 : w;
    struct rect r = abs_xywh(x, y, w, 1);

    if (tim.event.type == DRAW_EVENT) {
        const char* st = *state == -1 ? "-" : *state ? "x" : "  ";
        draw_str("[ ] ", r.x, r.y, 4, color, color >> 8);
        draw_str(st, r.x + 1, r.y, 1, color >> 16, color >> 8);
        draw_str(txt, r.x + 4, r.y, r.w - 4, color, color >> 8);
    }

    bool click = is_click_over(r);
    *state = click ? !*state : *state;
    return click;
}

/* radio **********************************************************************/

// radio button - return true when clicked
// txt  : text label
// state: persistent state, selected if *state == v
// v    : value
// color: radio, background, text
static inline bool radio(const char* txt, int* state, int v, int x, int y,
                         int w, uint64_t color) {
    w = (w == A) ? utflen(txt) + 4 : w;
    struct rect r = abs_xywh(x, y, w, 1);

    if (tim.event.type == DRAW_EVENT) {
        const char* st = *state == v ? "o" : " ";
        draw_str("( ) ", r.x, r.y, 4, color, color >> 8);
        draw_str(st, r.x + 1, r.y, 1, color >> 16, color >> 8);
        draw_str(txt, r.x + 4, r.y, r.w - 4, color, color >> 8);
    }

    bool click = is_click_over(r);
    *state = click ? v : *state;
    return click;
}

/* rendering ******************************************************************/

// write character to output buffer
static inline void put_chr(char c) {
    if (tim.buf_size + 1 < MAX_BUF) {
        tim.buf[tim.buf_size] = c;
        tim.buf_size += 1;
    }
}

// write string to output buffer
static inline void put_str(const char* s, int size) {
    if (size > 0 && tim.buf_size + size < MAX_BUF) {
        memmove(&tim.buf[tim.buf_size], s, size);
        tim.buf_size += size;
    }
}

// write integer as decimal string to output buffer
static inline void put_int(int i) {
    // optimized for small positive values, reduces load by a third
    char* buf = &tim.buf[tim.buf_size];
    if (tim.buf_size + 11 >= MAX_BUF) {
        // not enough space for 32 bit integer
    } else if ((unsigned)i < 10) {
        buf[0] = '0' + i;
        tim.buf_size += 1;
    } else if ((unsigned)i < 100) {
        buf[0] = '0' + i / 10;
        buf[1] = '0' + i % 10;
        tim.buf_size += 2;
    } else if ((unsigned)i < 1000) {
        buf[0] = '0' + i / 100;
        buf[1] = '0' + (i / 10) % 10;
        buf[2] = '0' + i % 10;
        tim.buf_size += 3;
    } else {
        tim.buf_size += sprintf(buf, "%d", i); 
    }
}

static void render(void) {
    int  fg   = -1;
    int  bg   = -1;
    bool wide = false;
    bool skip = false;

    // screen buffers
    struct cell* new_cells = tim_cells;
    struct cell* old_cells = tim_cells;
#if ENABLE_DBUF
    new_cells += (tim.frame & 1) ? MAX_CELLS : 0;
    old_cells += (tim.frame & 1) ? 0 : MAX_CELLS;
#endif
    tim.buf_size = 0;

    for (int i = 0; i < tim.w * tim.h; i++) {
        struct cell c = new_cells[i];
#if ENABLE_DBUF
        // do nothing if cells in look-ahead are identical
        const int la = 8; // look-ahead
        if (!tim.resized && !(i % la) && (i + la < MAX_CELLS) &&
            !memcmp(new_cells + i, old_cells + i, la * sizeof(c))) {
            skip = true;
            i    = i + la - 1;
            continue;
        }
#endif
        // Set cursor position after a new line, after a string containing wide
        // characters or after skipping identical cells.
        bool new_line   = i % tim.w == 0;
        bool wide_spill = wide && (c.n == 0 || c.buf[0] == ' ');
        bool wide_flank = wide && !wide_spill && !c.wide;
        if (new_line || wide_flank || skip) {
            put_str(S("\33["));
            put_int((i / tim.w) + 1);
            put_chr(';');
            put_int((i % tim.w) + 1);
            put_chr('H');
        }
        wide = c.wide || wide_spill;
        skip = false;

        // change foreground color
        if (c.fg != fg) {
            fg = c.fg;
            put_str(S("\33[38;5;"));
            put_int(fg);
            put_chr('m');
        }

        // change background color
        if (c.bg != bg) {
            bg = c.bg;
            put_str(S("\33[48;5;"));
            put_int(bg);
            put_chr('m');
        }

        // write character
        if (c.n) {
            put_str((char*)c.buf, c.n);
        } else {
            put_chr(' ');
        }
    }

    // duration depends on connection and terminal rendering speed
    write_str(tim.buf, tim.buf_size);

    tim.resized = false;
    tim.frame  += 1;         // frame counter
    tim.cells   = old_cells; // swap buffer
}

/* event loop *****************************************************************/

static bool tim_run(float fps) {
    int timeout = (fps > 0) ? (int)(1000 / fps) : 0;

    while (true) {
        switch (tim.loop_stage) {
        case 0:
            // runs only once
            init_terminal();
            atexit(reset_terminal);
            // fallthru
        case 1:
            // process input event
            tim.start_us = time_us();
            if (tim.event.type != DRAW_EVENT) {
                // reset focus on mouse click
                if (is_event_key(MOUSE_EVENT, LEFT_BUTTON)) {
                    tim.focus = 0;
                }
                tim.loop_stage = 2;
                return true;
            }
            // fallthru
        case 2:
            // process draw event
            clear_cells();
            tim.event.type = DRAW_EVENT;
            tim.loop_stage = 3;
            return true;
        case 3:
            // render screen and wait for next event
            render();
            tim.render_us = time_us() - tim.start_us;
            read_event(timeout); // blocks
            // fallthru
        default:
            tim.loop_stage = 1;
            break;
        }
    } // while
}

