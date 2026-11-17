# CookTextures.cmake - cook source images into .ktx2 at build time.
#
# The texture counterpart of the game's GLSL -> SPIR-V step: every source
# asset gets its own custom command that runs `ktx create` (KTX-Software,
# fetched in the engine's CMakeLists.txt), and one custom target collects
# them so the executable depends on the cooked files.
#
#   pv_cook_textures(TARGET <target> SOURCE_DIR <dir> OUTPUT_DIR <dir>
#                    [NAME <custom-target>])
#
#   TARGET      Executable that depends on the cooked textures.
#   SOURCE_DIR  Searched recursively for source assets (see below). A
#               relative path is relative to the current source directory.
#   OUTPUT_DIR  Where .ktx2 files are written. Use a directory in the build
#               tree (a relative path is relative to the current binary
#               directory); the source layout is kept, e.g.
#               <SOURCE_DIR>/white-gold-ue/white-gold_albedo.png ->
#               <OUTPUT_DIR>/white-gold-ue/white-gold_albedo.ktx2
#   NAME        Name of the custom target (default: cook_textures). Pass a
#               different name if you call the function more than once.
#
# Source assets:
#   - Cubemap: a folder holding six faces named px, nx, py, ny, pz, nz
#     (same extension) cooks to one <folder>.ktx2.
#   - 2D texture: any other .png, .jpg/.jpeg or .exr. Files named
#     *_preview.* are skipped (thumbnails that ship with texture packs).
#   - Colour space comes from the file name: *_albedo, *_basecolor,
#     *_diffuse, *_color, *_colour and *_emissive are sRGB colour, as are
#     LDR cubemaps; every other LDR image (normal, roughness, metallic, ao,
#     height, ...) is linear data. .exr files are HDR.
#
# Every asset gets a full mip chain. Formats per PV_TEXTURE_PLATFORM:
#
#              DESKTOP                       MOBILE
#   colour     R8G8B8A8_SRGB       + zstd    ASTC 4x4 sRGB         + zstd
#   linear     R8G8B8A8_UNORM      + zstd    ASTC 4x4 UNORM        + zstd
#   HDR        R16G16B16A16_SFLOAT + zstd    E5B9G9R9_UFLOAT_PACK32 + zstd
#
# MOBILE HDR is not ASTC: ktx create (4.4) cannot encode ASTC HDR from
# float input. E5B9G9R9 is 4 bytes per pixel, and every Vulkan device
# must support sampling and linear filtering for it.
#
# Parallel cooking: each asset is its own custom command, so the build
# tool cooks assets in parallel.
#   - Ninja (recommended): cook commands share the `pv_cook` job pool.
#     ASTC encoding uses PV_COOK_THREADS threads per asset, so on MOBILE
#     the pool is sized so that (pool size) x PV_COOK_THREADS is about the
#     number of logical cores. Uncompressed DESKTOP cooks are
#     single-threaded, so there the pool is one job per core.
#   - Visual Studio / MSBuild: build with `cmake --build <dir> --parallel`.
#     MSBuild runs the custom commands of one target in parallel only with
#     CMake 3.27 or newer (policy CMP0147, enabled below). With older
#     CMake the assets cook one after another.

include_guard(GLOBAL)

# Let MSBuild run the custom commands of a target in parallel (CMake 3.27+).
if(POLICY CMP0147)
    cmake_policy(SET CMP0147 NEW)
endif()

set(PV_TEXTURE_PLATFORM "DESKTOP" CACHE STRING
    "Target platform for cooked .ktx2 textures (DESKTOP or MOBILE)")
set_property(CACHE PV_TEXTURE_PLATFORM PROPERTY STRINGS DESKTOP MOBILE)

if(NOT PV_TEXTURE_PLATFORM MATCHES "^(DESKTOP|MOBILE)$")
    message(FATAL_ERROR
        "PV_TEXTURE_PLATFORM must be DESKTOP or MOBILE, not '${PV_TEXTURE_PLATFORM}'")
endif()

# Encoder threads per asset. Many assets cook at once, so each one gets a
# share of the cores instead of all of them.
cmake_host_system_information(RESULT _pv_cook_cores QUERY NUMBER_OF_LOGICAL_CORES)
math(EXPR _pv_cook_default_threads "(${_pv_cook_cores} + 3) / 4")
set(PV_COOK_THREADS "${_pv_cook_default_threads}" CACHE STRING
    "Encoder threads per cooked texture (ktx create --threads)")

if(NOT PV_COOK_THREADS MATCHES "^[1-9][0-9]*$")
    message(FATAL_ERROR "PV_COOK_THREADS must be a positive integer, not '${PV_COOK_THREADS}'")
endif()

# Ninja only: limit how many cook commands run at once.
if(PV_TEXTURE_PLATFORM STREQUAL "MOBILE")
    math(EXPR _pv_cook_jobs "${_pv_cook_cores} / ${PV_COOK_THREADS}")
