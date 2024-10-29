CC = cc
COMPILER = gcc
FLAGS = -Wall -Wextra -Werror
LIBRARIES = -lm $(shell pkg-config --cflags --libs libmongoc-1.0)
RM = rm -f

SOURCE = main.c
OUTPUT = program
NAME = libogma.a
TNAME = bin

OPATH = build/objs/
RPATH = build/results/

CFILES = $(wildcard src/*.c)
OFILES = $(patsubst src/%.c, $(OPATH)%.o, $(CFILES))

all: $(NAME)

create_dirs:
	@mkdir -p $(OPATH) $(RPATH)

$(NAME): $(OFILES) | create_dirs
	ar rcs $(RPATH)$(NAME) $^
	ranlib $(RPATH)$(NAME)

$(OPATH)%.o: src/%.c | create_dirs
	$(CC) $(FLAGS) $(LIBRARIES) -c $< -o $@

clean:
	@if [ -f $(OPATH)$(NAME) ]; then $(RM) $(OPATH)$(NAME); fi

fclean: clean
	@if [ -n "$(OFILES)" ]; then $(RM) $(OFILES); fi
	@if [ -f $(TNAME) ]; then $(RM) $(TNAME); fi
	$(RM) -r $(RPATH)$(OUTPUT)
	$(RM) -r $(RPATH)$(TNAME)
	$(RM) -r $(RPATH)$(NAME)

$(OUTPUT):
	$(COMPILER) $(FLAGS) $(SOURCE) $(CFILES) $(LIBRARIES) -o $(RPATH)$(OUTPUT)

run: $(OUTPUT)
	./$(RPATH)$(OUTPUT)

gtest:
	$(COMPILER) -g $(FLAGS) $(SOURCE) $(CFILES) $(LIBRARIES) -o $(RPATH)$(TNAME) && gdb $(RPATH)$(TNAME)

memcheck: $(OUTPUT)
	valgrind --tool=memcheck --leak-check=full -s ./$(RPATH)$(OUTPUT)

.PHONY: all create_dirs clean fclean run gtest utest memcheck
.DEFAULT_GOAL = run