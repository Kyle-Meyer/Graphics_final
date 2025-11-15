project(scene_lib)

#######################
### STOCK FUNCTIONS ###
### DO NOT CHANGE!  ###
#######################
set(SUB_LIB_LIST "${SUB_LIB_LIST}" ${PROJECT_NAME} PARENT_SCOPE)
file(GLOB SRC_FILES *.cpp)
file(GLOB HDR_FILES *.hpp)
set(ProjectType STATIC)
add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${SRC_FILES} PUBLIC ${HDR_FILES})
target_compile_definitions(${PROJECT_NAME} PUBLIC)
### End STOCK FUNCTIONS ###
