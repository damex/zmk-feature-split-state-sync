# Copyright 2026 Roman Kuzmitskii (@damex)
# SPDX-License-Identifier: MIT

if(CONFIG_DT_HAS_ZMK_SPLIT_STATE_SYNC_ENABLED)
  zephyr_library()
  zephyr_library_compile_options(-Wconversion -Wsign-conversion)
  zephyr_library_sources(src/split_state_sync.c)
endif()
