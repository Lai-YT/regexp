<h3 align="center"><i>regexp</i></h3>

<div align="center">

  [![Status](https://img.shields.io/badge/status-active-success.svg)]()
  [![GitHub Issues](https://img.shields.io/github/issues/lai-yt/regexp.svg)](https://github.com/lai-yt/regexp/issues)
  [![GitHub Pull Requests](https://img.shields.io/github/issues-pr/lai-yt/regexp.svg)](https://github.com/lai-yt/regexp/pulls)
  [![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

</div>

---

<p align="center">
  Regular expression implementation.
  <br>
  Supports only ( | ) * + ?. No escapes.
</p>

## 📝 Table of Contents
- [About](#about)
- [Getting Started](#getting_started)
- [Usage](#usage)
- [Development](#development)
- [Acknowledgments](#acknowledgement)

## 🧐 About <a name = "about"></a>
_regexp_ is a side project I wrote when learning _Compilers_. It's a simplified regular expression matching command line tool which converts the regular expression into an [NFA](https://en.wikipedia.org/wiki/Nondeterministic_finite_automaton) and then simulates the NFA using [Thompson's algorithm](https://en.wikipedia.org/wiki/Thompson%27s_construction).

## 🏁 Getting Started <a name = "getting_started"></a>
To try out _regexp_, clone (or download) this repo and compile the source locally.

### Prerequisites
We use [GNU Make](https://www.gnu.org/software/make/) to control the generation of executable.

### Installing
```shell
# Get the repo
$ git clone https://github.com/Lai-YT/regexp.git

# Compile to executable
$ make
```

The executable will locate in the `bin/` folder, named as `regexp`.

## 🔧 Running the tests <a name = "tests"></a>
_regexp_ uses [cmocka](https://cmocka.org/) for unit-testing and [Valgrind](https://valgrind.org/) for detecting memory management bugs.

- Run the unit tests
```shell
$ make tests
```
- Check for memory-related bugs
```shell
$ make valgrind
```

## 🎈 Usage <a name="usage"></a>
Get the help message,
```shell
$ bin/regexp -h # or --help
```

```
regexp

Usage: regexp [options] [regexp string]

Description: Regular expression implementation.
Supports only ( | ) * + ?. No escapes.
Compiles to NFA and then simulates NFA using Thompson's algorithm.

Matches the string (string) to the regular expression (regexp) and exits with 1 if it does not match.

Options:

        -v, --version
                Prints regexp version

        -h, --help
                Prints this help message

Written by: Lai-YT

regexp version: 0.0.1
```

### Example

_regexp_ takes two arguments: a regular expression and a string to match.
```shell
$ bin/regexp '(a|b)*abb' 'bababb'
```

## 🚀 Development <a name = "development"></a>
There are several *make targets* to use.
```shell
$ make help
```

```
Target rules:
    all      - Compiles and generates binary file
    tests    - Compiles with cmocka and runs test binary file
    valgrind - Runs test binary file using valgrind tool
    fmt      - Formats the source and test files
    clean    - Cleans the project by removing binaries
    help     - Prints a help message with target rules
```

### Code formatting
_regexp_ uses [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) as formatting tool.

Format source and test codes,
```shell
$ make fmt
```

## 🎉 Acknowledgements <a name = "acknowledgement"></a>
- http://swtch.com/~rsc/regexp/
- Thompson, Ken. Regular Expression Search Algorithm, Communications of the ACM 11(6) (June 1968), pp. 419-422.
