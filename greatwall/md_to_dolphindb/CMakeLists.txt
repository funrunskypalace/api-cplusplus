set(StratRunnerSrcDir               ${DolphindbGreatWallSrcDir}/../SkyPalaceRunner)
set(ImPluginSrcDir                  ${DolphindbGreatWallSrcDir}/../implugin)
set(ImPluginLibraryRef              implugin_$ENV{OSVER}$<$<CONFIG:Debug>:d>)

file(GLOB_RECURSE md_to_dolphindb_src
    "*.h"
    "*.hpp"
    "*.cc"
    "*.cpp"
    ${DolphindbGreatWallSrcDir}/source/pch/compat.h
	${DolphindbGreatWallSrcDir}/source/pch/compat.cc
	${DolphindbGreatWallSrcDir}/source/pch/compat_*.*
)

add_library(md_to_dolphindb SHARED ${md_to_dolphindb_src})
cotire(md_to_dolphindb)
set_target_properties(md_to_dolphindb PROPERTIES OUTPUT_NAME "md_to_dolphindb_$ENV{OSVER}$<$<CONFIG:Debug>:d>")
set_property(TARGET md_to_dolphindb PROPERTY FOLDER "strategy_dll")
#add_dependencies(md_to_dolphindb )
target_include_directories(md_to_dolphindb PRIVATE ${ImPluginSrcDir}/include)
target_include_directories(md_to_dolphindb PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_directories(md_to_dolphindb PRIVATE ${ImPluginSrcDir}/target/lib)
target_link_directories(md_to_dolphindb PRIVATE ${ImPluginSrcDir}/target/lib$<$<CONFIG:Debug>:/Debug>)
target_link_directories(md_to_dolphindb PRIVATE ${StratRunnerSrcDir}/target/lib)
target_link_libraries(md_to_dolphindb skypalace_framework strat_framework_common ${ImPluginLibraryRef})

if(UNIX)
	target_link_libraries(md_to_dolphindb pthread dl)
else()
    add_precompiled_header(
        md_to_dolphindb pch/compat.h FORCEINCLUDE SOURCE_CXX ${DolphindbGreatWallSrcDir}/source/pch/compat.cc
    )
endif()
add_custom_command(TARGET md_to_dolphindb POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DolphindbGreatWallSrcDir}/config/md_to_dolphindb_config.json.template ${OpenOrderDistDir}/app_home/instance_config)
add_custom_command(TARGET md_to_dolphindb POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DolphindbGreatWallSrcDir}/config/md_to_dolphindb_config.json.template ${OpenOrderDistDir}/target/instance_config)
add_custom_command(TARGET md_to_dolphindb POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:md_to_dolphindb> ${OpenOrderDistDir}/app_home/bin/)
INSTALL(TARGETS md_to_dolphindb RUNTIME)