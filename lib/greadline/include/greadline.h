#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#if !defined(TCGETA) && defined(TIOCGETA)
#define TCGETA TIOCGETA
#define TCSETAF TIOCSETAF
#define termio termios
#endif

#define TRUE 1
#define FALSE 0 
#define MAX_BUF 256
#define MAX_TOKEN_NUM 128
#define MAX_LIST_SIZE 256
#define MAX_COMP_SIZE 256
#define EOL '\0'

enum {
	CTR_A = 1,
	CTR_B,
	CTR_C,
	CTR_D,
	CTR_E,
	CTR_F,
	CTR_G,
	CTR_H,
	CTR_I,
	CTR_J,
	CTR_K,
	CTR_L,
	CTR_M,
	CTR_N,
	CTR_O,
	CTR_P,
	CTR_Q,
	CTR_R,
	CTR_S,
	CTR_T,
	CTR_U,
	CTR_V,
	CTR_W,
	CTR_X,
	CTR_Y,
	CTR_Z,
};

#define DELETE 127
#define TAB 9
#define ENTER 13
#define SPACE 32
#define DOUBLE_QUATE 34
#define SINGLE_QUATE 39
#define FUNC_KEY 27
#define UP_KEY 'A'
#define DOWN_KEY 'B'
#define RIGHT_KEY 'C'
#define LEFT_KEY 'D'

#define CHANGE_COLOR(color) fprintf(stderr, "\x1b[%dm", color)

enum {
	BLACK = 30,
	RED,
	GREEN,
	YELLOW,
	BLUE,
	MAGENTA,
	SYAN,
	GRAY,
	DEFAULT,
	WHITE,
};

#define	CUR_UPCAST(n) fprintf(stderr, "\033[%dA", n)
#define CUR_DOWN fprintf(stderr, "\033D")
#define	CUR_TOP(offset) fprintf(stderr, "\033[%dD\033[%dC", MAX_BUF, offset)
#define CLEAR fprintf(stderr, "\033[0K")
#define	CUR_LEFT(n) fprintf(stderr, "\033[%dD", n)
#define CUR_RIGHT(n) fprintf(stderr, "\033[%dC", n)

typedef struct key_words_t {
	char *name;
	int color;
} KeyWords;

typedef struct auto_comp_t {
	char **completion_list;
	unsigned int comp_num;
	int tab_press_count;
} Completion;

typedef struct window_size_t {
	int width;
	int height;
} WindowSize;

/*================API====================*/
char *greadline(const char *init_message);
void setKeyWords(char *(*func1)(int), int (*func2)(int), int (*func3)(int), int(*func4)(void));
void setSpecificKeyWords(int (*func1)(char *), char **(*func2)(char *));
void usingHistory(void);
void addHistory(char *line);
/*=======================================*/

void *gmalloc(size_t size);
Completion *new_Completion(void);
void setCompletionList(char **list, char *line, Completion *c);
void autoCompletion(Completion *c, char *line);
void addBuf(char *line, char in_char);
void deleteAction(char *line);
void changeTermStatus(struct termio tty_backup);
char *readBuf(struct termio tty_backup, char *init_message);
int getColor(char *name);
int getHideFlag(int index);
void printCompletionList(Completion *c);
void setList(char **list);
char **getList(void);
char **tokenizer(char **token, char *line);
char *upHistory(int line_top_offset);
char *downHistory(int line_top_offset);
void deleteList(char **list);
void deleteKeyWords(void);
WindowSize *getTermSize(int fd);
int isSpecificKeyWord(char *name);
char **getSpecificCompList(char *name);
