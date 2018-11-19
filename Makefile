
CC = g++
CFLAGS = -std=c++14 -Wall -O3

SRCDIR = src
BUILDDIR = build

SRCS := $(wildcard $(SRCDIR)/*.cpp)
HEADERS := $(wildcard $(SRCDIR)*.h)
OBJS := $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRCS))
TARGET = $(BUILDDIR)/satsolver

all: $(BUILDDIR) $(TARGET)
.PHONY: all
.PHONY: $(BUILDDIR)

.PHONY: debug
debug: CFLAGS = -std=c++14 -Wall -DDEBUG -g
debug: $(BUILDDIR) $(TARGET)

$(BUILDDIR):
	@mkdir -p $(BUILDDIR)

$(TARGET): $(OBJS)
	@$(CC) $(CFLAGS) $^ -o $@

$(BUILDDIR)/%.o : $(SRCDIR)/%.cpp $(HEADERS)
	@$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@rm -rf $(BUILDDIR)
