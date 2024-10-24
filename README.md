<h1 align="center"><i>regexp</i></h1>

<div align="center">

  [![CI](https://github.com/Lai-YT/regexp/actions/workflows/build.yml/badge.svg)](https://github.com/Lai-YT/regexp/actions/workflows/build.yml)
  [![Status](https://img.shields.io/badge/status-active-success.svg)]()
  [![GitHub Issues](https://img.shields.io/github/issues/lai-yt/regexp.svg)](https://github.com/lai-yt/regexp/issues)
  [![GitHub Pull Requests](https://img.shields.io/github/issues-pr/lai-yt/regexp.svg)](https://github.com/lai-yt/regexp/pulls)
  [![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

</div>

---

<p align="center">
  Regular expression implementation.
  <br>
  Supports . ( ) | * + ?. No escapes.
</p>

## 📝 Table of Contents
- [About](#about)
- [Getting Started](#getting_started)
- [Usage](#usage)
- [Development](#development)
- [Acknowledgments](#acknowledgement)
- [License](#license)

## 🧐 About <a name = "about"></a>
_regexp_ is a side project I wrote when learning _Compilers_. It's a command line tool for simplified [regular expression](https://en.wikipedia.org/wiki/Regular_expression) matching which converts the regular expression into an [NFA](https://en.wikipedia.org/wiki/Nondeterministic_finite_automaton) and simulates the NFA using [Thompson's algorithm](https://en.wikipedia.org/wiki/Thompson%27s_construction).

## 🏁 Getting Started <a name = "getting_started"></a>
To try out _regexp_, clone (or download) this repo and compile the source locally.

### Prerequisites
_regexp_ uses [GNU Make](https://www.gnu.org/software/make/) to control the generation of executable and [gcc](https://gcc.gnu.org/) as the C compiler.

### Installing
```shell
# Get the repo
$ git clone https://github.com/Lai-YT/regexp.git

# Compile to executable with optimizations
$ make release
```

The executable will be located in the `bin/` folder and named `regexp`.

## 🔧 Running the tests <a name = "tests"></a>
_regexp_ uses [cmocka](https://cmocka.org/) for unit-testing and [Valgrind](https://valgrind.org/) for detecting memory management bugs.

- Run the unit tests
```console
$ make tests
```
- Check for memory-related bugs
```shell
$ make valgrind
```
- Run the command line tests
```console
# Give permission on execution
$ chmod +x cli_test.sh

# Run CLI tests on release (or debug) build
$ ./cli_test.sh --release # --debug
```

## 🎈 Usage <a name="usage"></a>
Get the help message,
```console
$ bin/regexp -h # or --help
```

```
regexp

Usage: regexp [-h] [-V] {-g regexp [-o FILE] | [-c] regexp string}

Description: Regular expression implementation.
Supports . ( ) | * + ?. No escapes.
Compiles to NFA and then simulates NFA using Thompson's algorithm.

One can either match a string (default) or graph the regexp.
See the following options.
Notice that character # can't appear in the regular expression,
it's reserved technically as the special character.

Options:
  -h, --help            Shows this help message and exit
  -V, --version         Shows regexp version and exit

Match mode:
  Matches the string with the regular expression,
  exits with 1 if regexp is ill-formed or it does not match

  -c, --cache           Caches NFA states to build DFA on the fly
  regexp                The regular expression to use on matching
  string                The string to be matched

Graph mode:
  Converts the regular expression into a graph,
  exits with 1 if regexp is ill-formed or the file can't be opened

  -g, --graph           Converts the NFA of the regexp into a Graphviz
                        dot file (default: False)
  -o FILE, --output FILE
                        The name of the dot file.
                        A .dot extension is appended automatically
                        (default: nfa)
  regexp                The regular expression to be converted

Written by: Lai-YT

regexp version: 1.0.2
```

### Example

#### Match mode
This is the default mode. \
_regexp_ takes two arguments: a regular expression and a string to match.
```console
$ bin/regexp '(a|b)*abb' 'bababb'
```
This exits with 0 if the string is matched by the regular expression or 1 if the regular expression is ill-formed or too long.

You can check the exit code with the following command if you're on an Unix shell.
```console
$ echo $?
```

#### Caching the NFA to build a DFA on the fly
"In a sense, Thompson's NFA simulation is executing the equivalent DFA by reconstructing each DFA state as it is needed. Rather than throw away this work after each step, we could cache them, avoiding the cost of repeating the computation in the future and essentially computing the equivalent DFA as it is needed." (Russ Cox, see [Acknowledgments](#acknowledgement))

_regexp_ doesn't cache the states by default. \
Set the `--cache` (or `-c`) option to enable the caching.
```console
$ bin/regexp -c '(a|b)*abb' 'bababb'
```

#### Graph mode
_regexp_ uses [Graphviz](https://graphviz.org/) to graph the NFA of a regular expression. It represents the NFA with the [DOT language](https://graphviz.org/doc/info/lang.html).

By setting the `--graph` (or `-g`) option, you can use the graph mode. This takes a single regular expression as an argument.

- Convert the NFA of a regular expression into a DOT file,
```console
$ bin/regexp --graph '(a|b)*abb'
```
By default, the DOT file is written into `nfa.dot`.

- You can also specify the output file with the `--output` (or `-o`) option, which takes a file as an argument.
```console
$ bin/regexp --graph '(a|b)*abb' --output 'some/path/and/filename'
```
This will then write the graph into `some/path/and/filename.dot`.
> [!note]
> A `.dot` extension is always append to the you given.

- After the DOT file is generated, you can convert it into a PNG image with Graphviz.
```console
$ dot -Tpng nfa.dot -o nfa.png
```
![The NFA of "(a|b)*abb"](https://imgur.com/aVNvEoK.png)
> [!note]
> The numbering of the states is related to the order of their creations.

See the [command line documentation of Graphviz](https://graphviz.org/doc/info/command.html) to learn more.

## 🚀 Development <a name = "development"></a>
Run _regex_ with debug messages and less optimizations (`-O0`).
```console
$ make debug
```

There are several other *make targets* to use.
```console
$ make help
```

```
Target rules:
    debug    - Compiles and generates binary file with
               debug messages and less optimizations
    release  - Compiles and generates optimized binary file
    tests    - Compiles with cmocka and runs test binary file
    valgrind - Runs test binary file using valgrind tool
    fmt      - Formats the source and test files
    tidy     - Checks naming conventions and bug-proneness
    clean    - Cleans the project by removing binaries
    help     - Prints a help message with target rules
```

### Implementation
_regex_ matches strings with regular expressions in 3 steps:
1. The regular expression is converted into a parenthesis-free postfix notation using the `#` operator to make concatenations explicit. This is implemented in [re2post.c](src/re2post.c).
2. The postfixed regular expression is converted into a Nondeterministic Finite Automaton (NFA) using Thompson's algorithm. This step is implemented in [post2nfa.c](src/post2nfa.c).
3. Reads in the input string character by character and walks along the NFA. If it stops at the accepting state when the entire string has been read, the string is considered a match. This step is implemented in [regexp.c](src/regexp.c).

By breaking down the process into these 3 steps, _regexp_ is able to efficiently match strings with regular expressions.

### Codebase structure
```
regexp
│   Makefile
│   README.md
│   ...
└───bin
│     regexp
│     regexp_test_runner
└───src
│     main.c
│     regexp.h
│     regexp.c
│     ...
└───test
│     main.c
│     regexp.h
│     regexp.c
│     ...
└───lib
│     main.o
│     regexp.o
│     ...
└───log
      valgrind.log
```

- `bin/`: executables
- `src/`: source files
- `test/`: test files
- `lib/`: object files
- `log/`: output message of Valgrind
> [!note]
> There aren't any prefix or postfix on the filename of test files.

### Code formatting
_regexp_ uses [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) as formatting tool.

Format source and test codes,
```console
$ make fmt
```

### Naming conventions & bug-proneness checking
_regexp_ uses [Clang-Tidy](https://clang.llvm.org/extra/clang-tidy/) as the linter, which checks naming conventions and several bug-proneness.

Check naming conventions and bug-proneness,
```console
$ make tidy
```

## 🎉 Acknowledgements <a name = "acknowledgement"></a>

I would like to express my gratitude to Russ Cox for his inspiring work, which motivated me to embark on this project. His webpage on regular expression matching has been a valuable reference, demonstrating that simplicity can indeed be fast. \
I would also like to thank Alfred Aho at el. for their excellent book on compilers, which has guided me through the implementation of the algorithms used in this project. Without their clear and insightful explanations, this project would not have been possible.
- Cox, Russ. "Regular Expression Matching Can Be Simple And Fast," https://swtch.com/~rsc/regexp/regexp1.html, 2007
- Aho, Alfred V., Sethi, Ravi, Ullman, Jeffrey D. "Compilers: Principles, Techniques, and Tools," 2nd ed., Addison-Wesley, 2006, pp. 147-166

## ✍️ License <a name = "license"></a>

This project is licensed under the [MIT License](LICENSE). However, if any individual file within the project includes its own copyright claim, that claim should be respected and take precedence over the project-wide license.
> [!note]
> They are still guaranteed to be licensed under the MIT License but with different authors.
