#include "tests.h"
#include <errno.h>
#include "util.h"

#include <stdlib.h>

int tst_util_mkdir()
{
	//
	// int util_mkdir(const char *path);
	//

	int ret;

	// create directory
	ret = util_mkdir("util/mkdir/new\n");
	TEST_ASSERT(ret == 0, "Unable to create directory\n");

	// This should not fail, as long as the dir exists
	ret = util_mkdir("util/mkdir/new");
	TEST_ASSERT(ret == 0, "Failed when creating already existing directory\n");

	// This should fail - NULL string
	ret = util_mkdir(0);
	TEST_ASSERT(ret != 0 && errno == EINVAL, "Accepted NULL argument\n");

	//
	// util_mkdir wont take a string with more than 4096 chars
	// This must fail, with errno as ENAMETOOLONG
	//
	char s[5000] = {64};
	memset(s, 64, 5000);
	s[4999] = 0;
	ret = util_mkdir(s);
	TEST_ASSERT(ret != 0 && errno == ENAMETOOLONG, "Path too long");
	
	//
	// This should work - 4096 bytes string
	//
	s[4096] = 0;
	ret = util_mkdir(s);
	TEST_ASSERT(ret == 0, "Wont take a 4096 byte string");


	return 0;
}

TEST_MAIN(tst_util_mkdir)
