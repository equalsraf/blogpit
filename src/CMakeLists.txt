

# libblogpit
set(libblogpit_SOURCES blogpit.c strlcpy.c strlcat.c util.c)

add_library(blogpit_static STATIC ${libblogpit_SOURCES} )
set_target_properties(blogpit_static PROPERTIES OUTPUT_NAME "blogpit")
target_link_libraries( blogpit_static ${LIBGIT2} )
target_link_libraries( blogpit_static z )

add_library(blogpit_shared SHARED ${libblogpit_SOURCES})
set_target_properties(blogpit_shared PROPERTIES OUTPUT_NAME "blogpit")
target_link_libraries( blogpit_shared ${LIBGIT2} )
target_link_libraries( blogpit_shared z )

# blogpitc
add_executable(blogpitc main.c)
target_link_libraries( blogpitc z blogpit_static )



