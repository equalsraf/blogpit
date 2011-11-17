#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <git2.h>

#include "util.h"

#define MAXPATHLEN 4096

int util_mkdir(const char *path)
{
	if ( path == NULL ) {
		errno = EINVAL;
		return -1;
	}

	struct stat buf;
	int err;
	int len = strnlen(path, MAXPATHLEN) + 1;
	char pbuf[len];


	size_t count = strlcpy(pbuf, path, len);
	if ( count >= sizeof(pbuf) ) {
		errno = ENAMETOOLONG;
		return -1;
	}

	char *s;
	for (s=pbuf; *s; s++ ) {
		if ( *s != '/' ) {
			continue;
		}

		*s = '\0';

		err = stat(pbuf, &buf);
		if ( err != 0 && errno == ENOENT ) {
			err = mkdir( pbuf, S_IRUSR | S_IWUSR | S_IXUSR);
			if (err != 0) {
				return -1;
			}
		}

		*s = '/';
	}

	err = stat(pbuf, &buf);
	if ( err != 0 && errno == ENOENT ) {
		err = mkdir( pbuf, S_IRUSR | S_IWUSR | S_IXUSR);
		if (err != 0) {
			return -1;
		}
	}

	return 0;
}

git_repository *
util_create_repo(const char *path)
{
	assert(path);

	if ( util_mkdir(path) == -1 ) {
		return NULL;
	}

	git_repository *repo;
	if ( git_repository_init( &repo, path, 1) != 0) {
		return NULL;
	}

	return repo;
}

char*
util_resolve_name(git_repository *repo, const char *branch)
{
	assert(repo);
	assert(branch);

	git_reference *ref;
	int err = git_reference_lookup(&ref, repo, branch);
	if ( err != 0 ) {
		return NULL;
	}

	const git_oid *oid;
	oid = git_reference_oid(ref);
	if ( oid == NULL ) {
		return NULL;
	}

	char *id = calloc(1, GIT_OID_HEXSZ+1);
	if ( id == NULL ) {
		return NULL;
	}
	git_oid_to_string(id, GIT_OID_HEXSZ+1, oid);
	return id;
}


int
__util_get_object( git_oid *oid_out, git_repository *repo, const git_oid *commit_oid, 
		const char *path, git_otype type)
{
	int rvalue = -1;
	const git_oid *oid;
	git_commit *commit;

	int err = git_commit_lookup(&commit, repo, commit_oid);
	if ( err != 0 ) {
		return -1;
	}

	git_tree *tree=NULL;
	err = git_commit_tree(&tree, commit);
	git_commit_close(commit);

	if ( err != 0 ) {
		return -1;
	}

	int len = strnlen(path, MAXPATHLEN) + 1;
	char pbuf[len];

	size_t count = strlcpy(pbuf, path, len);
	if ( count >= sizeof(pbuf) ) {
		goto free_tree;
	}

	//
	// Split path and find tree entry
	//
	char *sptr;
	char *token;
	const git_tree_entry *tree_entry=NULL;
		
	token = strtok_r( pbuf, "/", &sptr);
	if ( token == NULL ) {
		// Empty path is valid
		oid = git_tree_id(tree);
		git_oid_cpy( oid_out, oid);
		git_tree_close(tree);
		return 0;
	}

	while (1) {
		// Ignore names starting with'.'
		if ( token[0] == '.' ) {
			goto free_tree;
			return -1;
		}

		tree_entry = git_tree_entry_byname(tree, token);
		if ( tree_entry == NULL ) {
			goto free_tree;
		}

		token = strtok_r( NULL, "/", &sptr);
		if ( token == NULL ) {
			break;
		}

		if ( git_tree_entry_type(tree_entry) != GIT_OBJ_TREE ) {
			goto free_tree;
		}
		
		git_tree *old_tree = tree;
		oid = git_tree_entry_id(tree_entry);
		git_tree_close(old_tree);

		if ( git_tree_lookup( &tree, repo, oid) != 0 ) {
			return -1;
		}
	}

	if (tree_entry == NULL) {
		goto free_tree;
	}

	if ( git_tree_entry_type(tree_entry) != type ) {
		goto free_tree;
	}

	oid = git_tree_entry_id(tree_entry);
	if ( oid == NULL ) {
		goto free_tree;
	}
	
	git_oid_cpy( oid_out, oid);

	rvalue = 0;
free_tree:
	git_tree_close(tree);
	return rvalue;
}

