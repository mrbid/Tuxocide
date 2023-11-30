all:
	gcc ../main.c ../glad_gl.c -I ../inc -Ofast -lglfw -lm -o tuxocide

install:
	cp tuxocide $(DESTDIR)

uninstall:
	rm $(DESTDIR)/tuxocide

clean:
	rm tuxocide