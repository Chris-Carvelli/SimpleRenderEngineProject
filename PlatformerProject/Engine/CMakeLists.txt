# Engine Only

file(GLOB_RECURSE MyEngine-src "*.cpp")
file(GLOB_RECURSE MyEngine-hpp "*.h")

set(MYENGINE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "" FORCE)


add_library(MyEngine ${MyEngine-src} ${MyEngine-hpp})

target_link_libraries(MyEngine ${SGE_LIBRARIES})

target_include_directories(MyEngine PUBLIC ${MYENGINE_INCLUDE_DIR})
target_include_directories(MyEngine PUBLIC "3rdParty")
