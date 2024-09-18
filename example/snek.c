// Simple game of snake to show how to do animation and draw cells.

#include "../tim.h"

#define FG  0x10
#define BG  0xdd
#define BTN (FG << 16 | BG << 8 | FG)

#define NEW   0
#define RUN   1
#define PAUSE 2
#define OVER  3

typedef union {
    struct {
        int32_t x;
        int32_t y;
    };
    int64_t xy;
} point;

static struct {
    int     state;     // game state (NEW RUN PAUSE OVER)
    int64_t tick;      // updates every 10 ms
    int     len;       // snake length
    point   body[200]; // snake body
    point   food;      // food position
    point   look;      // active direction
} snek;

static void start(void) {
    memset(snek.body, -1, sizeof(snek.body));
    snek.len     = 2;
    snek.body[0] = (point){{1, tim.h / 2}};
    snek.food    = (point){{tim.w / 8, tim.h / 2}};
    snek.look    = (point){{1, 0}};
}

static void game(void) {
    // update game state about every 10 ms
    int64_t tick = time_us() / 100000;
    if (snek.tick != tick) {
        snek.tick = tick;
        // move one unit
        memmove(snek.body + 1, snek.body, sizeof(snek.body) - sizeof(point));
        snek.body[0].x = snek.body[1].x + snek.look.x;
        snek.body[0].y = snek.body[1].y + snek.look.y;
        // self crash
        bool crash = false;
        for (int i = 1; i < snek.len; i++) {
            crash |= snek.body[0].xy == snek.body[i].xy;
        }
        // border crash
        crash |= snek.body[0].x < 0 || snek.body[0].x >= tim.w / 2 ||
                 snek.body[0].y < 0 || snek.body[0].y >= tim.h;
        snek.state = crash ? OVER : snek.state;
        // food
        if (snek.food.xy == snek.body[0].xy) {
            snek.len    = MIN(snek.len + 2, (int)ARRAY_SIZE(snek.body));
            snek.food.x = rand() % (tim.w / 2 - 2) + 1;
            snek.food.y = rand() % (tim.h - 2) + 1;
        }
    }

    // draw
    if (tim.event.type == DRAW_EVENT) {
        // food
        draw_chr(cell(" ", 0, 0xc5), snek.food.x * 2 + 0, snek.food.y);
        draw_chr(cell(" ", 0, 0xc5), snek.food.x * 2 + 1, snek.food.y);
        // snek
        struct cell s = cell(" ", 0, 0);
        for (int i = 0; i < snek.len; i++) {
            s.bg  = (i / 2) % 2 ? 0xe3 : 0xea;
            int x = snek.body[i].x * 2;
            int y = snek.body[i].y;
            draw_chr(s, x + 0, y);
            draw_chr(s, x + 1, y);
        }
    }

    // user input
    if (tim.event.type == KEY_EVENT) {
        int key = tim.event.key;
        if ((key == RIGHT_KEY || key == 'd') && snek.look.x != -1) {
            snek.look = (point){{1, 0}};
        } else if ((key == LEFT_KEY || key == 'a') && snek.look.x != 1) {
            snek.look = (point){{-1, 0}};
        } else if ((key == DOWN_KEY || key == 's') && snek.look.y != -1) {
            snek.look = (point){{0, 1}};
        } else if ((key == UP_KEY || key == 'w')  && snek.look.y != 1) {
            snek.look = (point){{0, -1}};
        }
    }
}

static void menu(void) {
    scope(A, A, 20, 13) {
        char* lbl = snek.state == OVER ? "GAME OVER" : "SNEK - THE GAME";
        char* btn = snek.state == PAUSE ? "Resume" : "Play";
        label(lbl, A, 0, A, A, BTN);
        if (button(btn, A, 2, 20, 5, BTN) || is_key_press(ENTER_KEY)) {
            if (snek.state != PAUSE) {
                start();
            }
            snek.state = RUN;
        }
        if (button("Exit", A, 8, 20, 5, BTN) || is_key_press(ESCAPE_KEY)) {
            exit(0);
        }
    }
}

int main(void) {
    // draw every 10 ms
    while (tim_run(10)) {
        struct cell bg = cell(" ", 0, BG);
        draw_lot(bg, 0, 0, tim.w, tim.h);

        if (snek.state == RUN) {
            game();
        } else {
            menu();
        }

        if (is_key_press(ESCAPE_KEY)) {
            snek.state = PAUSE;
        }
    }
}

