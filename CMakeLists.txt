# Copyright 2026 Roman Kuzmitskii (@damex)
# SPDX-License-Identifier: MIT

if(CONFIG_DT_HAS_ZMK_SPLIT_STATE_SYNC_ENABLED)
  zephyr_library()
  zephyr_library_compile_options(
      -Werror
      -Wextra
      -Wshadow
      -Wformat=2
      -Wlogical-op
      -Wduplicated-cond
      -Wduplicated-branches
      -Wmissing-prototypes
      -Wjump-misses-init
      -Wwrite-strings
      -Winit-self
      -Wold-style-definition
      -fanalyzer
  )
  zephyr_library_sources(src/split_state_sync.c)
endif()
