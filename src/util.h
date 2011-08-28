/**
 * @file util.h
 *
 * Some utility functions functions to handle path resolution
 * git operations and string copy.
 *
 */

#ifndef __BLOGPIT_UTIL__
#define __BLOGPIT_UTIL__

#include <git2.h>

#define PATH_SEPARATOR '/'

/**
 * Create all elements in a path
 *
 * @return 0 on success
 */
int util_mkdir(const char *path);

/**
 *
 * Create a new BARE git repository in the given path
 *
 * The repository will be a bare repository stored right
 * at the given path (i.e.no .git directory).
 *
 * @return NULL on failure or a new repo on success
 * @warning If the repository already exists this will fail
 *
 */
git_repository* util_create_repo(const char *path);

/**
 * Get the oid of the given path element/type
 *
 *
 * @param oid_out A pointer to a git_oid where the result will be stored
 * @param repo A git repository object
 * @param branch A branch reference such as refs/heads/master
 * @param path A path to an object within the repo
 * @param type Specifies the type of the object
 *
 * @return 0 on success
 */
int util_get_object(git_oid *oid_out, git_repository *repo, const char *branch, 
		const char *path, git_otype type);

/**
 *
 * Return the tree object for the given path, if it exists
 *
 * @return A git tree or NULL on failure
 * @warning the return reference must be released using git_tree_close()
 * @see util_get_object
 */
git_blob* util_get_blob( git_repository *repo, const char *branch, 
		const char *path);

/**
 *
 * Return the blob object for the given path, if it exists
 *
 * @return A git blob or NULL on failure
 * @warning the return tree must be released using git_blob_close()
 * @see util_get_object
 */
git_tree* util_get_tree( git_repository *repo, const char *branch, 
		const char *path);

/**
 *
 * Git the Git commit id of the last commit in the given branch
 *
 * @return an allocated string with the Object Id or NULL
 */
char* util_resolve_name(git_repository *repo, const char *branch);

/**
 * For the given path return all object names of the given type
 *
 * @return a list of strings, terminated by a NULL pointer
 */
char **util_get_tree_names(git_repository *repo, const char *branch, const char *path, git_otype type);

/**
 *
 * Create a file a associate trees in a git repo
 *
 */
int util_commit_file(git_repository *repo, const char *branch, const char *path,
				const char *filename, void *data, size_t len, const char *msg);


/**
 * strl* functions imported from OpenBSD
 */
size_t strlcat(char *dst, const char *src, size_t siz);
/**
 * strl* functions imported from OpenBSD
 */
size_t strlcpy(char *dst, const char *src, size_t siz);

#endif
