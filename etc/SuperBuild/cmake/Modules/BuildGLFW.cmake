include(ExternalProject)

set(GLFW_GIT_REPOSITORY "https://github.com/glfw/glfw.git")
set(GLFW_GIT_TAG "814b7929c5add4b0541ccad26fb81f28b71dc4d8") # tag: 3.3.4

set(GLFW_ARGS
    ${TLRENDER_EXTERNAL_ARGS}
    -DCMAKE_INSTALL_LIBDIR=lib
    -DGLFW_SHARED_LIBS=${BUILD_SHARED_LIBS}
    -DGLFW_BUILD_EXAMPLES=FALSE
    -DGLFW_BUILD_TESTS=FALSE
    -DGLFW_BUILD_DOCS=FALSE)

ExternalProject_Add(
    GLFW
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/GLFW
    GIT_REPOSITORY ${GLFW_GIT_REPOSITORY}
    GIT_TAG ${GLFW_GIT_TAG}
    LIST_SEPARATOR |
    CMAKE_ARGS ${GLFW_ARGS})

