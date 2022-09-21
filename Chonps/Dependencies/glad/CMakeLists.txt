set(GLAD_HEADER 
    include/glad/glad.h
    include/KHR/khrplatform.h
)

set(GLAD_SOURCE
    ${GLAD_HEADER}
    src/glad.c
)

add_library(glad STATIC ${GLAD_SOURCE})

target_include_directories(glad 
    PRIVATE ${PROJECT_SOURCE_DIR}/Chonps/Dependencies/glad/include
)