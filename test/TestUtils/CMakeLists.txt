include_directories ( $ENV{CETLIB_INC} )
cet_test(NameSelector_test
         SOURCES   NameSelector_test.cxx
         LIBRARIES TestUtils
                   art_Utilities canvas
         USE_BOOST_UNIT
)

cet_test(ProviderList_test
  USE_BOOST_UNIT
)

cet_test(StopWatch_test)
