if(WIN32)
    set(TgbotLib TgBot.lib)
    if (EXISTS $ENV{APPDATA}/../Local/vcpkg/vcpkg.path.txt)
        set(FILE $ENV{APPDATA}/../Local/vcpkg/vcpkg.path.txt)
        file(READ ${FILE} FILE_CONTENTS)
        set(CMAKE_TOOLCHAIN_FILE ${FILE_CONTENTS}/scripts/buildsystems/vcpkg.cmake)
    else()
        set(FILE $ENV{LOCALAPPDATA}/vcpkg/vcpkg.path.txt)
        file(READ ${FILE} FILE_CONTENTS)
        set(CMAKE_TOOLCHAIN_FILE ${FILE_CONTENTS}/scripts/buildsystems/vcpkg.cmake)
    endif()
else()
    set(TgbotLib libTgBot.a)
    if (EXISTS $ENV{HOME}/.vcpkg/vcpkg.path.txt)
        set(FILE $ENV{HOME}/.vcpkg/vcpkg.path.txt)
        file(READ ${FILE} FILE_CONTENTS)
        set(CMAKE_TOOLCHAIN_FILE ${FILE_CONTENTS}/scripts/buildsystems/vcpkg.cmake)
    else()
        set(FILE /var/.vcpkg/vcpkg.path.txt)
        file(READ ${FILE} FILE_CONTENTS)
        set(CMAKE_TOOLCHAIN_FILE ${FILE_CONTENTS}/scripts/buildsystems/vcpkg.cmake)
    endif()
endif()

SET(CMAKE_BUILD_TYPE "Release")
SET(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "" FORCE)

cmake_minimum_required(VERSION 3.23)
project(SashaProject)
SET(CMAKE_CXX_STANDARD 17)

SET(path ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/${TgbotLib})

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(CURL)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)

find_package(Doxygen REQUIRED)

if(DOXYGEN_FOUND)
    set(DOXYGEN_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)

    add_custom_target(
            documentation
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
    )
endif()

if (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
    add_definitions(-DHAVE_CURL)
endif()

include_directories(include ${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIR})

add_executable(SashaProject src/main.cpp src/Converter.cpp src/Converter.h)
target_link_libraries(SashaProject ${path} cpr::cpr nlohmann_json::nlohmann_json ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} ${CURL_LIBRARIES})
set_target_properties(SashaProject PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin/)
# set_target_properties(SashaProject PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/$<0:>)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

install(TARGETS SashaProject DESTINATION ${CMAKE_INSTALL_PREFIX}/TgBot/bin)
install(DIRECTORY res DESTINATION ${CMAKE_INSTALL_PREFIX}/TgBot)
file(GLOB DLL_FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/Release/*.dll)
install(FILES ${DLL_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/TgBot/bin)
