# Pfade
set(GO_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)   # JBEmbyAPI (go.mod)
set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin)

file(MAKE_DIRECTORY ${OUTPUT_DIR})

# Plattformabhängiger Name
if(WIN32)
    set(GO_LIB_FILE jbembyapi.dll)
elseif(APPLE)
    set(GO_LIB_FILE libjbembyapi.dylib)
else()
    set(GO_LIB_FILE libjbembyapi.so)
endif()

# Custom command: Go-Library bauen
file(GLOB GO_SOURCES ${GO_SRC_DIR}/export/*.go)
add_custom_command(
    OUTPUT ${OUTPUT_DIR}/${GO_LIB_FILE}
    COMMAND go build -buildmode=c-shared -o ${OUTPUT_DIR}/${GO_LIB_FILE} ${GO_SRC_DIR}/export
    WORKING_DIRECTORY ${GO_SRC_DIR}
    DEPENDS ${GO_SOURCES}
    COMMENT "Building Go shared library..."
)

add_custom_target(JBEmbyAPI ALL
    DEPENDS ${OUTPUT_DIR}/${GO_LIB_FILE}
)

# Export für UI
set(JBEmbyAPI_LIB ${OUTPUT_DIR}/${GO_LIB_FILE} PARENT_SCOPE)
set(JBEmbyAPI_INCLUDE_DIRS ${GO_SRC_DIR}/export ${GO_SRC_DIR}/cpp-client PARENT_SCOPE)
