/*
Copyright (c) 2016 Gustavo Pantuza Coelho Pinto
Copyright (c) 2023 Lai-YT

Can be distributed under the MIT license.
The full text of the license can be found in
https://opensource.org/license/mit/.
 */

#ifndef ARGS_H
#define ARGS_H

#include <stdbool.h>

#define BUF_SIZE 100

/* Defines the command line allowed options struct */
struct options {
  bool help;
  bool version;
  bool cache;
  bool graph;
  char filename[BUF_SIZE];
  char regexp[BUF_SIZE];
  char string[BUF_SIZE];
};

/* Exports options as a global type */
typedef struct options Options;

/* Public functions section */
void options_parser(int argc, char* argv[], Options* options);

#endif  // ARGS_H
