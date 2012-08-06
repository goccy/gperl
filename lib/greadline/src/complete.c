#include "greadline.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#ifdef __cplusplus
extern "C" {
#endif

static int line_index = 0;
static int end_buf = 0;
static int cur_token = 0;
static char **token;
static int line_top_offset = 0;
static unsigned int max_complete_depth = 1;
static char yank_line[MAX_BUF];
static int history_depth = 0;

Completion *new_Completion(void)
{
	Completion *ret = (Completion *)gmalloc(sizeof(Completion));
	int i = 0;
	ret->completion_list = (char **)gmalloc(sizeof(char *) * MAX_COMP_SIZE);
	for (i = 0; i < MAX_COMP_SIZE; i++) {
		ret->completion_list[i] = (char *)gmalloc(MAX_BUF);
	}
	ret->comp_num = 0;
	ret->tab_press_count = 0;
	return ret;
}

void setCompletionList(char **list, char *line, Completion *c)
{
	token = tokenizer(token, line);
	line = token[cur_token];
	unsigned int i = 0;
	c->tab_press_count++;
	c->comp_num = 0;
	if (cur_token > 0 && isSpecificKeyWord(token[cur_token - 1])) {
		//need to display all key word
		list = getSpecificCompList(token[cur_token - 1]);
	} else if (cur_token > 1 && isSpecificKeyWord(token[cur_token - 2])) {
		//need to auto complete key word
		list = getSpecificCompList(token[cur_token - 2]);
	}
	while (list[i] != EOL) {
		if (!strncmp(line, list[i], strlen(line)) || line[0] == ' ') {
			snprintf(c->completion_list[c->comp_num], strlen(list[i]) + 1, "%s", list[i]);
			c->comp_num++;
		}
		i++;
	}
}

void autoCompletion(Completion *c, char *line)
{
	unsigned int i = 0;
	unsigned int j = 0;
	if (c->comp_num == 1) {
		token = tokenizer(token, line);
		c->comp_num--;
		if (strncmp(token[cur_token], c->completion_list[c->comp_num],
					strlen(c->completion_list[c->comp_num]))) {
			CUR_LEFT((int)strlen(token[cur_token]));
			line_index -= strlen(token[cur_token]);
			for (i = 0; i < strlen(c->completion_list[c->comp_num]); i++) {
				fprintf(stderr, "%c", c->completion_list[c->comp_num][i]);
				line[line_index] = c->completion_list[c->comp_num][i];
				line_index++;
				if (line_index > end_buf) {
					end_buf = line_index;
				}
				c->tab_press_count = 0;
			}
		}
	} else if (c->comp_num > 1) {
		token = tokenizer(token, line);
		for (i = strlen(token[cur_token]); i < strlen(c->completion_list[0]); i++) {
			for (j = 1; j < c->comp_num; j++) {
				if (c->completion_list[0][i] != c->completion_list[j][i]) {
					break;
				}
			}
			if (j == c->comp_num) {//match until end word
				fprintf(stderr, "%c", c->completion_list[0][i]);
				line[line_index] = c->completion_list[0][i];
				line_index++;
				if (line_index > end_buf) {
					end_buf = line_index;
				}
			} else {
				break;
			}
			c->tab_press_count = 0;
		}
	}
}

void deleteCompletion(Completion *c)
{
	int i = 0;
	for (i = 0; i < MAX_COMP_SIZE; i++) {
		free(c->completion_list[i]);
	}
	free(c->completion_list);
	free(c);
}

void printCompletionList(Completion *c)
{
	WindowSize *win = getTermSize(STDIN_FILENO);
	unsigned int i = 0;
	unsigned int j = 1;
	int line_length = line_top_offset;
	if (c->tab_press_count > 0) {
		CUR_DOWN;
		CUR_TOP(line_top_offset);
		while (i < c->comp_num) {
			if (getHideFlag(i)) {
				int l = (int)strlen(c->completion_list[i]) + 1;
				if (win->width < l + line_length) {
					CUR_DOWN;
					CUR_TOP(line_top_offset);
					j++;
					line_length = line_top_offset;
				}
				fprintf(stderr, "%s ", c->completion_list[i]);
				line_length += l;
			}
			i++;
		}
		if (j > max_complete_depth) {
			max_complete_depth = j;
		} else {
			CLEAR;
			for (i = 0; i < max_complete_depth - j; i++) {
				CUR_DOWN;
				CUR_TOP(line_top_offset);
				CLEAR;
			}
		}
		CLEAR;
		CUR_UPCAST(max_complete_depth);

		CUR_TOP(line_top_offset);
		if (line_index != 0) {
			CUR_RIGHT(line_index);
		}
	}
}

void hideCompletionList(void)
{
	unsigned int i = 0;
	CLEAR;
	for (i = 0; i < max_complete_depth; i++) {
		CUR_DOWN;
		CUR_TOP(line_top_offset);
		CLEAR;
	}
	CUR_UPCAST(max_complete_depth);
	CUR_TOP(line_top_offset);
	if (line_index != 0) {
		CUR_RIGHT(line_index);
	}
}

void deleteAction(char *line)
{
	char buf[MAX_BUF] = {0};
	if (line_index > 0) {
		CUR_LEFT(1);
		CLEAR;
	}
	int i = line_index;
	if (line_index > 0) {
		line_index--;
		end_buf--;
	}
	int j = 0;
	while (i <= end_buf) {
		buf[j] = line[i];
		i++;
		j++;
	}
	int cur_pos = line_index;
	i = 0;
	while (buf[i] != EOL) {
		fprintf(stderr, "%c", buf[i]);
		line[line_index] = buf[i];
		i++;
		line_index++;
	}
	line[line_index] = EOL;
	line_index = cur_pos;
	if (end_buf > line_index) {
		CUR_TOP(line_top_offset);
		if (line_index != 0) {
			CUR_RIGHT(line_index);
		}
	}
}

char *insert(char *base, char *target, int pos)
{
	char temp[MAX_BUF] = {0};
	strncpy(temp, &base[pos], strlen(&base[pos]));
	base[pos] = EOL;
	snprintf(base, MAX_BUF, "%s%s%s", base, target, temp);
	if (strlen(target) != 1) {
		end_buf += strlen(target);
	}
	memset(&base[end_buf + 1], 0, strlen(&base[end_buf + 1]));
	return base;
}

void addBuf(char *line, char in_char)
{
	if (line[line_index] == EOL) {
		line[line_index] = in_char;
	} else {
		char target[2] = {0};
		target[0] = in_char;
		line = insert(line, target, line_index);
	}
	line_index++;
	end_buf++;
}

char **tokenizer(char **token, char *line)
{
	int i = 0;
	int token_n = 0;
	char space[64] = {0};
	int space_index = 0;
	char temp[MAX_BUF] = {0};
	int temp_index = 0;

	for (i = 0; i < MAX_TOKEN_NUM; i++) {
		memset(token[i], 0, MAX_BUF);
	}
	cur_token = 0;
	i = 0;
	while (line[i] != EOL) {
		if (line[i] == ' ') {
			memset(space, 0, 64);
			space_index = 0;
			while (line[i] == ' ') {
				space[space_index] = ' ';
				i++;
				space_index++;
			}
			snprintf(token[token_n], strlen(space) + 1, "%s", space);
			token_n++;
		} else {
			memset(temp, 0, MAX_BUF);
			temp_index = 0;
			while (line[i] != ' ' && line[i] != EOL) {
				temp[temp_index] = line[i];
				i++;
				temp_index++;
			}
			snprintf(token[token_n], strlen(temp) + 1, "%s", temp);
			token_n++;
		}
		if (i == line_index) {
			cur_token = token_n - 1;
		}
	}
	return token;
}

void changeColor(char *line)
{
	int i = 0;
	int j = 0;
	int is_quate = 0;
	token = tokenizer(token, line);
	CUR_TOP(line_top_offset);
	for (i = 0; i < MAX_TOKEN_NUM; i++) {
		int color = getColor(token[i]);
		if (color != 0) {
			for (j = 0; j < (int)strlen(token[i]); j++) {
				CHANGE_COLOR(color);
				fprintf(stderr, "%c", token[i][j]);
				CHANGE_COLOR(WHITE);
			}
		} else {
			for (j = 0; j < (int)strlen(token[i]); j++) {
				if (token[i][j] == '"' && is_quate == 0) {
					CHANGE_COLOR(GREEN);
					fprintf(stderr, "\"");
					is_quate = 1;
					continue;
				} else if (token[i][j] == '"' && is_quate == 1) {
					is_quate = 0;
					fprintf(stderr, "\"");
					CHANGE_COLOR(WHITE);
					continue;
				}
				if (is_quate == 1) {
					CHANGE_COLOR(GREEN);
				} else {
					CHANGE_COLOR(WHITE);
				}
				fprintf(stderr, "%c", token[i][j]);
				if (token[i][j] == '\n') {
					CUR_TOP(line_top_offset);
					line_index = strlen(token[i]) - (j + 1);
				}
			}
		}
	}
	CUR_TOP(line_top_offset);
	if (line_index != 0) {
		CUR_RIGHT(line_index);
	}
}

void copyFromHistory(char *line, char *history_line)
{
	CUR_TOP(line_top_offset);
	CLEAR;
	memset(line, 0, strlen(line));
	snprintf(line, strlen(history_line) + 1, "%s", history_line);
	cur_token = 0;
	end_buf = strlen(history_line);
	line_index = end_buf;
	changeColor(line);
}

void funcAction(char key, char *line)
{
	int i;
	switch (key) {
	case UP_KEY: {
		for (i = 0; i < history_depth; i++) {
			CUR_TOP(line_top_offset);
			CLEAR;
			CUR_UPCAST(1);
		}
		char *history_line = upHistory(line_top_offset);
		copyFromHistory(line, history_line);
		break;
	}
	case DOWN_KEY: {
		for (i = 0; i < history_depth; i++) {
			CUR_TOP(line_top_offset);
			CLEAR;
			CUR_UPCAST(1);
		}
		char *history_line = downHistory(line_top_offset);
		copyFromHistory(line, history_line);
		break;
	}
	case LEFT_KEY:
		if (line_index > 0) {
			CUR_LEFT(1);
			line_index--;
		}
		break;
	case RIGHT_KEY:
		if (line_index < end_buf) {
			CUR_RIGHT(1);
			line_index++;
		}
		break;
	default :
		break;
	}
}

int checkLines(char *history_line)
{
	int i = 0;
	int count = 0;
	while (history_line[i] != EOL) {
		if (history_line[i] == '\n') {
			count++;
		}
		i++;
	}
	return count;
}

char *readBuf(struct termio tty_backup, char *init_message)
{
	char in_char[3] = {0};  //input char
	char read_byte = 0; //red bytes
	char *line = (char *)gmalloc(MAX_BUF);
	Completion *comp = new_Completion();
	char **list = getList();
	int i = 0;
	token = (char **)gmalloc(sizeof(char *) * MAX_TOKEN_NUM);
	for (i = 0; i < MAX_TOKEN_NUM; i++) {
		token[i] = (char *)gmalloc(MAX_BUF);
	}

	line_index = 0;
	end_buf = 0;
	cur_token = 0;
	fprintf(stderr, "%s", init_message);
	line_top_offset = (int)strlen(init_message);

	while (1) {
		if ((read_byte = read(0, &in_char, 3)) == 2) {
			break;
		} else if (read_byte == -1) {
			// replace prev status
			ioctl(0, TCSETAF, &tty_backup);
			//error
			return line;
		} else if (in_char[0] != EOL) {
			//fprintf(stderr, "%d", in_char[0]);
			switch (in_char[0]) {
			case ENTER:
				goto BREAK;
				break;
			case TAB:
				setCompletionList(list, line, comp);//create completion list
				autoCompletion(comp, line);//execute auto complete
				printCompletionList(comp);//print to shell
				changeColor(line);
				break;
			case CTR_A:
				CUR_TOP(line_top_offset);
				line_index = 0;
				break;
			case CTR_B:
				if (line_index > 0) {
					CUR_LEFT(1);
					line_index--;
				}
				break;
			case CTR_C:
			case CTR_D:
				break;
			case CTR_E:
				if (line_index < end_buf) {
					CUR_TOP(line_top_offset);
					CUR_RIGHT(end_buf);
					line_index = end_buf;
				}
				break;
			case CTR_F:
				if (line_index < end_buf) {
					CUR_RIGHT(1);
					line_index++;
				}
				break;
			case CTR_G:
				break;
			case CTR_H:
			case DELETE:
				if (history_depth == 0) {
					deleteAction(line);
					changeColor(line);
				}
				break;
				//case CTR_I: => TAB
			case CTR_J:
				break;
			case CTR_K:
				if (history_depth == 0) {
					CLEAR;
					memset(yank_line, 0, MAX_BUF);
					snprintf(yank_line, strlen(&line[line_index]) + 1, "%s", &line[line_index]);
					memset(&line[line_index], 0, strlen(&line[line_index]));
					end_buf = line_index;
				}
				break;
			case CTR_L:
				//case CTR_M: => ENTER
				break;
			case CTR_N: {
				for (i = 0; i < history_depth; i++) {
					CUR_TOP(line_top_offset);
					CLEAR;
					CUR_UPCAST(1);
				}
				char *history_line = downHistory(line_top_offset);
				copyFromHistory(line, history_line);
				history_depth = checkLines(history_line);
				break;
			}
			case CTR_O:
				break;
			case CTR_P: {
				for (i = 0; i < history_depth; i++) {
					CUR_TOP(line_top_offset);
					CLEAR;
					CUR_UPCAST(1);
				}
				char *history_line = upHistory(line_top_offset);
				copyFromHistory(line, history_line);
				history_depth = checkLines(history_line);
				break;
			}
			case CTR_Q:
			case CTR_R:
			case CTR_S:
			case CTR_T:
			case CTR_U:
			case CTR_V:
			case CTR_W:
			case CTR_X:
			case CTR_Y:
				if (history_depth == 0) {
					line = insert(line, yank_line, line_index);
					changeColor(line);
				}
				break;
			case CTR_Z:
				break;
			case FUNC_KEY:
				funcAction(in_char[2], line);
				break;
			default:
				addBuf(line, in_char[0]);
				changeColor(line);
				break;
			}
		} else {
			// no input
		}
		in_char[0] = EOL;
	}
BREAK:
	CUR_TOP(line_top_offset);
	CUR_RIGHT(end_buf);
	hideCompletionList();
	for (i = 0; i < MAX_TOKEN_NUM; i++) {
		free(token[i]);
	}
	free(token);
	deleteList(list);
	deleteCompletion(comp);
	return line;
}

#ifdef __cplusplus
} /* extern "C" */
#endif
