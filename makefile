# ----------------------------
# Makefile Options
# ----------------------------

NAME = DDGAME
ICON = icon.png
DESCRIPTION = "Dungeons & Diagrams"
COMPRESSED = YES
ARCHIVED = NO

CFLAGS = -Wall -Wextra -Oz -Wno-pointer-sign
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)