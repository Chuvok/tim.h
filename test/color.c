// Shows xterm-256 color palette.

#include "../tim.h"

static void foo(int x, int y, int c) {
    char buf[16] = {0};
    sprintf(buf, " %02x ", c);
    draw_str(buf, x * 4, y, 4, 0, c);
}

int main(void) {
    while (tim_run(0)) {
        for (int i = 0; i < 16; i++) {
            foo(i % 8, i / 8, i);
        }
        for (int i = 0; i < 108; i++) {
            foo(i % 6, i / 6 + 3, i + 16);
        }
        for (int i = 0; i < 108; i++) {
            foo(i % 6 + 7, i / 6 + 3, i + 124);
        }
        for (int i = 0; i < 24; i++) {
            foo(i % 12, i / 12 + 22, i + 232);
        }
        if (is_key_press('q') || is_key_press(ESCAPE_KEY)) {
            exit(1);
        }
    }
}

