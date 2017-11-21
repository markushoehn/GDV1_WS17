#Author: Roman Getto                          
#Roman.Getto@gris.informatik.tu-darmstadt.de                       

cmake_minimum_required(VERSION 2.8)
project(PRAK1)

# files of project
add_executable(main main.h main.cpp TriangleMesh.h TriangleMesh.cpp Vec3.h)

option(AUTO_SEARCH_AND_INCLUDE_OpenGL "You can activate this option or include OpenGL by yourself" ON)
option(AUTO_SEARCH_AND_INCLUDE_Glut "You can activate this option or include GLUT by yourself" ON)
option(AUTO_SEARCH_AND_INCLUDE_Glew "You can activate this option or include GLEW by yourself" ON)

# FIND AND INCLUDE OPENGL				
if(AUTO_SEARCH_AND_INCLUDE_OpenGL)
find_package(OpenGL REQUIRED)
include_directories( ${OpenGL_INCLUDE_DIR})
target_link_libraries(main ${OPENGL_LIBRARIES})
endif(AUTO_SEARCH_AND_INCLUDE_OpenGL)

# FIND AND INCLUDE GLUT 				 
if(AUTO_SEARCH_AND_INCLUDE_Glut)
find_package(GLUT REQUIRED)
include_directories( ${GLUT_INCLUDE_DIR})
target_link_libraries(main ${GLUT_LIBRARIES})        
endif(AUTO_SEARCH_AND_INCLUDE_Glut)

# FIND AND INCLUDE GLEW                                                   
if(AUTO_SEARCH_AND_INCLUDE_Glew)
find_package(GLEW REQUIRED)
include_directories( ${GLEW_INCLUDE_DIRS})
target_link_libraries(main ${GLEW_LIBRARIES})
else(AUTO_SEARCH_AND_INCLUDE_Glew)
# At least define the glew library if the package is not searched       
    IF (WIN32)
    set(GLEW_LIBRARY GLEW32)
    ELSE (WIN32)
    set(GLEW_LIBRARY GLEW)
    ENDIF(WIN32)
    target_link_libraries(main ${GLEW_LIBRARY})
endif(AUTO_SEARCH_AND_INCLUDE_Glew)

#include source                                               
include_directories( ${PROJECT_SOURCE_DIR})


