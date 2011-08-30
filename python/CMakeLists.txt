
IF(NOT PYTHONLIBS_FOUND OR NOT PYTHON_EXECUTABLE)
	MESSAGE(SEND_ERROR "You need Python to build Python binding")
ENDIF(NOT PYTHONLIBS_FOUND OR NOT PYTHON_EXECUTABLE)

#if ( ${PYTHONLIBS_FOUND} STREQUAL "TRUE" )
	include_directories(${PYTHON_INCLUDE_DIRS})
	add_library(pyblogpit SHARED pyblogpit.c)
	set_target_properties(pyblogpit PROPERTIES PREFIX "") # filename must be pyblogpit.so
	target_link_libraries(pyblogpit z)
	target_link_libraries(pyblogpit blogpit_static)
	target_link_libraries(pyblogpit ${PYTHON_LIBRARIES})
#endif ( ${PYTHONLIBS_FOUND})

