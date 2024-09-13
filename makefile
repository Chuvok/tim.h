all: out/test out/string out/color out/hello out/ask out/snek

out/test: test/test.c out
	$(CC) $< -Wall $(CFLAGS) -o $@
out/string: test/string.c out
	$(CC) $< -Wall $(CFLAGS) -o $@
out/color: test/color.c out
	$(CC) $< -Wall $(CFLAGS) -o $@
out/hello: example/hello.c out
	$(CC) $< -Wall $(CFLAGS) -o $@
out/ask: example/ask.c out
	$(CC) $< -Wall $(CFLAGS) -o $@
out/snek: example/snek.c out
	$(CC) $< -Wall $(CFLAGS) -o $@

out:
	mkdir -p out

clean:
	 rm -rf out
