/*
 * ============================================================================
 *
 *       Filename:  args.h
 *
 *    Description:  Header file of the command line options parser
 *
 *        Created:  24/03/2016 21:30:39 PM
 *       Compiler:  gcc
 *
 *         Author:  Gustavo Pantuza
 *   Organization:  Software Community
 *
 * ============================================================================
 */

#ifndef ARGS_H
#define ARGS_H

#include <stdbool.h>

#define BUF_SIZE 100

/* Defines the command line allowed options struct */
struct options {
  bool help;
  bool version;
  bool graph;
  char filename[BUF_SIZE];
  char regexp[BUF_SIZE];
  char string[BUF_SIZE];
};

/* Exports options as a global type */
typedef struct options options_t;

/* Public functions section */
void options_parser(int argc, char* argv[], options_t* options);

#endif  // ARGS_H
