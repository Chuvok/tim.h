// Test character width.

#include <assert.h>
#include "../tim.h"

static int cp_to_utf8(int32_t cp, char* s) {
    assert(cp > 0 && cp < 0x110000);

    if (cp < 0x80) {
        s[0] = cp;
        return 1;
    } else if (cp < 0x800) {
        s[0] = (cp >> 6)   | 0xc0;
        s[1] = (cp & 0x3f) | 0x80;
        return 2;
    } else if (cp < 0x10000) {
        s[0] =  (cp >> 12)         | 0xe0;
        s[1] = ((cp >>  6) & 0x3f) | 0x80;
        s[2] = (cp         & 0x3f) | 0x80;
        return 3;
    } else {
        s[0] =  (cp >> 18)         | 0xf0;
        s[1] = ((cp >> 12) & 0x3f) | 0x80;
        s[2] = ((cp >>  6) & 0x3f) | 0x80;
        s[3] = (cp         & 0x3f) | 0x80;
        return 4;
    }

    return -1;
}

static int cursor_pos() {
    write(STDOUT_FILENO, S("\33[6n"));
    char buf[64] = {0};
    int n = read(STDIN_FILENO, buf, 64);
    if (n < 6 || buf[0] != '\33' || buf[n - 1] != 'R') {
        return -1;
    }
    int r = atoi(buf + 2);
    int c = atoi(buf + 4 + (r > 9));
    return c;
}

int main(int argc, char** argv) {
    assert(argc == 2);
    (void)tim_run;

    FILE* f = fopen(argv[1], "w");
    assert(f);

    init_terminal();

    for (int i = 32; i < 0x110000; i++) {
        write(STDOUT_FILENO, S("\33[0;0H"));
        char buf[5] = {0};
        int n = cp_to_utf8(i, buf);
        write(STDOUT_FILENO, buf, n);
        int w = cursor_pos() - 1;
        if (w) {
            fprintf(f, "u+%06x %d %s\n", i, w, buf);
        } else {
            fprintf(f, "u+%06x %d\n", i, w);
        }
    }

    reset_terminal();

    fclose(f);
}
