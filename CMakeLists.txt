cmake_minimum_required(VERSION 3.21)
project(zal-web)
set(CMAKE_CXX_STANDARD 17)

add_library (${PROJECT_NAME} SHARED )

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "zal-web.node" PREFIX "" SUFFIX "")

target_sources (${PROJECT_NAME} PRIVATE
  cpp/zal-web.h
  cpp/zal-web.cc
)

target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB} /home/konstantin/.vs/Zal-Core/out/build/linux-debug/libZalMainLib.a)

# dependencies
if(NOT EXISTS ${PROJECT_SOURCE_DIR}/node-addon/node-addon-api)
  file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/node-addon)
  set(NPM_CMD npm)
#  if(WIN32)
    # workaround for windows, without this the npm command will return immediately
#    set(NPM_CMD cmd /Q /C npm)
#  endif()
  # fetch packages using npm, then move them to ./node-addon
  execute_process(
    COMMAND ${NPM_CMD} install node-addon-api@6.0.0 node-api-headers@0.0.4 --save false
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/node-addon
  )
  file(RENAME ${PROJECT_SOURCE_DIR}/node_modules/node-addon-api ${PROJECT_SOURCE_DIR}/node-addon/node-addon-api)
  file(RENAME ${PROJECT_SOURCE_DIR}/node_modules/node-api-headers ${PROJECT_SOURCE_DIR}/node-addon/node-api-headers)
  file(REMOVE_RECURSE ${PROJECT_SOURCE_DIR}/node-addon/node_modules)
  execute_process(
    COMMAND node -e "let a=require('./symbols.js').v8;console.log('LIBRARY node.exe\\nEXPORTS\\n'+[...a.node_api_symbols,...a.js_native_api_symbols].join('\\n'))"
    OUTPUT_FILE libnode.def
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/node-addon/node-api-headers
  )
endif()
target_include_directories(${PROJECT_NAME} PUBLIC
  ${PROJECT_NAME} 
  ${PROJECT_SOURCE_DIR}/node-addon/node-addon-api 
  ${PROJECT_SOURCE_DIR}/node-addon/node-api-headers/include 
  ../.vs/Zal-Core/HLib
  ../.vs/Zal-Core/MainLib
  ../.vs/Zal-Core/ThirdParty/Sqlite
)

# platform special options
#if(APPLE)
#  target_link_options(${PROJECT_NAME} PUBLIC -undefined dynamic_lookup)
#elseif(MSVC)
#  if(NOT EXISTS ${PROJECT_SOURCE_DIR}/node-addon/node-api-headers/libnode.lib)
#    execute_process(
#      COMMAND ${CMAKE_AR} /def:libnode.def /out:libnode.lib
#      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/node-addon/node-api-headers
#    )
#  endif()
#  target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/node-addon/node-api-headers/libnode.lib)
#elseif(MINGW)
#  if(NOT EXISTS ${PROJECT_SOURCE_DIR}/node-addon/node-api-headers/libnode.a)
#    execute_process(
#      COMMAND dlltool -d libnode.def -l libnode.a
#      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/node-addon/node-api-headers
#    )
#  endif()
#  target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/node-addon/node-api-headers/libnode.a)
#endif()
