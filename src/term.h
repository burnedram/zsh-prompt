#define RESET "0"
#define BOLD "1"

#define FG_RESET          "39"
#define FG_BLACK          "30"
#define FG_RED            "31"
#define FG_GREEN          "32"
#define FG_YELLOW         "33"
#define FG_BLUE           "34"
#define FG_MAGENTA        "35"
#define FG_CYAN           "36"
#define FG_WHITE          "37"

#define BG_RESET          "49"
#define BG_BLACK          "40"
#define BG_RED            "41"
#define BG_GREEN          "42"
#define BG_YELLOW         "43"
#define BG_BLUE           "44"
#define BG_MAGENTA        "45"
#define BG_CYAN           "46"
#define BG_WHITE          "47"

#define COLOR_1(attr) "\x1b["attr"m"
#define COLOR_2(attr1, attr2) "\x1b["attr1";"attr2"m"
