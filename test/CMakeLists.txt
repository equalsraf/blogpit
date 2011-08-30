#
# Tests
#

include_directories(${CMAKE_SOURCE_DIR}/src)
set(test_LIBS z blogpit_static)


#
# Find all tst_*.c files and build one test per source file
#
file(GLOB TESTS tst_*.c)

foreach( SOURCE ${TESTS}  )
	get_filename_component(TESTNAME ${SOURCE} NAME_WE)
	add_executable( ${TESTNAME} ${SOURCE} )
	target_link_libraries(${TESTNAME} ${test_LIBS})
	add_test(${TESTNAME} ${CMAKE_CURRENT_BINARY_DIR}/${TESTNAME})
endforeach( SOURCE ${TESTS} )




