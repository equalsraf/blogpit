
IF(NOT PYTHONLIBS_FOUND OR NOT PYTHON_EXECUTABLE)
	MESSAGE(SEND_ERROR "You need Python to build Python binding")
ENDIF(NOT PYTHONLIBS_FOUND OR NOT PYTHON_EXECUTABLE)

if ( ${PYTHONLIBS_FOUND} STREQUAL "TRUE" )
	include_directories(${PYTHON_INCLUDE_DIRS})
	add_library(pyblogpit SHARED pyblogpit.c)
	set_target_properties(pyblogpit PROPERTIES PREFIX "") # filename must be blogpit.so
	set_target_properties(pyblogpit PROPERTIES OUTPUT_NAME "blogpit") # filename must be blogpit.so
	target_link_libraries(pyblogpit z)
	target_link_libraries(pyblogpit blogpit_static)
	target_link_libraries(pyblogpit ${PYTHON_LIBRARIES})

	configure_file(setup.py.in ${CMAKE_CURRENT_BINARY_DIR}/setup.py)

	# Find out the install location
	execute_process ( COMMAND python -c "from distutils.sysconfig import get_python_lib; print get_python_lib(plat_specific=1)" OUTPUT_VARIABLE PYTHON_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE)
	install( TARGETS pyblogpit LIBRARY DESTINATION ${PYTHON_SITE_PACKAGES} )


endif ( ${PYTHONLIBS_FOUND} STREQUAL "TRUE")

