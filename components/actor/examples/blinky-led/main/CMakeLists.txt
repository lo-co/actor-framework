idf_component_register(SRCS "blinky_actor_main.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES actor freertos driver log)