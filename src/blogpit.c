#include <git2.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/param.h>
#include <libgen.h>

#include <stdio.h>

/*
 * Blogpit is a set of simple functions to access a dictionary
 *
 *
 * - open(path)
 * - close()
 * - version()
 * - sections(path)
 * - get_article(path)
 * - articles(path)
 * - set_article(path, content)
 *
 * Dictionary keys are a 2 level structure "section1/section2/articles"
 * i.e. sections are folders and articles are files
 */

#include "util.h"
#include "structs.h"

struct blogpit*
blogpit_open(const char *path, const char *name)
{
	assert(path); assert(name);

	git_repository *repo;

	int len = strnlen(name, MAX_BRANCHNAME_LENGTH);
	if ( len == MAX_BRANCHNAME_LENGTH ) {
		return NULL;
	}

	char *s = calloc(1,len+1);
	strlcpy(s, name, len+1);

	struct blogpit *b = calloc(1, sizeof(struct blogpit));
	if ( b == NULL ) {
		return NULL;
	}

	int err = git_repository_open(&repo, path);
	if ( err != 0 ) {
		repo = util_create_repo( path );
		if ( repo == NULL ) {
			goto fail_repo;
		}
	}

	b->repo = repo;
	b->branch = s;
	return b;

fail_repo:
	free(s);
	free(b);
	return NULL;
}


void
blogpit_close(struct blogpit *b)
{
	if (!b) {
		return;
	}

	git_repository_free(b->repo);
	free(b->branch);
	free(b);
}

char *
blogpit_version(struct blogpit *b)
{
	assert(b);
	char *s = util_resolve_name(b->repo, b->branch);

	if ( s == NULL ) {
		s = malloc(1);
		s[0] = '\0';
	}

	return s;
}

void *
blogpit_getarticle(struct blogpit *b, const char *path, size_t *buflen)
{
	assert(b); assert(path); assert(buflen);

	size_t len;
	git_blob *blob = util_get_blob( b->repo, b->branch, path);
	if ( blob == NULL ) {
		return NULL;
	}


	len = git_blob_rawsize(blob);
	void *data = calloc( 1, len );
	if ( data == NULL ) {
		return NULL;
	}

	memcpy( data, git_blob_rawcontent(blob), len);
	*buflen = len;

	git_blob_close(blob);
	return data;
}

int
blogpit_setarticle(struct blogpit *b, const char *path, void *data, size_t len, const char *msg)
{
	int rvalue = -1;
	char *cp1 = strdup(path);
	char *cp2 = strdup(path);

	char *filename = basename(cp1);
	char *dir = dirname(cp2);

	if ( strcmp(filename, "") == 0 ) {
		goto empty_filename;
	}

	rvalue = util_commit_file(b->repo, b->branch, 
					dir, filename, data, len, msg);

empty_filename:
	free(cp1);
	free(cp2);

	return rvalue;
}

char**
blogpit_sections(struct blogpit *b, const char *path)
{
	assert(b); assert(path);
	return util_get_tree_names( b->repo, b->branch, path, GIT_OBJ_TREE);
}

char**
blogpit_articles(struct blogpit *b, const char *path)
{
	assert(b); assert(path);
	return util_get_tree_names( b->repo, b->branch, path, GIT_OBJ_BLOB);
}



