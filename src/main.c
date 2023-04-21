/*
Copyright (c) 2016 Gustavo Pantuza Coelho Pinto
Copyright (c) 2023 Lai-YT

Can be distributed under the MIT license.
The full text of the license can be found in
https://opensource.org/license/mit/.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "args.h"
#include "colors.h"
#include "regexp.h"
#include "visstate.h"

int main(int argc, char* argv[]) {
  /* Read command line options */
  options_t options;
  options_parser(argc, argv, &options);

#ifdef DEBUG
  fprintf(stdout, CYAN "Command line options:\n" NO_COLOR);
  fprintf(stdout, CYAN "  help: %d\n" NO_COLOR, options.help);
  fprintf(stdout, CYAN "  version: %d\n" NO_COLOR, options.version);
  fprintf(stdout, CYAN "  cache: %d\n" NO_COLOR, options.cache);
  fprintf(stdout, CYAN "  graph: %d\n" NO_COLOR, options.graph);
  fprintf(stdout, CYAN "  filename: %s\n" NO_COLOR, options.filename);
  fprintf(stdout, CYAN "  regexp: %s\n" NO_COLOR, options.regexp);
  fprintf(stdout, CYAN "  string: %s\n" NO_COLOR, options.string);
#endif

  const char* post = re2post(options.regexp);
  if (!post) {
    fprintf(stderr,
            RED "The regexp \"%s\" is ill-formed or too long.\n" NO_COLOR,
            options.regexp);
    exit(EXIT_FAILURE);
  }

  Nfa* nfa = post2nfa(post);
  if (options.graph) {
    char filename[BUF_SIZE + 4];
    snprintf(filename, BUF_SIZE + 4, "%s.dot", options.filename);
    FILE* dotfile = fopen(filename, "w");
    if (!dotfile) {
      fprintf(stderr, RED "Can't open file: \"%s\"\n" NO_COLOR, filename);
      exit(EXIT_FAILURE);
    }
    nfa2dot(nfa, dotfile);
#ifdef DEBUG
    fprintf(stdout, YELLOW "Dot file written to \"%s\"\n" NO_COLOR, filename);
#endif
    fclose(dotfile);
    delete_nfa(nfa);
    return EXIT_SUCCESS;
  }

  bool matches_the_string = options.cache
                                ? is_accepted_with_cache(nfa, options.string)
                                : is_accepted(nfa, options.string);
#ifdef DEBUG
  if (matches_the_string) {
    fprintf(stdout, YELLOW "The regexp matches the string.\n" NO_COLOR);
  } else {
    fprintf(stdout, RED "The regexp doesn't match the string.\n" NO_COLOR);
  }
#endif
  delete_nfa(nfa);
  return matches_the_string ? EXIT_SUCCESS : EXIT_FAILURE;
}
