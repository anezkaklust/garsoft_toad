
include_directories( include/ )

cet_make_library(LIBRARY_NAME garsoft_paramsim_utils
                 SOURCE
                   src/CAF.cpp
                   src/Utils.cpp
		               src/MCP_Skimmer.cpp
		               LIBRARIES
                    ROOT::Core
                    ROOT::Hist
                    ROOT::Tree              
                    ROOT::EG
                    ROOT::MathCore
                 )

cet_make_exec(cafanatree_module
  SOURCE
    main/cafanatree_module.cxx
  LIBRARIES
    garsoft_paramsim_utils
                    ROOT::Core
                    ROOT::Hist
                    ROOT::Tree              
)

cet_make_exec(mcp_skimmer
  SOURCE
    main/mcp_skimmer.cxx
  LIBRARIES
    garsoft_paramsim_utils
                    ROOT::Core
                    ROOT::Hist
                    ROOT::Tree              

)

install_source()
install_headers()
