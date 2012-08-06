#include "greadline.h"
#include <sys/ioctl.h>

#ifdef __cplusplus
extern "C" {
#endif

void changeTermStatus(struct termio tty_backup)
{
	struct termio tty_change = tty_backup;
	tty_change.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
	tty_change.c_lflag &= ~(ICANON | ECHO | IEXTEN | ECHONL | ECHOKE);// | ISIG); //no echo mode(raw mode)
	tty_change.c_oflag &= ~OPOST;
	tty_change.c_cflag &= ~(CSIZE | PARENB);
	tty_change.c_cflag |= CS8;
	tty_change.c_cc[VMIN] = 0; //accept input of 0 char
	tty_change.c_cc[VTIME] = 5; //wait 0/10 sec while no input
	ioctl(0, TCSETAF, &tty_change); //changed current status
}

WindowSize *getTermSize(int fd)
{
	WindowSize *ret = (WindowSize *)gmalloc(sizeof(WindowSize));
#ifdef TIOCGSIZE
	struct ttysize win;
	if (ioctl(fd, TIOCGSIZE, &win)) {
		return ret;
	}
	ret->width = win.ts_cols;
	ret->height = win.ts_lines;
#elif defined(TIOCGWINSZ)
	struct winsize win;
	if (ioctl(fd, TIOCGWINSZ, &win)) {
		return ret;
	}
	ret->width = win.ws_col;
	ret->height = win.ws_row;
#else
	char *line = getenv("COLUMNS");
	if (line) {
		ret->width = strtol(line, NULL, 10);
	} else {
		ret->width = 80;
	}
	line = getenv("LINES");
	if (line) {
		ret->height = strtol(line, NULL, 10);
	} else {
		ret->height = 25;
	}
#endif
	return ret;
}

#ifdef __cplusplus
} /* extern "C" */
#endif
