include_directories( $ENV{NUTOOLS_DIR}/include   )
include_directories( $ENV{CLHEP_INCLUDE_DIR}     )

art_make(
  LIB_LIBRARIES
    ${ROOT_EG}
    ${ROOT_CORE}
    nusimdata_SimulationBase
    ${ART_FRAMEWORK_SERVICES_REGISTRY}
    ${ART_FRAMEWORK_PRINCIPAL}
    art_Persistency_Common
    art_Persistency_Provenance
    art_Utilities
    canvas
    ${MF_MESSAGELOGGER}
    ${ROOT_EG}
                    ROOT::Core
                    ROOT::Hist
                    ROOT::Tree         
		    ROOT::Physics     
		    ROOT::EG
    ${CETLIB_EXCEPT}
    ${FHICLCPP}
    ${Boost_SYSTEM_LIBRARY}
    ${CLHEP}

  MODULE_LIBRARIES
    Utilities_Dumpers
    nusimdata_SimulationBase
    ReconstructionDataProducts
    ${ART_FRAMEWORK_CORE}
    ${ART_FRAMEWORK_PRINCIPAL}
    art_Persistency_Common
    art_Persistency_Provenance
    art_Utilities
    canvas
    ${MF_MESSAGELOGGER}
    ${ROOT_EG}
                    ROOT::Core
                    ROOT::Hist
                    ROOT::Tree              
		    ROOT::Physics     
		    ROOT::EG
    ${FHICLCPP}
    ${CETLIB_EXCEPT}
  )


install_headers()
install_fhicl()
install_source()
