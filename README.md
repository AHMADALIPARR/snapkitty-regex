# snapkitty-regex

A Thompson NFA regular expression engine in C — under 700 lines, zero
dependencies, no exponential backtracking, ever.

Built as an exercise from the guides in
[AHMADALIPARR/build-your-own-x](https://github.com/AHMADALIPARR/build-your-own-x)
("Build your own `Regex Engine`"), primarily Russ Cox's
*[Regular Expression Matching Can Be Simple And Fast](https://swtch.com/~rsc/regexp/regexp1.html)*.

## The idea

Most popular regex engines (Perl, Python, Ruby, Java, PCRE) simulate an
NFA by *backtracking*: guess a path, and retry on failure. On patterns
like `a?a?a?aaa` that means trying all 2^n combinations — Perl needs
**60+ seconds** to match a 29-character string.

This engine instead simulates *all* NFA paths in parallel, tracking the
set of reachable states one input character at a time (Thompson's 1968
algorithm). Matching is **O(m·n)** worst case for pattern length m and
text length n. There are no pathological inputs.

```
infix pattern --re2post--> postfix --post2nfa--> NFA --pump--> match?
                  (explicit '.' concat)   (Thompson construction)
```

## Syntax

| construct | meaning |
|---|---|
| `abc` | literal characters match themselves |
| `e1 e2` | concatenation |
| `e1\|e2` | alternation |
| `e*` | zero or more |
| `e+` | one or more |
| `e?` | zero or one |
| `(e)` | grouping |
| `\` + metachar | literal metacharacter: `\* \+ \? \| \( \) \\` |
| `.` | literal dot (no wildcard — see roadmap) |

Precedence: `|` weakest, then concatenation, then `* + ?`. Matching is
byte-oriented.

## Build & test

```sh
make          # builds ./regex, clean compile, zero warnings
make test     # 82-check correctness harness (fullmatch, search, bad patterns)
make bench    # pathological benchmark vs Python's re
```

## Usage

```sh
$ ./regex 'a(bb)+a' abbbba abba abbba
abbbba: MATCH
abba: MATCH
abbba: NO MATCH

$ ./regex -s 'colou?r' 'the colour blue'
the colour blue: MATCH
```

Exit codes: `0` all matched, `1` some didn't, `2` usage/bad pattern.

As a library (`src/regex.h`):

```c
Regex *re = re_compile("(ab|cd)+e");
int m = re_fullmatch(re, "cdabe");  /* whole string */
int p = re_match(re, "cdabex");     /* prefix of string */
int s = re_search(re, "xxcdabexx"); /* anywhere in string */
re_free(re);
```

## Benchmark

`a?`^n`a`^n against `a`^n — the article's pathological case:

| n | snapkitty-regex | python `re` |
|---|---|---|
| 10 | 0.0011s | 0.0000s |
| 20 | 0.0010s | 0.0442s |
| 24 | 0.0013s | 0.7139s |
| 26 | 0.0014s | 3.0580s |
| 500 | 0.0021s | (heat death of universe) |

(The ~1ms floor on our side is process-spawn overhead, not matching.)

## What's hardened vs the article's reference code

- **No fixed static buffers** — postfix output and the paren stack are
  sized from the input instead of `char buf[8000]` / `paren[100]`.
- **Backslash escapes** for metacharacters.
- **Literal dots** — a `.` in the pattern can't collide with the postfix
  concatenation operator anymore (this crashes the original).
- **Real error paths** — malformed patterns and `malloc` failures return
  `NULL` instead of crashing; partial allocations unwind cleanly.
- **Reusable, releasable** — compiled patterns are opaque `Regex*`
  values with `re_free`; the accept state and dedup generation are
  per-pattern, not globals.
- **Prefix match + search** on top of full match.

## Roadmap

- `.` wildcard / character classes `[a-z]`
- Capture groups and submatch extraction
- DFA / hybrid execution for very large inputs
- Streaming matcher API
