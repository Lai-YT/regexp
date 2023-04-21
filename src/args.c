/*
Copyright (c) 2016 Gustavo Pantuza Coelho Pinto
Copyright (c) 2023 Lai-YT

Can be distributed under the MIT license.
The full text of the license can be found in
https://opensource.org/license/mit/.
 */

#include "args.h"

#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "colors.h"
#include "messages.h"

/*
 * Sets the default options
 */
static void set_default_options(Options* options) {
  options->help = false;
  options->version = false;
  options->cache = false;
  options->graph = false;
  strncpy(options->filename, "nfa", BUF_SIZE);
  options->regexp[0] = '\0';
  options->string[0] = '\0';
}

/*
 * Finds the matching case of the current command line option
 */
void switch_options(int arg, Options* options) {
  switch (arg) {
    case 'h':
      options->help = true;
      help();
      exit(EXIT_SUCCESS);

    case 'V':
      options->version = true;
      version();
      exit(EXIT_SUCCESS);

    case 'c':
      options->cache = true;
      break;

    case 'g':
      options->graph = true;
      break;

    case 'o':
      if (!options->graph) {
        fprintf(stderr,
                "option --output has to be used together with --graph\n");
        usage();
        exit(EXIT_FAILURE);
      }
      strncpy(options->filename, optarg, BUF_SIZE);
      break;

    case '?':
      usage();
      exit(EXIT_FAILURE);

    default:
      usage();
      abort();
  }
}

void get_regexp(int argc, char* argv[], Options* options) {
  if (optind < argc) {
    strncpy(options->regexp, argv[optind++], BUF_SIZE);
  } else {
    usage();
    exit(EXIT_FAILURE);
  }
}

void get_string(int argc, char* argv[], Options* options) {
  if (optind < argc) {
    strncpy(options->string, argv[optind++], BUF_SIZE);
  } else {
    usage();
    exit(EXIT_FAILURE);
  }
}

/*
 * Public function that loops until command line options were parsed
 */
void options_parser(int argc, char* argv[], Options* options) {
  set_default_options(options);

  int arg; /* Current option */

  /* getopt allowed options */
  static struct option long_options[] = {
      {"help", no_argument, 0, 'h'},         {"version", no_argument, 0, 'V'},
      {"cache", no_argument, 0, 'c'},        {"graph", no_argument, 0, 'g'},
      {"output", required_argument, 0, 'o'}, {0, 0, 0, 0},
  };

  while (true) {
    int option_index = 0;
    arg = getopt_long(argc, argv, "hVcgo:", long_options, &option_index);

    /* End of the options? */
    if (arg == -1) {
      break;
    }
    /* Find the matching case of the argument */
    switch_options(arg, options);
  }

  /* Both graph and match mode take a regexp */
  get_regexp(argc, argv, options);

  if (!options->graph) {
    get_string(argc, argv, options);
  }
  if (optind < argc) {
    fprintf(stderr, "%s: unknown arguments\n", argv[0]);
    usage();
    exit(EXIT_FAILURE);
  }
}
