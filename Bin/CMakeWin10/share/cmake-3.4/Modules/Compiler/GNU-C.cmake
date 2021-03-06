include(Compiler/GNU)
__compiler_gnu(C)

if (NOT CMAKE_C_COMPILER_VERSION VERSION_LESS 4.5)
  set(CMAKE_C90_STANDARD_COMPILE_OPTION "-std=c90")
  set(CMAKE_C90_EXTENSION_COMPILE_OPTION "-std=gnu90")
elseif (NOT CMAKE_C_COMPILER_VERSION VERSION_LESS 4.4)
  set(CMAKE_C90_STANDARD_COMPILE_OPTION "-std=c89")
  set(CMAKE_C90_EXTENSION_COMPILE_OPTION "-std=gnu89")
endif()

if (NOT CMAKE_C_COMPILER_VERSION VERSION_LESS 4.4)
  set(CMAKE_C99_STANDARD_COMPILE_OPTION "-std=c99")
  set(CMAKE_C99_EXTENSION_COMPILE_OPTION "-std=gnu99")
endif()

if (NOT CMAKE_C_COMPILER_VERSION VERSION_LESS 4.7)
  set(CMAKE_C11_STANDARD_COMPILE_OPTION "-std=c11")
  set(CMAKE_C11_EXTENSION_COMPILE_OPTION "-std=gnu11")
elseif (NOT CMAKE_C_COMPILER_VERSION VERSION_LESS 4.6)
  set(CMAKE_C11_STANDARD_COMPILE_OPTION "-std=c1x")
  set(CMAKE_C11_EXTENSION_COMPILE_OPTION "-std=gnu1x")
endif()

if(NOT CMAKE_C_COMPILER_VERSION VERSION_LESS 4.4)
  if (NOT CMAKE_C_COMPILER_FORCED)
    if (NOT CMAKE_C_STANDARD_COMPUTED_DEFAULT)
      message(FATAL_ERROR "CMAKE_C_STANDARD_COMPUTED_DEFAULT should be set for ${CMAKE_C_COMPILER_ID} (${CMAKE_C_COMPILER}) version ${CMAKE_C_COMPILER_VERSION}")
    endif()
    set(CMAKE_C_STANDARD_DEFAULT ${CMAKE_C_STANDARD_COMPUTED_DEFAULT})
  endif()
endif()


macro(cmake_record_c_compile_features)
  macro(_get_gcc_features std_version list)
    record_compiler_features(C "${std_version}" ${list})
  endmacro()

  set(_result 0)
  if (NOT CMAKE_C_COMPILER_VERSION VERSION_LESS 4.6)
    _get_gcc_features(${CMAKE_C11_STANDARD_COMPILE_OPTION} CMAKE_C11_COMPILE_FEATURES)
  endif()
  if (NOT CMAKE_C_COMPILER_VERSION VERSION_LESS 4.4)
    if (_result EQUAL 0)
      _get_gcc_features(${CMAKE_C99_STANDARD_COMPILE_OPTION} CMAKE_C99_COMPILE_FEATURES)
    endif()
    if (_result EQUAL 0)
      _get_gcc_features(${CMAKE_C90_STANDARD_COMPILE_OPTION} CMAKE_C90_COMPILE_FEATURES)
    endif()
  endif()
endmacro()
