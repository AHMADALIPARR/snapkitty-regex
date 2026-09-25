/*
 * snapkitty-regex: a Thompson NFA regular expression engine.
 *
 * Design follows Russ Cox's "Regular Expression Matching Can Be Simple
 * And Fast" (https://swtch.com/~rsc/regexp/regexp1.html), as listed in
 * AHMADALIPARR/build-your-own-x under "Build your own Regex Engine".
 *
 * Pipeline:
 *   infix pattern --re2post--> postfix --post2nfa--> NFA --match--> 0/1
 *
 * The parallel NFA simulation gives O(m*n) worst-case matching time
 * (m = pattern length, n = text length). No backtracking, so there are
 * no pathological inputs: a?^29 a^29 against a^29 matches in
 * microseconds instead of the 60+ seconds Perl needs.
 *
 * Extensions over the article's reference code:
 *   - backslash escapes for metacharacters (\* \+ \? \| \( \) \\)
 *   - no fixed-size static buffers; postfix and paren stack are sized
 *     from the input
 *   - malloc failure and malformed-pattern paths return errors instead
 *     of crashing
 *   - compiled patterns are reusable and releasable (re_free)
 *   - re_search() for unanchored matching
 */

#ifndef SNAPKITTY_REGEX_H
#define SNAPKITTY_REGEX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque compiled pattern. */
typedef struct Regex Regex;

/*
 * Compile a pattern. Returns NULL on malformed pattern or allocation
 * failure. Supported syntax:
 *   literal chars    match themselves
 *   \* \+ \? \| \( \) \\   literal metacharacters
 *   e1 e2            concatenation
 *   e1|e2            alternation
 *   e*               zero or more
 *   e+               one or more
 *   e?               zero or one
 *   (e)              grouping
 */
Regex *re_compile(const char *pattern);

/* 1 if the whole string matches, 0 otherwise. */
int re_fullmatch(Regex *re, const char *s);

/* 1 if the pattern matches a prefix of the string, 0 otherwise. */
int re_match(Regex *re, const char *s);

/* 1 if the pattern matches anywhere in the string, 0 otherwise. */
int re_search(Regex *re, const char *s);

/* Release a compiled pattern. */
void re_free(Regex *re);

#ifdef __cplusplus
}
#endif

#endif /* SNAPKITTY_REGEX_H */
