file(GLOB PRIVATE_SOURCES
    "*.cpp"
)

add_library(bsp STATIC
    ${PRIVATE_SOURCES}
)

target_include_directories(bsp
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(bsp
    PRIVATE
        gx
        model
        gameui
        cursor
        world
    PUBLIC
        bc
        common
        storm
        tempest
)
