include_directories( $ENV{NUTOOLS_DIR}/include   )
include_directories( $ENV{CLHEP_INCLUDE_DIR}     )
include_directories( $ENV{GSL_INC}     )

art_make(BASENAME_ONLY
        LIBRARY_NAME   GArRecoAlg
	      LIB_LIBRARIES  nusimdata_SimulationBase
                       Geometry
		                   ReconstructionDataProducts
                       ${ART_FRAMEWORK_PRINCIPAL}
                       ${ART_FRAMEWORK_SERVICES_REGISTRY}
                       ${ART_FRAMEWORK_SERVICES_OPTIONAL}
                       ${ART_ROOT_IO_TFILESERVICE_SERVICE}
                       ${ART_FRAMEWORK_SERVICES_OPTIONAL_RANDOMNUMBERGENERATOR_SERVICE}
                       art_Persistency_Common
                       art_Persistency_Provenance
                       art_Utilities
                       canvas
                       ${MF_MESSAGELOGGER}
                       ${FHICLCPP}
                       ${CETLIB_EXCEPT}
                    ROOT::Core
                    ROOT::Hist
                    ROOT::Tree              
                       ${CLHEP}
                       ${Boost_SYSTEM_LIBRARY}
		       gsl
      MODULE_LIBRARIES Geometry
                       GArRecoAlg
                       SimulationDataProducts
		                   ReconstructionDataProducts
                       MCCheater_BackTracker_service
                       nusimdata_SimulationBase
                       nutools_RandomUtils_NuRandomService_service
		                   nug4_MagneticField_MagneticField_service
                       ${ART_FRAMEWORK_CORE}
                       ${ART_FRAMEWORK_IO_SOURCES}
                       ${ART_FRAMEWORK_PRINCIPAL}
                       art_Persistency_Common
                       art_Persistency_Provenance
                       art_Utilities
                       canvas
                       ${ART_FRAMEWORK_SERVICES_REGISTRY}
                       ${ART_FRAMEWORK_SERVICES_OPTIONAL}
                       ${ART_ROOT_IO_TFILESERVICE_SERVICE}
                       ${MF_MESSAGELOGGER}
                       ${FHICLCPP}
                       ${CETLIB_EXCEPT}
                    ROOT::Core
                    ROOT::Hist
                    ROOT::Tree              
         )

install_headers()
install_fhicl()
install_source()