int
util_get_object( git_oid *oid_out, git_repository *repo, const char *branch, 
		const char *path, git_otype type)
{
	assert(repo);
	assert(path);
	assert(branch);

	git_reference *ref;
	int err;
	err = git_reference_lookup(&ref, repo, branch);
	if ( err != 0 ) {
		return -1;
	}

	const git_oid *oid;
	oid = git_reference_oid(ref);
	if ( oid == NULL ) {
		return -1;
	}

	return __util_get_object( oid_out, repo, oid, path, type);
}

git_tree*
util_get_tree( git_repository *repo, const char *branch, 
		const char *path)
{
	assert(repo); assert(branch); assert(path);

	git_oid oid;
	if ( util_get_object( &oid, repo, branch, path, GIT_OBJ_TREE) != 0 ) {
		return NULL;
	}

	git_tree *tree;
	if ( git_tree_lookup( &tree, repo, &oid) != 0 ) {
		return NULL;
	}

	return tree;
}

git_blob*
util_get_blob( git_repository *repo, const char *branch, 
		const char *path)
{
	assert(repo); assert(branch); assert(path);

	git_oid oid;
	if ( util_get_object( &oid, repo, branch, path, GIT_OBJ_BLOB) != 0 ) {
		return NULL;
	}

	git_blob *blob;
	if ( git_blob_lookup( &blob, repo, &oid) != 0 ) {
		return NULL;
	}

	return blob;
}

char **
util_get_tree_names(git_repository *repo, const char *branch, const char *path, git_otype type)
{
	assert(repo); assert(branch); assert(path);

	git_tree *tree = util_get_tree( repo, branch, path);
	if ( tree == NULL ) {
		char **single = calloc(sizeof(char*), 1);
		single[0] = NULL;
		return single;
	}

	char **sections = calloc(sizeof(char *), git_tree_entrycount(tree)+1);

	unsigned int i, j;
	for ( i=0, j=0; i<git_tree_entrycount(tree); i++ ) {

		const git_tree_entry *entry = git_tree_entry_byindex(tree, i);

		// Ignore names starting with '.'
		if ( git_tree_entry_name(entry)[0] == '.' ) {
			continue;
		}
		if ( git_tree_entry_type(entry) == type ) {
			//char *s = strdup(git_tree_entry_name(entry));
			const char *name = git_tree_entry_name(entry);
			int slen = strlen(name);
			char *s = malloc(slen+2);

			if (s ) {
				// Prefix with slash if it is a directory
				strlcpy(s, name, slen+1);
				s[slen+1] = '\0';
				s[slen] = (type == GIT_OBJ_TREE) ? '/' : 0;

				sections[j++] = s;
			}
		}
	}

	git_tree_close(tree);

	return sections;
}

/**
 * Returns the next position in the string which is not a path separator
 *
 * @return a char pointer inside the given string or NULL
 */
static const char*
next_path_component(const char *string)
{
	const char *s = string;

	if (!s) {
		return NULL;
	}

	for ( ; *s && (*s==PATH_SEPARATOR); s++ );

	return s;
}

/**
 *
 * Return the length if this path component,
 * i.e. how many chars exist before the next path separator
 *
 */
static ssize_t
path_component_length(const char *string)
{
	const char *s = string;
	if ( !s ) {
		return 0;
	}

	ssize_t len = 0;
	for ( ; *s && (*s != PATH_SEPARATOR) ; s++ ) {
		len++;
	}

	return len;
}

/**
 * @eturn 0 if this path is NOT the root i.e.
 * '/'
 * '//'
 * '//...'
 *
 */
static int
path_is_root(const char *path)
{
	const char *s;
	for ( s=path; *s && *s=='/'; s++ )
		;;

	return ( *s=='\0' );
}

