cmake_minimum_required(VERSION 3.16)
project(moon_render C CXX)
set(CONFIG_LV_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_USE_THORVG_INTERNAL OFF CACHE BOOL "" FORCE)
set(LV_BUILD_CONF_PATH "${CMAKE_CURRENT_LIST_DIR}/lv_conf.h" CACHE PATH "" FORCE)
add_subdirectory(../../managed_components/lvgl__lvgl lvgl)
add_executable(moon_render render.c ../../main/moon_ui.c ../../main/moon_model.c
    ../../assets/fonts/moon_font_12.c ../../assets/fonts/moon_font_14.c
    ../../assets/fonts/moon_font_18.c ../../assets/fonts/moon_font_28.c
    ../../assets/images/moon_background.c)
target_include_directories(moon_render PRIVATE ../../main)
target_link_libraries(moon_render PRIVATE lvgl m)
