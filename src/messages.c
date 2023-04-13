/*
Copyright (c) 2016 Gustavo Pantuza Coelho Pinto
Copyright (c) 2023 Lai-YT

Can be distributed under the MIT license.
The full text of the license can be found in
https://opensource.org/license/mit/.
 */

#include "messages.h"

#include <stdio.h>

#include "colors.h"

/*
 * Help message
 */
void help() {
  fprintf(stdout, CYAN __PROGRAM_NAME__ "\n\n" NO_COLOR);
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
  fprintf(stdout, "%s [-h] [-V] [-d regexp [-o FILE]] [[-c] regexp string]\n\n",
          __PROGRAM_NAME__);
}

/*
 * Description message
 */
void description() {
  fprintf(stdout, YELLOW "Description: " NO_COLOR);
  fprintf(stdout,
          "Regular expression implementation.\n"
          "Supports . ( ) | * + ?. No escapes.\n"
          "Compiles to NFA and then simulates NFA using Thompson's algorithm.\n"
          "\n"
          "One can either graph the regexp or match a string.\n"
          "See the following options.\n"
          "Notice that character # can't appear in the regular expression,\n"
          "it's reserved technically as the special character.\n"
          "\n");
}

void graph_mode() {
  fprintf(
      stdout, WHITE
      "Graph mode:\n"
      "  Converts the regular expression into a graph,\n"
      "  exits with 1 if regexp is ill-formed or the file can't be opened\n"
      "\n"
      "  -d, --graph           Converts the NFA of the regexp into a Graphviz\n"
      "                        dot file (default: False)\n"
      "  -o FILE, --output FILE\n"
      "                        The name of the dot file.\n"
      "                        A .dot extension is appended automatically\n"
      "                        (default: nfa)\n"
      "  regexp                The regular expression to be converted\n"
      "\n" NO_COLOR);
}

void match_mode() {
  fprintf(stdout, WHITE
          "Match mode:\n"
          "  Matches the string with the regular expression,\n"
          "  exits with 1 if regexp is ill-formed or it does not match\n"
          "\n"
          "  -c, --cache           Caches NFA states to build DFA on the fly\n"
          "  regexp                The regular expression to use on matching\n"
          "  string                The string to be matched\n"
          "\n" NO_COLOR);
}

/*
 * Options message
 */
void options() {
  fprintf(stdout,
          WHITE
          "Options:\n"
          "  -h, --help            Shows this help message and exit\n"
          "  -V, --version         Shows %s version and exit\n"
          "\n" NO_COLOR,
          __PROGRAM_NAME__);
  graph_mode();
  match_mode();
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
