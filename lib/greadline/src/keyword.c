#include "greadline.h"

typedef char *(*KeyFunc)(int);
typedef int (*ColorFunc)(int);
typedef int (*HideFlagFunc)(int);
typedef int (*SizeFunc)(void);
typedef int (*SpecificKeyFunc)(char *);
typedef char **(*SpecificListFunc)(char *);

static KeyFunc get_key_word = NULL;
static ColorFunc get_key_word_color = NULL;
static SizeFunc get_key_word_size = NULL;
static HideFlagFunc get_key_word_hide_flag = NULL;
static SpecificKeyFunc is_specific_key_word = NULL;
static SpecificListFunc get_specific_key_word_list = NULL;

char **getList(void)
{
	char **comp_list = (char **)gmalloc(sizeof(char *) * MAX_LIST_SIZE);
	int i = 0;
	if (get_key_word_size != NULL) {
		for (i = 0; i < get_key_word_size(); i++) {
			char *tmp = get_key_word(i);
			comp_list[i] = (char*)gmalloc(strlen(tmp) + 1);
			snprintf(comp_list[i], strlen(tmp) + 1, "%s", tmp);
		}
	}
	return comp_list;
}

int getColor(char *name)
{
	int color = 0;
	int i = 0;
	if (get_key_word_size != NULL) {
		for (i = 0; i < get_key_word_size(); i++) {
			char *key_word = get_key_word(i);
			if (strlen(name) == strlen(key_word)) {
				if (!strncmp(name, key_word, strlen(key_word))) {
					color = get_key_word_color(i);
				}
			}
		}
	}
	return color;
}

int getHideFlag(int index)
{
	return get_key_word_hide_flag(index);
}

void setKeyWords(KeyFunc func1, ColorFunc func2, HideFlagFunc func3, SizeFunc func4)
{
	get_key_word = func1;
	get_key_word_color = func2;
	get_key_word_hide_flag = func3;
	get_key_word_size = func4;
}

void setSpecificKeyWords(SpecificKeyFunc func1, SpecificListFunc func2)
{
	is_specific_key_word = func1;
	get_specific_key_word_list = func2;
}

int isSpecificKeyWord(char *name)
{
	if (is_specific_key_word(name)) {
		return TRUE;
	}
	return FALSE;
}

char **getSpecificCompList(char *name)
{
	return get_specific_key_word_list(name);
}

void deleteList(char **list)
{
	int i = 0;
	if (get_key_word_size != NULL) {
		for (i = 0; i < get_key_word_size(); i++) {
			free(list[i]);
		}
		free(list);
	}
}
