#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "diff.h"
#include "para.c"
#include "util.c"

char buf[BUFLEN];

void version(void) {
  printf("\n\n\ndiff (CSUF diffutils) 1.0.0\n");
  printf("Copyright (C) 2014 CSUF\n");
  printf("This program comes with NO WARRANTY, to the extent permitted by law.\n");
  printf("You may redistribute copies of this program\n");
  printf("under the terms of the GNU General Public License.\n");
  printf("For more information about these matters, see the file named COPYING.\n");
  printf("Written by Cesar Sepulveda\n");
}

void todo_list(void) {
  printf("\n\n\nTODO: check line by line in a paragraph, using '|' for differences");
  printf("\nTODO: this starter code does not yet handle printing all of fin1's paragraphs.");
  printf("\nTODO: handle the rest of diff's options\n");
}

void loadfiles(const char* filename1, const char* filename2) {
  memset(buf, 0, sizeof(buf));
  memset(strings1, 0, sizeof(strings1));
  memset(strings2, 0, sizeof(strings2));

  FILE *fin1 = openfile(filename1, "r");
  FILE *fin2 = openfile(filename2, "r");

  while (!feof(fin1) && fgets(buf, BUFLEN, fin1) != NULL) { strings1[count1++] = strdup(buf); }  fclose(fin1);
  while (!feof(fin2) && fgets(buf, BUFLEN, fin2) != NULL) { strings2[count2++] = strdup(buf); }  fclose(fin2);
}

void print_option(const char* name, int value) { printf("%17s: %s\n", name, yesorno(value)); }

void diff_output_conflict_error(void) {
  fprintf(stderr, "diff: conflicting output style options\n");
  fprintf(stderr, "diff: Try `diff --help' for more information.)\n");
  exit(CONFLICTING_OUTPUT_OPTIONS);
}

void setoption(const char* arg, const char* s, const char* t, int* value) {
  if ((strcmp(arg, s) == 0) || ((t != NULL && strcmp(arg, t) == 0))) {
    *value = 1;
  }
}

void showoptions(const char* file1, const char* file2) {
  printf("diff options...\n");
  print_option("diffnormal", diffnormal);
  print_option("show_version", showversion);
  print_option("show_brief", showbrief);
  print_option("ignorecase", ignorecase);
  print_option("report_identical", report_identical);
  print_option("show_sidebyside", showsidebyside);
  print_option("show_leftcolumn", showleftcolumn);
  print_option("suppresscommon", suppresscommon);
  print_option("showcontext", showcontext);
  print_option("show_unified", showunified);

  printf("file1: %s,  file2: %s\n\n\n", file1, file2);

  printline();
}


void init_options_files(int argc, const char* argv[], para* p, para* q) {
  int cnt = 0;
  const char* files[2] = { NULL, NULL };

  while (argc-- > 0) {
    const char* arg = *argv;
    setoption(arg, "-v",       "--version",                  &showversion);
    setoption(arg, "-q",       "--brief",                    &showbrief);
    setoption(arg, "-i",       "--ignore-case",              &ignorecase);
    setoption(arg, "-s",       "--report-identical-files",   &report_identical);
    setoption(arg, "--normal", NULL,                         &diffnormal);
    setoption(arg, "-y",       "--side-by-side",             &showsidebyside);
    setoption(arg, "--left-column", NULL,                    &showleftcolumn);
    setoption(arg, "--suppress-common-lines", NULL,          &suppresscommon);
    setoption(arg, "-c",       "--context",                  &showcontext);
    setoption(arg, "-u",       "showunified",                &showunified);
    if (arg[0] != '-') {
      if (cnt == 2) {
        fprintf(stderr, "apologies, this version of diff only handles two files\n");
        fprintf(stderr, "Usage: ./diff [options] file1 file2\n");
        exit(TOOMANYFILES_ERROR);
      } else { files[cnt++] = arg; }
    }
    ++argv;   // DEBUG only;  move increment up to top of switch at release
  }

  if (!showcontext && !showunified && !showsidebyside && !showleftcolumn) {
    diffnormal = 1;
  }

  if (showversion) { version();  exit(0); }

  if (((showsidebyside || showleftcolumn) && (diffnormal || showcontext || showunified)) ||
      (showcontext && showunified) || (diffnormal && (showcontext || showunified))) {
        diff_output_conflict_error();
        exit(0);
  }
  if(showbrief){ show_brief(p,q); }
  if(ignorecase){
    ignore_case(p,q);
    return;
   }
  if(report_identical){ reportidentical(p,q); }
  if(diffnormal){
    diff_normal(p,q);
    return;
  }
  if(showsidebyside){
    side_by_side(p,q);
    return;
  }
  if(showleftcolumn){
    left_column(p,q);
    return;
  }
  if(suppresscommon){ suppress_common(p,q); }
  if(showcontext){ show_context(p,q); }
  if(showunified){ show_unified(p,q); }

//  showoptions(files[0], files[1]);
  loadfiles(files[0], files[1]);
}

