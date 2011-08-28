/**
 * @file blogpit.h
 *
 *
 */
#ifndef __BLOGPIT__
#define __BLOGPIT__

struct blogpit;

/**
 *
 * Open a repository
 *
 * @param path The path to a git repository - including the last component(.git)
 * @param branch A branch reference such as refs/heads/master
 * @warning If the path does not exist it will be created
 *
 * @return A Blogpit handle or NULL if an error occurred
 */
struct blogpit* blogpit_open(const char *path, const char *branch);

/**
 *
 * Close a blogpit and release all associated resources
 *
 * @param b A blogpit handle to be released
 */
void blogpit_close(struct blogpit *b);

/**
 *
 * Return the current version of the blogpit, i.e. an unique string
 * that identifies the current version of the blogpit. This can be used
 * as caching key.
 *
 * @param b A blogpit handle
 * @return An allocaed string that identifies the current version
 *
 * @warning If not version is available then an empty string is returned
 * @warning You must free() the returned string
 *
 */
char* blogpit_version(struct blogpit *b);

/**
 * 
 * Get the content of an article
 *
 * @param b A blogpit handle
 * @param path A path for an article within the blogpit
 * @param buflen The length of the returned buffer
 * @return An allocated buffer. its size is stored in buflen
 *
 * @warning You must free the returned buffer after use
 */
void* blogpit_getarticle(const struct blogpit *b, const char *path, size_t *buflen);

/**
 *
 * Set article content
 *
 * @param b
 * @param path The article path
 * @param data A buffer of data to be stored
 * @param len The size of the buffer
 */
int blogpit_setarticle(struct blogpit *b, const char *path, void *data, size_t len, const char *msg);

/**
 * Get the names of all sections at the given path
 *
 * @param b A blogpit handle
 * @param path A path for a section within the blogpit
 * @return an array of strings terminated by a NULL entry
 * @warning all the returned space must be free()ed by the caller
 */
char** blogpit_sections(struct blogpit *b, const char *path);

/**
 * Get the names of all articles at the given path
 *
 * @param b A blogpit handle
 * @param path A path for a section within the blogpit
 * @return an array of strings terminated by a NULL entry
 * @warning all the returned space must be free()ed by the caller
 */
char** blogpit_articles(struct blogpit *b, const char *path);


#endif
