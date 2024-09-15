#include "../tim.h"                             // one header, no lib
int main(void) {                                //
    while (tim_run(0)) {                        // event loop
        scope (A, A, 24, 8) {                   // centered 24x8 scope
            uint64_t c = 0x0a060f;              // three colors
            frame(0, 0, ~0, ~0, c);             // draw frame for scope
            label("Greetings!", A, 2, A, A, c); // label in top center
            if (button("OK", A, ~1, 8, A, c))   // button in bottom center
                return 0;                       // exit on button click
            if (is_key_press('q'))              // ctrl-c is masked
                return 0;                       // exit on 'q' press
        }                                       //
    }                                           // atexit cleanup
}

