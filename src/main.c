#include <stdio.h>
#include <blogpit.h>
#include <string.h>
#include <stdlib.h>

int main(int ac, char **av)
{

	if ( ac < 3 ) {
		fprintf(stderr, "Usage: blogpitc <PATH> <COMMAND> [ARGS]\n");
		return -1;
	}

	struct blogpit *B;
	B = blogpit_open(av[1], "refs/heads/master");
	if (!B) {
		fprintf(stderr, "blogpit_open(): Unable to open repository\n");
		return -1;
	}

	if ( strcmp(av[2], "version") == 0 ) {
		char *v = blogpit_version(B);
		printf("%s\n", v);
		free(v);
	}

	blogpit_close(B);
	return 0;
}
