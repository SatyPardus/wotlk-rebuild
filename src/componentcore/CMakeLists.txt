file(GLOB PRIVATE_SOURCES "*.cpp")

add_library(componentcore STATIC
    ${PRIVATE_SOURCES}
)

target_include_directories(componentcore
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(componentcore
    PRIVATE
        glue
        client
        console
        db
        event
        gx
        model
        net
        sound
        ui
        util
    PUBLIC
        bc
        storm
        common
)
