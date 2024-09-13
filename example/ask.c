// Display a yes/no dialog with a message. Returns with 0 when yes was clicked.
// syntax: ./ask "message"

#include "../tim.h"

// colors
#define CTXT 0xf     // text   black, white
#define CFR  0x8     // frame  black, gray
#define CYES 0xa000f // yes    green, black, white
#define CNO  0x9000f // no     red, black  white

int main(int argc, char** argv) {
    if (argc < 2 || strcmp(argv[1], "-h") == 0) {
        printf("syntax: %s message\n", argv[0]);
        exit(1);
    }

    // get text properties
    struct text msg = scan_str(argv[1]);

    while (tim_run(0)) {
        // calculate size of message box
        int w = MAX(msg.width + 4, 24);
        int h = MAX(msg.lines + 6, 7);

        scope (A, A, w, h) {
            // draw frame around entire scope
            frame(0, 0, ~0, ~0, CFR);

            // draw message
            label(argv[1], A, 1, msg.width, msg.lines, CTXT);

            // draw 'yes' button, return 0 when clicked
            if (button("Yes", 2, ~1, A, A, CYES)) {
                exit(0);
            }

            // draw 'no' button, return 1 when clicked
            if (button("No ", ~2, ~1, A, A, CNO)) {
                exit(1);
            }

            // return with 1 when q or esc is pressed
            if (is_key_press('q') || is_key_press(ESCAPE_KEY)) {
                exit(1);
            }
        }
    }
}

