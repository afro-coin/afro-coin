# Copyright (c) 2024-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or https://opensource.org/license/mit/.

add_executable(test_afrocoin-qt
  apptests.cpp
  optiontests.cpp
  rpcnestedtests.cpp
  test_main.cpp
  uritests.cpp
  util.cpp
  ../../init/afrocoin-qt.cpp
)

target_link_libraries(test_afrocoin-qt
  core_interface
  afrocoinqt
  test_util
  afrocoin_node
  Boost::headers
  Qt5::Test
)

import_plugins(test_afrocoin-qt)

if(ENABLE_WALLET)
  target_sources(test_afrocoin-qt
    PRIVATE
      addressbooktests.cpp
      wallettests.cpp
      ../../wallet/test/wallet_test_fixture.cpp
  )
endif()

add_test(NAME test_afrocoin-qt
  COMMAND test_afrocoin-qt
)
if(WIN32 AND VCPKG_TARGET_TRIPLET)
  # On Windows, vcpkg configures Qt with `-opengl dynamic`, which makes
  # the "minimal" platform plugin unusable due to internal Qt bugs.
  set_tests_properties(test_afrocoin-qt PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=windows"
  )
endif()

install(TARGETS test_afrocoin-qt
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
