#include "para.h"
#define ARGC_ERROR 1
#define TOOMANYFILES_ERROR 2
#define CONFLICTING_OUTPUT_OPTIONS 3
#define MAXSTRINGS 1024
#define MAXPARAS 4096
#define HASHLEN 200

char *strings1[MAXSTRINGS], *strings2[MAXSTRINGS];
int showversion = 0, showbrief = 0, ignorecase = 0, report_identical = 0, showsidebyside = 0;
int showleftcolumn = 0, showunified = 0, showcontext = 0, suppresscommon = 0, diffnormal = 0;
int count1 = 0, count2 = 0;

void version(void);
void loadfiles(const char* filename1, const char* filename2);
void diff_output_conflict_error(void);
void setoption(const char* arg, const char* s, const char* t, int* value);
void init_options_files(int argc, const char* argv[], para* p, para* q);
void show_brief(para* p, para* q);
void ignore_case(para* p, para* q);
void reportidentical(para* p, para* q);
void diff_normal(para* p, para* q);
void side_by_side(para* p, para* q);
void left_column(para* p, para* q);
void suppress_common(para* p, para* q);
void show_context(para* p, para* q);
void show_unified(para* p, para* q);
