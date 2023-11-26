set(LIBINISRCS headings.h
                ini.cpp
                ini.h
                keys.h
                list.h)
add_library(libini ${LIBINISRCS})
target_include_directories(libini PUBLIC include)
target_compile_definitions(libini PRIVATE HAVE_STRCASECMP)