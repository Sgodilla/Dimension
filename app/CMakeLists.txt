# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Declares and names the project.

project("dimension")

set (anOcctLibs
        TKernel TKMath TKG2d TKG3d TKGeomBase TKBRep TKGeomAlgo TKTopAlgo TKShHealing TKMesh
        # exchange
        TKPrim TKBO TKBool TKFillet TKOffset
        TKXSBase
        TKIGES
        TKSTEPBase TKSTEPAttr TKSTEP209 TKSTEP
        # OCCT Visualization
        TKService TKHLR TKV3d TKOpenGles
        )

set(aLibDeps "")

# OCCT libraries
include_directories(${OCCT_ROOT}/include/opencascade)
foreach(anOcctLib ${anOcctLibs})
    add_library(lib_${anOcctLib} SHARED IMPORTED)
    set_target_properties(lib_${anOcctLib} PROPERTIES IMPORTED_LOCATION ${OCCT_ROOT}/lib/lib${anOcctLib}.so)
    list(APPEND aLibDeps lib_${anOcctLib})
endforeach()

# FreeType
add_library(lib_FreeType SHARED IMPORTED)
set_target_properties(lib_FreeType PROPERTIES IMPORTED_LOCATION ${FREETYPE_ROOT}/libs/${ANDROID_ABI}/libfreetype.so)
list(APPEND aLibDeps lib_FreeType)

# glm
include_directories(src/main/cpp/glm)
add_definitions("-DGLM_FORCE_SIZE_T_LENGTH -DGLM_FORCE_RADIANS -DGLM_FORCE_DEPTH_ZERO_TO_ONE -DGLM_LANG_STL11_FORCED")
add_subdirectory(src/main/cpp/glm)

# Import the ARCore (Google Play Services for AR) library.
add_library(arcore SHARED IMPORTED)
set_target_properties(arcore PROPERTIES IMPORTED_LOCATION
        ${ARCORE_LIBPATH}/${ANDROID_ABI}/libarcore_sdk_c.so
        INTERFACE_INCLUDE_DIRECTORIES ${ARCORE_INCLUDE}
        )

# Import the glm header file from the NDK.
#add_library( glm INTERFACE )
#set_target_properties( glm PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${GLM_INCLUDE})

set(libs_dir ${CMAKE_CURRENT_SOURCE_DIR}/libraries)

# Sources
file(GLOB native_srcs
        "src/main/cpp/ARCore/*.cpp"
        "src/main/cpp/OCCT/*.cpp"
        "src/main/cpp/Prometheus/*.cpp"
        )

# Output binary
add_library(dimension_native SHARED ${native_srcs})
# Includes
target_include_directories(dimension_native PRIVATE ${libs_dir})
target_include_directories(dimension_native PRIVATE
        src/main/cpp/ARCore
        src/main/cpp/OCCT
        src/main/cpp/Prometheus
        )
# Build
target_link_libraries(dimension_native
        ${aLibDeps}
        android
        log
        GLESv2
        glm
        arcore)