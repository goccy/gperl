#include "greadline.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct history_t {
	char *line;
	struct history_t *prev;
	struct history_t *next;
} History;

static History *history;
static History *search;

History *new_History(void)
{
	History *ret = (History *)gmalloc(sizeof(History));
	ret->line = (char *)gmalloc(MAX_BUF);
	ret->prev = NULL;
	ret->next = NULL;
	return ret;
}

void usingHistory(void)
{
	history = new_History();
	search = history;
}

void addHistory(char *line)
{
	if (history == NULL) {
		fprintf(stderr, "not initialized !!\n");
	}
	if (history->next == NULL) {
		//add first
		history->next = new_History();
		History *next_history = history->next;
		next_history->prev = history;
		snprintf(next_history->line, MAX_BUF, "%s", line);
	} else {
		History *next_history = history->next;
		history->next = new_History();
		History *latest_history = history->next;
		latest_history->prev = history;
		snprintf(latest_history->line, MAX_BUF, "%s", line);
		latest_history->next = next_history;
		next_history->prev = latest_history;
	}
	search = history;
}

char *upHistory(int line_top_offset)
{
	if (search->next != NULL) {
		search = search->next;
		CUR_TOP(line_top_offset);
		CLEAR;
	}
	return search->line;
}

char *downHistory(int line_top_offset)
{
	if (search->prev != NULL) {
		search = search->prev;
		CUR_TOP(line_top_offset);
		CLEAR;
	}
	return search->line;
}

#ifdef __cplusplus
} /* extern "C" */
#endif
