# regexp

Regular expression implementation. \
Supports only ( | ) * + ?.  No escapes. \
Compiles to NFA and then simulates NFA using Thompson's algorithm. \
Caches steps of Thompson's algorithm to build DFA on the fly, as in Aho's egrep.

See also http://swtch.com/~rsc/regexp/ and Thompson, Ken. Regular Expression Search Algorithm, Communications of the ACM 11(6) (June 1968), pp. 419-422.
