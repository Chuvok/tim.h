// Test string functions.

#include "../tim.h"

#define U(s)    (uint8_t*)(""s), (sizeof(s) - 1)
#define TEST(t) printf("\33[3%s\33[0m %s\n", (t) ? "2mpass" : "1mfail", #t)

int main(void) {
    (void)tim_run;

    TEST(ztrlen(NULL) == 0);
    TEST(ztrlen("")   == 0);
    TEST(ztrlen("$")  == 1);
    TEST(ztrlen("£")  == 2);
    TEST(ztrlen("€")  == 3);
    TEST(ztrlen("𐍈")  == 4);

    TEST(bsr8(128) == 0);
    TEST(bsr8(64)  == 1);
    TEST(bsr8(1)   == 7);
    TEST(bsr8(0)   == 8);

    TEST(utfchr(NULL) == 0);
    TEST(utfchr("")   == 0);
    TEST(utfchr("$")  == 0x24);
    TEST(utfchr("£")  == 0xA3);
    TEST(utfchr("И")  == 0x418);
    TEST(utfchr("ह")  == 0x939);
    TEST(utfchr("€")  == 0x20AC);
    TEST(utfchr("한")  == 0xD55C);
    TEST(utfchr("𐍈")  == 0x10348);

    TEST(utflen(NULL)  == 0);
    TEST(utflen("")    == 0);
    TEST(utflen("$")   == 1);
    TEST(utflen("$$")  == 2);
    TEST(utflen("$£")  == 2);
    TEST(utflen("$€𐍈") == 3);

    TEST(utfpos(NULL, 0)  == 0);
    TEST(utfpos("äbc", 0) == 0);
    TEST(utfpos("äbc", 1) == 2);
    TEST(utfpos("äbc", 2) == 3);
    TEST(utfpos("äbc", 9) == 4);

    TEST(scan_str(NULL).lines    == 0);
    TEST(scan_str("").lines      == 0);
    TEST(scan_str("abc").lines   == 1);
    TEST(scan_str("a\no").lines  == 2);
    TEST(scan_str("a").width     == 1);
    TEST(scan_str("äß\no").width == 2);

    struct line ln = {.str = "foo\nbar"};
    TEST(next_line(&ln) == true);
    TEST(!memcmp(ln.line, "foo", ln.size));
    TEST(next_line(&ln) == true);
    TEST(!memcmp(ln.line, "bar", ln.size));
    TEST(next_line(&ln) == false);

    TEST(is_wide_perhaps(NULL, 0) == false);
    TEST(is_wide_perhaps(U(""))   == false);
    TEST(is_wide_perhaps(U("$"))  == false);
    TEST(is_wide_perhaps(U("£"))  == false);
    TEST(is_wide_perhaps(U("ह"))  == false);
    TEST(is_wide_perhaps(U("€"))  == true);
    TEST(is_wide_perhaps(U("┌"))  == false);
    TEST(is_wide_perhaps(U("한"))  == true);
    TEST(is_wide_perhaps(U("𐍈"))  == true);
}

