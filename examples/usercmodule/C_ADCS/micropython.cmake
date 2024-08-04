# Create an INTERFACE library for our C module.
add_library(usermod_C_ADCS INTERFACE)

# Add our source files to the lib
target_sources(usermod_C_ADCS INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/C_Adcs.c
    ${CMAKE_CURRENT_LIST_DIR}/Adcs.c
    ${CMAKE_CURRENT_LIST_DIR}/Control.c
    ${CMAKE_CURRENT_LIST_DIR}/Ekf.c
	${CMAKE_CURRENT_LIST_DIR}/linearAlgebra.c
	#    ${CMAKE_CURRENT_LIST_DIR}/igrf.c
	#	${CMAKE_CURRENT_LIST_DIR}/igrfCoeffs.c
)

# Add the current directory as an include directory.
target_include_directories(usermod_C_ADCS INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE usermod_C_ADCS)
