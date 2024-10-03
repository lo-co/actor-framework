set(priv_req freertos)

set(src "src/actor.c" "src/time_event.c")

idf_component_register(SRCS ${src}
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES ${priv_req})