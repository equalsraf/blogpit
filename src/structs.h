#ifndef __BLOGPIT_STRUCTS__
#define __BLOGPIT_STRUCTS__

#include <git2.h>

#define MAX_BRANCHNAME_LENGTH 255


struct blogpit
{
	git_repository *repo;
	char *branch;
	const char *workdir;
};

#endif
