naga: src/naga.c
	gcc -std=gnu99 src/naga.c -lusb-1.0 -o naga

install: naga
	cp naga /usr/bin/naga
	chmod +x /usr/bin/naga

uninstall:
	rm /usr/bin/naga

clean:
	rm naga
