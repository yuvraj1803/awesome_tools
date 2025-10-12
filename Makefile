CC       ?= gcc
CFLAGS   ?= -O2 -Wall -Wextra -Wpedantic

SRC_DIR  := src
BIN_DIR  := bin

PREFIX   ?= /usr/local
BINDIR   ?= $(PREFIX)/bin
DESTDIR  ?=
INSTALL  ?= install
MODE     ?= 0755

SRCS   := $(wildcard $(SRC_DIR)/*.c)
PROGS  := $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%,$(SRCS))

.PHONY: all install uninstall clean

all: $(BIN_DIR) $(PROGS)

$(BIN_DIR):
	@mkdir -p $@

$(BIN_DIR)/%: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ $(LDFLAGS)

install: all
	@mkdir -p "$(DESTDIR)$(BINDIR)"
	@for p in $(PROGS); do \
	  echo "$(INSTALL) -m $(MODE) $$p -> $(DESTDIR)$(BINDIR)/"; \
	  $(INSTALL) -m $(MODE) "$$p" "$(DESTDIR)$(BINDIR)/"; \
	done

uninstall:
	@for p in $(PROGS); do \
	  n=$$(basename $$p); \
	  if [ -e "$(DESTDIR)$(BINDIR)/$$n" ]; then \
	    rm -f "$(DESTDIR)$(BINDIR)/$$n"; \
	    echo "removed $(DESTDIR)$(BINDIR)/$$n"; \
	  fi; \
	done

clean:
	@rm -f $(PROGS)
	@rmdir "$(BIN_DIR)" 2>/dev/null || true