void show_brief(para* p, para* q){
  while(p != NULL && q != NULL && para_equal(p,q) == 0){
    p = para_next(p);
    q = para_next(q);
  }
  if(p == NULL && q == NULL){
    return;
  }
  printf("Files %s and %s differ\n", *strings1, *strings2);
}

void ignore_case(para* p, para* q){
  if(showleftcolumn){
    para* qlast = q;
    int foundmatch = 0;
    while (p != NULL) {
      qlast = q;
      foundmatch = 0;
      while (q != NULL && (foundmatch = ipara_equal(p, q)) == 0) {
        q = para_next(q);
      }
      q = qlast;

      if (foundmatch) {
        while ((foundmatch = ipara_equal(p, q)) == 0) {
          q = para_next(q);
          qlast = q;
        }
        p = para_next(p);
        q = para_next(q);
      } else {
        para_print(p, printleft);
        p = para_next(p);
      }
    }
    while (q != NULL) {
      q = para_next(q);
    }
    return;
  }
  else if(showsidebyside){
    para* qlast = q;
    int foundmatch = 0;
    while (p != NULL) {
      qlast = q;
      foundmatch = 0;
      while (q != NULL && (foundmatch = ipara_equal(p, q)) == 0) {
        q = para_next(q);
      }
      q = qlast;

      if (foundmatch) {
        while ((foundmatch = ipara_equal(p, q)) == 0) {
          para_print(q, printright);
          q = para_next(q);
          qlast = q;
        }
        para_print(q, printboth);
        p = para_next(p);
        q = para_next(q);
      } else {
        para_print(p, printleft);
        p = para_next(p);
      }
    }
    while (q != NULL) {
      para_print(q, printright);
      q = para_next(q);
    }
    return;
  }
  else if(suppresscommon){
    para* qlast = q;
    int foundmatch = 0;
    while (p != NULL) {
      qlast = q;
      foundmatch = 0;
      while (q != NULL && (foundmatch = ipara_equal(p, q)) == 0) {
        q = para_next(q);
      }
      q = qlast;

      if (foundmatch) {
        while ((foundmatch = ipara_equal(p, q)) == 0) {
          para_print(q, printright);
          q = para_next(q);
          qlast = q;
        }
        p = para_next(p);
        q = para_next(q);
      } else {
        para_print(p, printleft);
        p = para_next(p);
      }
    }
    while (q != NULL) {
      para_print(q, printright);
      q = para_next(q);
    }
  }
  para* qlast = q;
  int foundmatch = 0;
  while (p != NULL) {
    qlast = q;
    foundmatch = 0;
    while (q != NULL && (foundmatch = ipara_equal(p, q)) == 0) {
      q = para_next(q);
    }
    q = qlast;

    if (foundmatch) {
      while ((foundmatch = ipara_equal(p, q)) == 0) {
        para_print(q, iprintright);
        q = para_next(q);
        qlast = q;
      }
      p = para_next(p);
      q = para_next(q);
    } else {
      para_print(p, printleft);
      p = para_next(p);
    }
  }
  while (q != NULL) {
    para_print(q, iprintright);
    q = para_next(q);
  }
}

void reportidentical(para* p, para* q){
  while(p != NULL && q != NULL && para_equal(p,q) == 0){
    p = para_next(p);
    q = para_next(q);
  }
  if(p == NULL && q == NULL){
    printf("Files %s and %s are identical\n", *strings1, *strings2);
    exit(0);
  }
}

