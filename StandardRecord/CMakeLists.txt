# for classes_def.xml!!
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

set( PACKAGE garsoft_StandardRecord )
FILE( GLOB src_files *.cxx )

cet_make_library( LIBRARY_NAME garsoft_StandardRecord
                  SOURCE       ${src_files}
                  LIBRARIES    
		  ROOT::Core
		  ROOT::Hist
		  ROOT::Tree
                )

build_dictionary( garsoft_StandardRecord
                  DICTIONARY_LIBRARIES garsoft_StandardRecord
                )

install_headers()
install_source()
