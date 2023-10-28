# Find the OpenSSL library.
#
# This module defines the following variables:
#
# * OpenSSL_VERSION
# * OPENSSL_VERSION
# * OpenSSL_INCLUDE_DIRS
# * OPENSSL_INCLUDE_DIRS
# * OpenSSL_LIBRARIES
# * OPENSSL_LIBRARIES
# * OPENSSL_VERSION
#
# This module defines the following imported targets:
#
# * OpenSSL::SSL
# * OpenSSL::Crypto
#
# This module defines the following interfaces:
#
# * OpenSSL

set(OpenSSL_VERSION "3.1.4")
set(OPENSSL_VERSION ${OpenSSL_VERSION})

find_path(OpenSSL_INCLUDE_DIR NAMES openssl/ssl.h)
set(OpenSSL_INCLUDE_DIRS ${OpenSSL_INCLUDE_DIR})
set(OPENSSL_INCLUDE_DIR ${OpenSSL_INCLUDE_DIR})
set(OPENSSL_INCLUDE_DIRS ${OpenSSL_INCLUDE_DIRS})

find_library(OpenSSL_SSL_LIBRARY NAMES ssl libssl PATH_SUFFIXES lib64)
find_library(OpenSSL_Crypto_LIBRARY NAMES crypto libcrypto PATH_SUFFIXES lib64)
set(OpenSSL_LIBRARIES ${OpenSSL_SSL_LIBRARY} ${OpenSSL_Crypto_LIBRARY})
set(OPENSSL_LIBRARIES ${OpenSSL_LIBRARIES})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    OpenSSL
    REQUIRED_VARS OpenSSL_INCLUDE_DIR OpenSSL_SSL_LIBRARY OpenSSL_Crypto_LIBRARY
    VERSION_VAR OpenSSL_VERSION)
mark_as_advanced(OpenSSL_INCLUDE_DIR OpenSSL_SSL_LIBRARY OpenSSL_Crypto_LIBRARY OPENSSL_INCLUDE_DIR)

if(OpenSSL_FOUND AND NOT TARGET OpenSSL::SSL)
    add_library(OpenSSL::SSL UNKNOWN IMPORTED)
    set_target_properties(OpenSSL::SSL PROPERTIES
        IMPORTED_LOCATION "${OpenSSL_SSL_LIBRARY}"
        INTERFACE_COMPILE_DEFINITIONS OpenSSL_FOUND
        INTERFACE_INCLUDE_DIRECTORIES "${OpenSSL_INCLUDE_DIR}")
endif()
if(OpenSSL_FOUND AND NOT TARGET OpenSSL::Crypto)
    add_library(OpenSSL::Crypto UNKNOWN IMPORTED)
    set_target_properties(OpenSSL::Crypto PROPERTIES
        IMPORTED_LOCATION "${OpenSSL_Crypto_LIBRARY}"
        INTERFACE_COMPILE_DEFINITIONS OpenSSL_FOUND
        INTERFACE_INCLUDE_DIRECTORIES "${OpenSSL_INCLUDE_DIR}")
endif()
if(OpenSSL_FOUND AND NOT TARGET OpenSSL)
    add_library(OpenSSL INTERFACE)
    target_link_libraries(OpenSSL INTERFACE OpenSSL::SSL OpenSSL::Crypto)
endif()