void diff_normal(para* p, para* q){
  para* qlast = q;
  int foundmatch = 0;
  while (p != NULL) {
    qlast = q;
    foundmatch = 0;
    while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
      q = para_next(q);
    }
    q = qlast;

    if (foundmatch) {
      while ((foundmatch = para_equal(p, q)) == 0) {
        para_print(q, iprintright);
        q = para_next(q);
        qlast = q;
      }
      p = para_next(p);
      q = para_next(q);
    } else {
      para_print(p, printleft);
      p = para_next(p);
    }
  }
  while (q != NULL) {
    para_print(q, iprintright);
    q = para_next(q);
  }
}

void side_by_side(para* p, para* q){
  para* qlast = q;
  int foundmatch = 0;
  while (p != NULL) {
    qlast = q;
    foundmatch = 0;
    while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
      q = para_next(q);
    }
    q = qlast;

    if (foundmatch) {
      while ((foundmatch = para_equal(p, q)) == 0) {
        para_print(q, printright);
        q = para_next(q);
        qlast = q;
      }
      para_print(q, printboth);
      p = para_next(p);
      q = para_next(q);
    } else {
      para_print(p, printleft);
      p = para_next(p);
    }
  }
  while (q != NULL) {
    para_print(q, printright);
    q = para_next(q);
  }
}

void left_column(para* p, para* q){
  para* qlast = q;
  int foundmatch = 0;
  while (p != NULL) {
    qlast = q;
    foundmatch = 0;
    while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
      q = para_next(q);
    }
    q = qlast;

    if (foundmatch) {
      while ((foundmatch = para_equal(p, q)) == 0) {
        q = para_next(q);
        qlast = q;
      }
      p = para_next(p);
      q = para_next(q);
    } else {
      para_print(p, printleft);
      p = para_next(p);
    }
  }
  while (q != NULL) {
    q = para_next(q);
  }
}

void suppress_common(para* p, para* q){
  para* qlast = q;
  int foundmatch = 0;
  while (p != NULL) {
    qlast = q;
    foundmatch = 0;
    while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
      q = para_next(q);
    }
    q = qlast;

    if (foundmatch) {
      while ((foundmatch = para_equal(p, q)) == 0) {
        para_print(q, printright);
        q = para_next(q);
        qlast = q;
      }
      p = para_next(p);
      q = para_next(q);
    } else {
      para_print(p, printleft);
      p = para_next(p);
    }
  }
  while (q != NULL) {
    para_print(q, printright);
    q = para_next(q);
  }
}

void show_context(para* p, para* q){
  para* qlast = q;
  int foundmatch = 0;
  while (p != NULL) {
    qlast = q;
    foundmatch = 0;
    while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
      q = para_next(q);
    }
    q = qlast;

    if (foundmatch) {
      while ((foundmatch = para_equal(p, q)) == 0) {
        para_print(q, cprintright);
        q = para_next(q);
        qlast = q;
      }
      p = para_next(p);
      q = para_next(q);
    } else {
      para_print(p, cprintleft);
      p = para_next(p);
    }
  }
  while (q != NULL) {
    para_print(q, cprintright);
    q = para_next(q);
  }
}

void show_unified(para* p, para* q){
  para* qlast = q;
  int foundmatch = 0;
  while (p != NULL) {
    qlast = q;
    foundmatch = 0;
    while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
      q = para_next(q);
    }
    q = qlast;

    if (foundmatch) {
      while ((foundmatch = para_equal(p, q)) == 0) {
        para_print(q, cprintright);
        q = para_next(q);
        qlast = q;
      }
      para_print(q, cprintboth);
      p = para_next(p);
      q = para_next(q);
    } else {
      para_print(p, cprintleft);
      p = para_next(p);
    }
  }
  while (q != NULL) {
    para_print(q, cprintright);
    q = para_next(q);
  }
}

int main(int argc, const char * argv[]) {
  para* p = para_first(strings1, count1);
  para* q = para_first(strings2, count2);

  init_options_files(--argc, ++argv, p, q);

  return 0;
}
