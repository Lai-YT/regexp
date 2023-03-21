/*
 * ============================================================================
 *
 *       Filename:  messages.c
 *
 *    Description:  Program messages implementation
 *
 *        Created:  24/03/2016 22:48:39
 *       Compiler:  gcc
 *
 *         Author:  Gustavo Pantuza
 *   Organization:  Software Community
 *
 * ============================================================================
 */

#include "messages.h"

#include <stdio.h>
#include <stdlib.h>

#include "colors.h"

/*
 * Help message
 */
void help() {
  fprintf(stdout, BLUE __PROGRAM_NAME__ "\n\n" NO_COLOR);
  usage();
  description();
  options();
  author();
  version();
}

/*
 * Usage message
 */
void usage() {
  fprintf(stdout, YELLOW "Usage: " NO_COLOR);
  fprintf(stdout, "%s [options] [regexp string]\n\n", __PROGRAM_NAME__);
}

/*
 * Description message
 */
void description() {
  fprintf(stdout, YELLOW "Description: " NO_COLOR);
  fprintf(stdout,
          "Matches the string (string) to the regular expression (regexp) and "
          "exits with 1 if it does not match.\n\n");
}

/*
 * Options message
 */
void options() {
  fprintf(stdout, YELLOW "Options:\n\n" NO_COLOR);
  fprintf(stdout,
          WHITE "\t-v, --version\n" NO_COLOR "\t\tPrints %s version\n\n",
          __PROGRAM_NAME__);
  fprintf(stdout,
          WHITE "\t-h, --help\n" NO_COLOR "\t\tPrints this help message\n\n");
  fprintf(stdout, WHITE "\t--no-color\n" NO_COLOR
                        "\t\tDoes not use colors for printing\n\n");
}

/*
 * Author message
 */
void author() {
  fprintf(stdout, YELLOW "Written by: " WHITE "%s\n\n" NO_COLOR,
          __PROGRAM_AUTHOR__);
}

/*
 * Version message
 */
void version() {
  fprintf(stdout, __PROGRAM_NAME__ " version: " WHITE "%s\n" NO_COLOR,
          __PROGRAM_VERSION__);
}
