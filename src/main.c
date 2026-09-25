/* snapkitty-regex CLI: match patterns against strings. */
#include "regex.h"

#include <stdio.h>
#include <string.h>

static void
usage(const char *prog)
{
	fprintf(stderr, "usage: %s [-s] <pattern> <string>...\n", prog);
	fprintf(stderr, "  -s  search (unanchored) instead of full match\n");
}

int
main(int argc, char **argv)
{
	int search = 0;
	int ai = 1;
	int allmatch = 1;
	Regex *re;

	if (ai < argc && strcmp(argv[ai], "-s") == 0) {
		search = 1;
		ai++;
	}
	if (argc - ai < 2) {
		usage(argv[0]);
		return 2;
	}
	re = re_compile(argv[ai++]);
	if (re == NULL) {
		fprintf(stderr, "error: bad pattern\n");
		return 2;
	}
	for (; ai < argc; ai++) {
		int m = search ? re_search(re, argv[ai])
			       : re_fullmatch(re, argv[ai]);
		printf("%s: %s\n", argv[ai], m ? "MATCH" : "NO MATCH");
		if (!m)
			allmatch = 0;
	}
	re_free(re);
	return allmatch ? 0 : 1;
}
