CFLAGS = -Wall -pedantic -Werror -g
LFLAGS = `sdl2-config --libs` -lSDL2_image -lSDL2_ttf -lm -lconfig

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
BREW_PREFIX := $(shell brew --prefix 2>/dev/null || echo /usr/local)
CFLAGS += -I$(BREW_PREFIX)/include
LFLAGS += -L$(BREW_PREFIX)/lib
endif
PROG = satellit
PROG_TARGET = $(addprefix $(SAT_GIT)/, $(PROG))
CXX = gcc
MD := mkdir

DIRS = generic graphics logic objects controls audio main
TDIRS = $(foreach dir, $(DIRS), $(addprefix $(SAT_WS)/, $(dir)))
INCLUDES = $(foreach dir, $(DIRS), $(addprefix -I, $(dir)))
SOURCES = $(foreach dir,$(DIRS),$(wildcard $(dir)/*.c))
OBJS = $(addprefix $(SAT_WS)/, $(SOURCES:.c=.o))

ifdef DEBUG
CFLAGS  += -DDEBUG
endif

# top-level rule to create the program.
all: $(TDIRS) $(PROG)

$(TDIRS):
	$(MD) -p $(TDIRS)

# Compiling
$(OBJS): $(SAT_WS)/%.o: ./%.c
	$(CXX) -c $(INCLUDES) $(CFLAGS) $< -o $@

# Linking the program.
$(PROG): $(OBJS)
	$(CXX) $(OBJS) -o $(PROG_TARGET) $(LFLAGS)

# cleaning everything that can be automatically recreated with "make".
clean:
	rm -f $(OBJS) $(PROG_TARGET)

