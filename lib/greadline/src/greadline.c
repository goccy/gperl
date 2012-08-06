#include "greadline.h"
#include <sys/ioctl.h>

#ifdef __cplusplus
extern "C" {
#endif

char *greadline(const char *init_message)
{
	struct termio tty_backup; //create backup
	ioctl(0, TCGETA, &tty_backup); //save current status
	changeTermStatus(tty_backup);//set non-canonical mode
	char *line = readBuf(tty_backup, (char *)init_message);//get line
	ioctl(0, TCSETAF, &tty_backup);//replace prev status
	CHANGE_COLOR(WHITE);
	fprintf(stderr, "\n");
	return line;
}

#ifdef __cplusplus
} /* extern "C" */
#endif
