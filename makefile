CC = /usr/bin/gcc
INC = -I/usr/local/include/gl3w
LINUXLIB = -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm -lncurses
MACOSLIB = -framework OpenGL -framework Cocoa -framework IOkit -lglfw3 -lncurses
EXEC = PACMAN

build:
	$(CC) ./src/*.c $(INC) $(MACOSLIB) -o $(EXEC)

run:
	./$(EXEC)

clean:
	rm ./$(EXEC)
