CSOURCES := $(shell find . -name "*.c")
CPPSOURCES := $(shell find . -name "*.cpp")
CFLAGS := -Wall -Wextra -Wfloat-equal -Werror -O -MMD

all: CFLAGS := $(CFLAGS) -O3
all: CXXFLAGS := $(CFLAGS)
debug: CFLAGS := $(CFLAGS) -g
debug: CXXFLAGS := $(CFLAGS)
test: CFLAGS := $(CFLAGS) -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize=null -fsanitize=bounds-strict -fstack-protector-all
test: CXXFLAGS := $(CFLAGS)

CXXFLAGS := $(CFLAGS)

LDLIBS := -lm -lpthread -lstdc++
test: LDFLAGS := -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize=null -fsanitize=bounds-strict -fstack-protector-all

all: main
debug: main
test: main

main: $(CSOURCES:%.c=%.o) $(CPPSOURCES:%.cpp=%.o) 

DEPS := $(shell find -name "*.d")
-include $(DEPS)

clean:
	rm -f main
	rm -f *.o
	rm -f *.d