else()
    set(_pv_cook_jobs ${_pv_cook_cores})
endif()
if(_pv_cook_jobs LESS 1)
    set(_pv_cook_jobs 1)
endif()
set_property(GLOBAL APPEND PROPERTY JOB_POOLS pv_cook=${_pv_cook_jobs})

message(STATUS "Texture cooking: ${PV_TEXTURE_PLATFORM}, up to ${_pv_cook_jobs} assets "
               "at once with Ninja, ${PV_COOK_THREADS} ASTC encoder thread(s) per asset")

# Adds the custom command that cooks one asset into TEXTURE_OUTPUT and
# appends it to COOKED_TEXTURES in the caller. Reads ARGS_* and
# TEXTURE_OUT_DIR from pv_cook_textures().
function(_pv_add_cook_command KIND TEXTURE_OUTPUT)
    set(INPUTS ${ARGN})
    list(GET INPUTS 0 FIRST_INPUT)
    get_filename_component(SOURCE_NAME ${FIRST_INPUT} NAME_WE)
    get_filename_component(SOURCE_EXT  ${FIRST_INPUT} LAST_EXT)
    string(TOLOWER "${SOURCE_NAME}" SOURCE_NAME)
    string(TOLOWER "${SOURCE_EXT}"  SOURCE_EXT)

    if(SOURCE_EXT STREQUAL ".exr")
        set(KIND_ARGS ${ARGS_HDR})
    elseif(KIND STREQUAL "CUBE" OR
           SOURCE_NAME MATCHES "_(albedo|basecolor|base_color|diffuse|color|colour|emissive)$")
        set(KIND_ARGS ${ARGS_COLOR})
    else()
        set(KIND_ARGS ${ARGS_LINEAR})
    endif()
    if(KIND STREQUAL "CUBE")
        list(APPEND KIND_ARGS --cubemap)
    endif()

    file(RELATIVE_PATH TEXTURE_NAME ${TEXTURE_OUT_DIR} ${TEXTURE_OUTPUT})
    get_filename_component(TEXTURE_OUTPUT_DIR ${TEXTURE_OUTPUT} DIRECTORY)

    add_custom_command(
        OUTPUT ${TEXTURE_OUTPUT}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${TEXTURE_OUTPUT_DIR}
        COMMAND ktxtools create ${ARGS_COMMON} ${KIND_ARGS} ${INPUTS} ${TEXTURE_OUTPUT}
        # Re-cook only when a source or this module changes. Naming ktxtools
        # in COMMAND builds the tool first; it is deliberately not a file
        # dependency, so relinking ktx.exe does not re-cook every texture.
        # After upgrading KTX, delete OUTPUT_DIR to re-cook with the new tool.
        DEPENDS ${INPUTS} ${CMAKE_CURRENT_FUNCTION_LIST_FILE}
        COMMENT "Cooking texture to KTX2 (${PV_TEXTURE_PLATFORM}): ${TEXTURE_NAME}"
        JOB_POOL pv_cook
        VERBATIM
    )
    set(COOKED_TEXTURES ${COOKED_TEXTURES} ${TEXTURE_OUTPUT} PARENT_SCOPE)
endfunction()

