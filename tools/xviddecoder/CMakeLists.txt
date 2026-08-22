# XvidDecoder -- movie decoder module implementing Blizzard's DivxDecoder ABI.
#
# Produces ONE self-contained module by linking xvidcore statically. By default
# it uses the copy vendored in vendor/libxvidcore-*; point XVIDCORE_ROOT
# somewhere else, or set XVIDCORE_INCLUDE_DIR / XVIDCORE_LIBRARY directly, to
# override.

# Locate the vendored xvidcore (newest, if several are present).
file(GLOB XVIDCORE_VENDOR_DIRS "${CMAKE_SOURCE_DIR}/vendor/libxvidcore-*")
list(SORT XVIDCORE_VENDOR_DIRS)
if (XVIDCORE_VENDOR_DIRS)
    list(GET XVIDCORE_VENDOR_DIRS -1 XVIDCORE_VENDOR_DIR)
else ()
    set(XVIDCORE_VENDOR_DIR "")
endif ()

set(XVIDCORE_ROOT "${XVIDCORE_VENDOR_DIR}" CACHE PATH "Root of an xvid tree containing xvid.h and a static xvidcore library")

# The vendored package keeps per-architecture libraries in lib/x86 and lib/x64.
if (WHOA_ARCH_32)
    set(XVIDCORE_ARCH_DIR "x86")
elseif (WHOA_ARCH_64)
    set(XVIDCORE_ARCH_DIR "x64")
elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(XVIDCORE_ARCH_DIR "x86")
else ()
    set(XVIDCORE_ARCH_DIR "x64")
endif ()

find_path(XVIDCORE_INCLUDE_DIR
    NAMES xvid.h
    HINTS
        ${XVIDCORE_ROOT}/include
        ${XVIDCORE_ROOT}
        ${XVIDCORE_ROOT}/src
    DOC "Directory containing xvid.h"
)

# libxvidcore is the static archive; xvidcore.lib in the same directory is the
# import library for xvidcore.dll and is deliberately NOT listed here.
find_library(XVIDCORE_LIBRARY
    NAMES libxvidcore xvidcore_static libxvidcore.a
    HINTS
        ${XVIDCORE_ROOT}/lib/${XVIDCORE_ARCH_DIR}
        ${XVIDCORE_ROOT}/lib
        ${XVIDCORE_ROOT}
    DOC "Static xvidcore library (NOT the import library of xvidcore.dll)"
)

if (NOT XVIDCORE_INCLUDE_DIR OR NOT XVIDCORE_LIBRARY)
    message(FATAL_ERROR
        "XvidDecoder: xvidcore not found for ${XVIDCORE_ARCH_DIR}.\n"
        "  Expected a vendored copy under vendor/libxvidcore-*, or set\n"
        "  -DXVIDCORE_ROOT=<path>, or set XVIDCORE_INCLUDE_DIR and XVIDCORE_LIBRARY\n"
        "  directly. XVIDCORE_LIBRARY must be the STATIC library; see README.md."
    )
endif ()

# An import library for xvidcore.dll contains the DLL name; a static archive
# does not. Catching this here beats shipping a build that still needs a second
# DLL at runtime.
file(STRINGS "${XVIDCORE_LIBRARY}" XVIDCORE_DLL_REF
    REGEX "xvidcore\\.dll"
    LIMIT_COUNT 1
    ENCODING UTF-8
)

if (XVIDCORE_DLL_REF)
    message(FATAL_ERROR
        "XvidDecoder: ${XVIDCORE_LIBRARY} looks like the IMPORT library for\n"
        "  xvidcore.dll, not a static archive -- linking it would leave you needing\n"
        "  xvidcore.dll next to the game. Use libxvidcore.lib / libxvidcore.a instead."
    )
endif ()

add_library(XvidDecoder SHARED
    XvidDecoder.c
)

# The client dlopen()s this by plain filename, so keep a stable base name. Unix
# builds land on libXvidDecoder.so / libXvidDecoder.dylib, both of which the
# client already looks for (src/ui/CSimpleMovieFrame.cpp:72).
set_target_properties(XvidDecoder PROPERTIES
    OUTPUT_NAME XvidDecoder
    C_VISIBILITY_PRESET hidden
)

target_include_directories(XvidDecoder
    PRIVATE
        ${XVIDCORE_INCLUDE_DIR}
)

target_link_libraries(XvidDecoder
    PRIVATE
        ${XVIDCORE_LIBRARY}
)

if (MSVC)
    # Undecorated export names regardless of architecture.
    set_target_properties(XvidDecoder PROPERTIES
        LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/XvidDecoder.def\""
    )

    # CRT linkage has to match the xvidcore build or the link fails -- with
    # LNK2038 for a /MT-vs-/MD mismatch, or a pile of unresolved __imp_* symbols
    # when a /MT object drags in the static UCRT while xvidcore expects the
    # import stubs.
    #
    # The vendored libxvidcore.lib is a RELEASE build against the DYNAMIC CRT
    # (its object directives say /DEFAULTLIB:"MSVCRT"), so this target is pinned
    # to /MD in every configuration -- including Debug, where CMake would
    # otherwise pass /MDd and mix debug and release CRTs. Nothing is allocated on
    # one side and freed on the other here (the shim owns no memory), so the
    # release CRT in a debug build is safe.
    option(XVIDDECODER_STATIC_CRT "Link XvidDecoder against the static CRT (/MT). Requires an xvidcore built the same way." 0)

    if (XVIDDECODER_STATIC_CRT)
        set(XVIDDECODER_CRT_FLAG "/MT")
    else ()
        set(XVIDDECODER_CRT_FLAG "/MD")
    endif ()

    # Scoped to this directory; does not affect the rest of the build.
    foreach (flagvar
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_MINSIZEREL
        CMAKE_C_FLAGS_RELWITHDEBINFO
    )
        if (${flagvar} MATCHES "/M[TD]d?")
            string(REGEX REPLACE "/M[TD]d?" "${XVIDDECODER_CRT_FLAG}" ${flagvar} "${${flagvar}}")
        else ()
            set(${flagvar} "${${flagvar}} ${XVIDDECODER_CRT_FLAG}")
        endif ()
    endforeach ()
endif ()

install(TARGETS XvidDecoder RUNTIME DESTINATION "bin" LIBRARY DESTINATION "bin")

message(STATUS "XvidDecoder: xvid.h in ${XVIDCORE_INCLUDE_DIR}")
message(STATUS "XvidDecoder: linking ${XVIDCORE_LIBRARY}")
