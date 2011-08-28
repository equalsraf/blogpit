#include "tests.h"
#include "util.h"

int tst_util_mkdir()
{
	//
	// git_repository *
	// util_create_repo(const char *path)
	//

	git_repository *ret;

	ret = util_create_repo("util/create_repo/repo");
	TEST_ASSERT(ret != NULL, "Unable to create repo");

	// This MUST fail, the repo already exists
	ret = util_create_repo("util/create_repo/repo");
	TEST_ASSERT(ret == NULL, "Create repository twice");

	// This MUST fail, empty string
	ret = util_create_repo("");
	TEST_ASSERT(ret == NULL, "Empty path");

	// This MUST fail, path is null
	// ret = util_create_repo(NULL);
	// TEST_ASSERT(ret == NULL, "Empty path");

	// This MUST fail, .git
	// ret = util_create_repo("util/create_repo/repo2/.git");
	// TEST_ASSERT(ret == NULL, ".git basename");

	return 0;
}

TEST_MAIN(tst_util_mkdir)