function(pv_cook_textures)
    cmake_parse_arguments(PARSE_ARGV 0 ARG "" "TARGET;SOURCE_DIR;OUTPUT_DIR;NAME" "")

    foreach(REQUIRED TARGET SOURCE_DIR OUTPUT_DIR)
        if(NOT ARG_${REQUIRED})
            message(FATAL_ERROR "pv_cook_textures: ${REQUIRED} is required")
        endif()
    endforeach()
    if(NOT TARGET ${ARG_TARGET})
        message(FATAL_ERROR "pv_cook_textures: '${ARG_TARGET}' is not a target")
    endif()
    if(NOT TARGET ktxtools)
        message(FATAL_ERROR "pv_cook_textures: the ktx tool (target ktxtools) is not available")
    endif()
    if(NOT ARG_NAME)
        set(ARG_NAME cook_textures)
    endif()

    # Absolute paths with forward slashes: native Windows paths would break
    # the CONFIGURE_DEPENDS glob check below.
    get_filename_component(TEXTURE_SRC_DIR "${ARG_SOURCE_DIR}" ABSOLUTE)
    get_filename_component(TEXTURE_OUT_DIR "${ARG_OUTPUT_DIR}" ABSOLUTE
                           BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}")

    # ktx create options. Colour, linear and HDR differ only in format and
    # transfer function.
    set(ASTC_BLOCK_COLOR  4x4)
    set(ASTC_BLOCK_LINEAR 4x4)
    # Higher levels give smaller files but cook much slower; for a 2048^2
    # RGBA8 albedo: level 10 ~1 s / 2.8 MB, level 18 ~10 s / 2.1 MB.
    set(ZSTD_LEVEL 10)

    if(PV_TEXTURE_PLATFORM STREQUAL "MOBILE")
        set(ARGS_COLOR  --format ASTC_${ASTC_BLOCK_COLOR}_SRGB_BLOCK
                        --threads ${PV_COOK_THREADS})
        set(ARGS_LINEAR --format ASTC_${ASTC_BLOCK_LINEAR}_UNORM_BLOCK
                        --threads ${PV_COOK_THREADS})
        set(ARGS_HDR    --format E5B9G9R9_UFLOAT_PACK32)
    else()
        # TODO(desktop): ship BC formats instead of uncompressed data:
        #   colour / linear -> BC7 (BC5 for normal maps, BC4 for 1-channel)
        #   HDR skybox      -> BC6H
        # `ktx create` cannot encode BC formats, so this needs an extra step
        # with Compressonator (compressonatorcli) or NVTT (nvcompress).
        set(ARGS_COLOR  --format R8G8B8A8_SRGB)
        set(ARGS_LINEAR --format R8G8B8A8_UNORM)
        set(ARGS_HDR    --format R16G16B16A16_SFLOAT)
    endif()
    # Assign colour metadata explicitly: the source PNGs carry Photoshop ICC
    # profiles or no colour info, which ktx create rejects or warns about.
    list(APPEND ARGS_COLOR  --assign-tf srgb --assign-primaries srgb)
    list(APPEND ARGS_LINEAR --assign-tf linear)
    set(ARGS_COMMON --generate-mipmap --zstd ${ZSTD_LEVEL})

    # CONFIGURE_DEPENDS re-runs the glob at build time, so added or removed
    # images are picked up without re-running CMake by hand.
    file(GLOB_RECURSE SOURCE_IMAGES CONFIGURE_DEPENDS LIST_DIRECTORIES false
        ${TEXTURE_SRC_DIR}/*.png
        ${TEXTURE_SRC_DIR}/*.jpg
        ${TEXTURE_SRC_DIR}/*.jpeg
        ${TEXTURE_SRC_DIR}/*.exr
    )

    set(CUBE_FACES px nx py ny pz nz)   # the face order ktx create expects
    set(CUBEMAP_DIRS "")
    set(TEXTURE_SOURCES "")
    foreach(IMAGE ${SOURCE_IMAGES})
        get_filename_component(IMAGE_NAME ${IMAGE} NAME_WE)
        string(TOLOWER ${IMAGE_NAME} IMAGE_NAME)
        if(IMAGE_NAME IN_LIST CUBE_FACES)
            get_filename_component(IMAGE_DIR ${IMAGE} DIRECTORY)
            list(APPEND CUBEMAP_DIRS ${IMAGE_DIR})
        elseif(NOT IMAGE_NAME MATCHES "_preview$")
            list(APPEND TEXTURE_SOURCES ${IMAGE})
        endif()
    endforeach()
    list(REMOVE_DUPLICATES CUBEMAP_DIRS)

    set(COOKED_TEXTURES "")

    foreach(TEXTURE_SOURCE ${TEXTURE_SOURCES})
        file(RELATIVE_PATH REL ${TEXTURE_SRC_DIR} ${TEXTURE_SOURCE})
        string(REGEX REPLACE "\\.[^./]+$" ".ktx2" REL ${REL})
        _pv_add_cook_command(2D ${TEXTURE_OUT_DIR}/${REL} ${TEXTURE_SOURCE})
    endforeach()

    foreach(CUBEMAP_DIR ${CUBEMAP_DIRS})
        # All six faces must exist and share the extension of px.
        file(GLOB PX_FACE ${CUBEMAP_DIR}/px.*)
        get_filename_component(FACE_EXT "${PX_FACE}" LAST_EXT)
        set(FACES "")
        foreach(FACE ${CUBE_FACES})
            if(NOT EXISTS ${CUBEMAP_DIR}/${FACE}${FACE_EXT})
                message(FATAL_ERROR "pv_cook_textures: cubemap folder ${CUBEMAP_DIR} has no "
                                    "${FACE}${FACE_EXT} (it needs px, nx, py, ny, pz and nz "
                                    "with the same extension)")
            endif()
            list(APPEND FACES ${CUBEMAP_DIR}/${FACE}${FACE_EXT})
        endforeach()

        file(RELATIVE_PATH REL ${TEXTURE_SRC_DIR} ${CUBEMAP_DIR})
        if(REL STREQUAL "")
            get_filename_component(REL ${CUBEMAP_DIR} NAME)
        endif()
        _pv_add_cook_command(CUBE ${TEXTURE_OUT_DIR}/${REL}.ktx2 ${FACES})
    endforeach()

    if(NOT COOKED_TEXTURES)
        message(WARNING "pv_cook_textures: no textures found in ${TEXTURE_SRC_DIR}")
    endif()

    add_custom_target(${ARG_NAME} ALL DEPENDS ${COOKED_TEXTURES})
    add_dependencies(${ARG_TARGET} ${ARG_NAME})
endfunction()