static int
__util_create_file( git_repository *repo, git_tree *tree, const char *path, 
			const char *filename, void *data, size_t datalen, git_oid *oid_out)
{
	int rvalue = -1;
	git_treebuilder *builder;
	git_treebuilder_create(&builder, tree);

	if ( strcmp(path, "") == 0 ) {
		// Create blog a set its entry in the 
		// tree

		const git_tree_entry *entry = git_treebuilder_get(builder, filename);
		if ( entry && git_tree_entry_type(entry) != GIT_OBJ_BLOB ) {
			// Already exists and is not a file :S
			goto free_builder;
		}
		
		git_oid blob_oid;
		if ( git_blob_create_frombuffer(&blob_oid, repo, data, datalen) != 0 ) {
			goto free_builder;
		}

		if ( git_treebuilder_insert( NULL, builder, filename, &blob_oid, 0100644) != 0) {
			goto free_builder;
		}
	} else {
		git_tree *next_tree = NULL;
		const char *s = next_path_component(path);
		ssize_t len = path_component_length(s);

		char comp[len+1];
		strlcpy(comp, s, len+1);

		const git_tree_entry *entry = git_treebuilder_get(builder, comp);
		if ( entry && git_tree_entry_type(entry) != GIT_OBJ_TREE ) {
			// Already exists and is not a tree :S
			return -1;
		}
		if ( entry ) {
			git_object *object;
			if (git_tree_entry_2object(&object, repo, entry) != 0) {
				return -1;
			}
			if ( git_tree_lookup(&next_tree, repo, git_object_id(object)) != 0 ) {
				return -1;
			}
			git_object_close(object);
		}

		git_oid dir_oid;
		int ret = __util_create_file( repo, next_tree, next_path_component(s+len),
						filename, data, datalen, &dir_oid);

		if ( next_tree) {
			git_tree_close(next_tree);
		}

		if ( ret != 0 ) {
			goto free_builder;
		}

		if ( git_treebuilder_insert( NULL, builder, comp, &dir_oid, 0040000) != 0) {
			goto free_builder;
		}
	}

	rvalue = 0;

	git_treebuilder_write(oid_out, repo, builder);
free_builder:
	git_treebuilder_free(builder);

	return rvalue;
}

/**
 *
 * Create a new commit with changing ONLY the given file
 *
 * @param repo
 * @param branch A branch reference to update (ex: refs/heads/master)
 * @param path The path to the file
 * @param filename The file name
 * @param data The data buffer to be saved
 * @param len The length of the buffer
 */
int
util_commit_file(git_repository *repo, const char *branch, const char *path,
		const char *filename, void *data, size_t len, const char *msg)
{
	assert(repo); assert(branch); assert(path);
	assert(filename);

	int rvalue = -1;
	// 
	// Resolve branch name into a commit object
	//
	git_reference *branch_ref;
	git_commit *parent=NULL;

	int err = git_reference_lookup(&branch_ref, repo, branch);
	if (  (err == 0) && ( git_commit_lookup( &parent, repo, git_reference_oid(branch_ref)) != 0 ) ) {
		goto error_lookup;
	}

	git_tree *tree = util_get_tree(repo, branch, "");

	const char *P = path;
	if ( strcmp(P, ".") == 0 || path_is_root(P) ) {
		P = "";
	}

	git_oid oid;
	int ret;
	ret = __util_create_file( repo, tree, P, filename, data, len, &oid);
	if ( ret != 0 ) {
		goto error_commit;
	}
	git_tree_close(tree);

	// 
	// Create commit
	git_oid cid;
	git_signature *author;

	if( git_signature_now( &author, "blogpit", "blog@undisclosed.org") != GIT_SUCCESS) {
		goto error_commit;
	}

	if ( git_tree_lookup( &tree, repo, &oid) != 0 ) {
		goto error_sig;
	}

	const char *M = "Blogpit update";
	if ( msg != NULL ) {
		M = msg;
	}

	int parent_count = parent ? 1 : 0 ;

	if ( git_commit_create_v(&cid, repo, NULL, author, author, "utf-8", M, tree, parent_count, parent) != 0 ) {
		goto error_tree;
	}

	git_reference *ref;
	if (git_reference_create_oid( &ref, repo, branch, &cid, 1) != 0 ) {
		goto error_tree;
	}

	rvalue = 0;
error_tree:
	git_tree_close(tree);

error_sig:
	git_signature_free(author);

error_commit:
	if ( parent ) {
		git_commit_close(parent);
	}
error_lookup:
	return rvalue;
}

