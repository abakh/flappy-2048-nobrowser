flappy-2048:
	$(CC) ./flappy-2048.c -lallegro -lallegro_image -lallegro_primitives -lallegro_font -lallegro_ttf -o ./flappy-2048
install: flappy-2048
	cp ./flappy-2048 $(PREFIX)/flappy-2048
