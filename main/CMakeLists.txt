idf_component_register(
    SRCS "moon_main.c" "moon_model.c" "moon_ui.c" "moon_audio.c"
         "../assets/fonts/moon_font_12.c"
         "../assets/fonts/moon_font_14.c"
         "../assets/fonts/moon_font_18.c"
         "../assets/fonts/moon_font_28.c"
         "../assets/images/moon_background.c"
    INCLUDE_DIRS "."
    REQUIRES bsp esp_timer nvs_flash
)
target_add_binary_data(${COMPONENT_LIB} "${CMAKE_CURRENT_LIST_DIR}/../assets/music/moon-bowls.pcm" BINARY RENAME_TO moon_bowls_pcm)
