# Copyright (c) 2022 Connor Mellon
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

set(SRCS
	pch.h
	.licenseheader
)

add_library(ingenue STATIC ${SRCS})
target_precompile_headers(ingenue PRIVATE pch.h)

target_link_libraries(ingenue
	PRIVATE
		project_settings
)

target_include_directories(ingenue INTERFACE ${CMAKE_SOURCE_DIR})
target_include_directories(ingenue PUBLIC ${CMAKE_SOURCE_DIR}/cubic)
target_include_directories(ingenue PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRCS})
