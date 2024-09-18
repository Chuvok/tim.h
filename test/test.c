#include "../tim.h"

static inline void test_screen(struct event* e) {
    static struct event me;
    static struct event ke;
    static int          render_us;
    char                buf[64];

    ke = (e->type == KEY_EVENT)   ? *e : ke;
    me = (e->type == MOUSE_EVENT) ? *e : me;

    // positioning
    label("+",  0,  0, A, A, 0xf);
    label("+", ~0,  0, A, A, 0xf);
    label("+",  0, ~0, A, A, 0xf);
    label("+", ~0, ~0, A, A, 0xf);
    label("+",  A,  A, A, A, 0xf);
    label("-",  0,  A, A, A, 0xf);
    label("-", ~0,  A, A, A, 0xf);
    label("|",  A,  0, A, A, 0xf);
    label("|",  A, ~0, A, A, 0xf);

    // some information
    sprintf(buf, "screen: %dx%d", tim.w, tim.h);
    label(buf, 2, 0, A, A, 0xf);
    sprintf(buf, "frame : [%c] %d",  ": "[tim.frame & 1], tim.frame);
    label(buf, 2, 1, A, A, 0xf);
    sprintf(buf, "key   : [%d] %s", ke.key, ke.str + (ke.key < 32));
    label(buf, 2, 2, A, A, 0xf);
    sprintf(buf, "mouse : [%d] %d:%d", me.key, me.x, me.y);
    label(buf, 2, 3, A, A, 0xf);
    sprintf(buf, "input : %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx",
            e->str[0], e->str[1], e->str[2], e->str[3], e->str[4], e->str[5], e->str[6], e->str[7]);
    label(buf, 2, 4, A, A, 0xf);

    // lower right
    render_us += tim.render_us;
    sprintf(buf, "%d µs (Ø %d µs)", tim.render_us, render_us / MAX(tim.frame, 1));
    label(buf, ~2, ~2, A, A, 0xf);
    sprintf(buf, "%d cells (%.0f %%)", tim.w * tim.h, 100.0 * tim.w * tim.h / MAX_CELLS);
    label(buf, ~2, ~1, A, A, 0xf);
    sprintf(buf, "%d bytes (%.0f %%)", tim.buf_size, 100.0 * tim.buf_size / MAX_BUF);
    label(buf, ~2, ~0, A, A, 0xf);

    // multi line label
    label("multi\nliñe\nlabël", 24, 1, A, A, 0xf);

    // colors
    scope (1, 5, 16, 5) {
        frame(0, 0, ~0, ~0, 0xf);
        label(" Red   ", 1, 1, 7, A, 0x0900);
        label("       ", 8, 1, 7, A, 0xc400);
        label(" Green ", 1, 2, 7, A, 0x0a00);
        label("       ", 8, 2, 7, A, 0x2e00);
        label(" Blue  ", 1, 3, 7, A, 0x0c00);
        label("       ", 8, 3, 7, A, 0x1500);
    }

    // button
    static uint64_t bc = 0x100;
    if (button("Click Me", 17, 5, 16, 5, bc)) {
        bc = (bc + 0x100) & 0xff00;
    }

    // edit
    static struct edit ed1 = {.str = "Edit 1"};
    static struct edit ed2 = {0};
    edit(&ed1, 1, 10, 32, 0xff00ff);
    sprintf(buf, "cursor: %d length: %d", ed1.cursor, ed1.length);
    label(buf, 2, 13, A, A, 0xf);
    edit(&ed2, 1, 14, 32, 0xff00ff);
    label(ed2.str, 2, 17, A, A, 0xf);

    // checkbox
    static int chk[2] = {-1, 1};
    check("Check 1", &chk[0],  1, 18, A, 0xa000f);
    check("Check 2", &chk[1], 14, 18, A, 0xa000f);

    // radiobox
    static int rad = 0;
    radio("Radio 1", &rad, 1,  1, 19, A, 0xa000f);
    radio("Radio 2", &rad, 2, 14, 19, A, 0xa000f);
    radio("Radio 3", &rad, 3,  1, 20, A, 0xa000f);
    radio("Radio 4", &rad, 4, 14, 20, A, 0xa000f);

    // scope nesting
    scope(~1, 1, 20, 10) {
        scope(0, 0, 10, 5) {
            frame(0, 0, ~0, ~0, 0x9);
        }
        scope(~0, 0, 10, 5) {
            frame(0, 0, ~0, ~0, 0xa);
        }
        scope(~0, ~0, 10, 5) {
            frame(0, 0, ~0, ~0, 0xb);
        }
        scope(0, ~0, 10, 5) {
            frame(0, 0, ~0, ~0, 0xc);
        }
    }

    // funny characters
    scope (~1, ~3, 11, 5) {
        frame(0, 0, ~0, ~0, 0xf);
        label("123456789", 1, 1, 9, A, 0x0f05);
        label("$£ह€𐍈6789", 1, 2, A, A, 0x0f05);
        label("圍棋56789", 1, 3, A, A, 0x0f05);
    }
}

int main(void) {
    while (tim_run(1.5)) {
        test_screen(&tim.event);
        if (is_key_press('q') || is_key_press(ESCAPE_KEY)) {
            break;
        }
    }
}

