cmake_minimum_required(VERSION 2.6.4)
project(CFS_SC C)

include_directories(fsw/public_inc)
include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)

aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(sc ${APP_SRC_FILES})

include_directories(fsw/src)

# Add the tables
add_static_cfe_tables(sc fsw/tables/sc_ats1.c  fsw/tables/sc_rts001.c  fsw/tables/sc_rts002.c)
