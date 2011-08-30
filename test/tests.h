
//
// The lazyest unit test library of them all
// 
//

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define TEST_MAIN(x) int main(void){return x();}

#define TEST_ASSERT(cond, msg) if ( !(cond) ) { printf(msg); return -1; }

