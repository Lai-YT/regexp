/*
 * ============================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  Main file of the project
 *
 *        Created:  03/24/2016 19:40:56
 *
 *         Author:  Gustavo Pantuza
 *   Organization:  Software Community
 *
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include "args.h"
#include "colors.h"
#include "regexp.h"

int main(int argc, char* argv[]) {
  /* Read command line options */
  options_t options;
  options_parser(argc, argv, &options);

#ifdef DEBUG
  fprintf(stdout, BLUE "Command line options:\n" NO_COLOR);
  fprintf(stdout, YELLOW "help: %d\n" NO_COLOR, options.help);
  fprintf(stdout, YELLOW "version: %d\n" NO_COLOR, options.version);
  fprintf(stdout, YELLOW "use colors: %d\n" NO_COLOR, options.use_colors);
  fprintf(stdout, YELLOW "regexp: %s\n" NO_COLOR, options.regexp);
  fprintf(stdout, YELLOW "string: %s\n" NO_COLOR, options.string);
#endif

  if (regexp(options.regexp, options.string)) {
    return 0;
  }
  return 1;
}
