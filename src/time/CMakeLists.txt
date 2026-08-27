file(GLOB PRIVATE_SOURCES "*.cpp")

add_library(time STATIC
    ${PRIVATE_SOURCES}
)

target_include_directories(time
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(time
    PRIVATE
        client
        clientobject
        console
        db
        event
        glue
        gx
        math
        model
        net
        util
    PUBLIC
        bc
        common
        storm
        tempest
        sound
)
