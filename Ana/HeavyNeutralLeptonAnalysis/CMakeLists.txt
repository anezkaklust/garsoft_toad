include_directories( $ENV{GEANT4_FQ_DIR}/include )
include_directories( $ENV{NUTOOLS_DIR}/include   )
include_directories( $ENV{CLHEP_INCLUDE_DIR}     )

art_make(BASENAME_ONLY
      MODULE_LIBRARIES Geometry
                       ReadoutSimulation
                       SimulationDataProducts
                       RawDataProducts
                       ReconstructionDataProducts
                       MCCheater_BackTracker_service
                       nusimdata_SimulationBase
                       nurandom_RandomUtils_NuRandomService_service
                       nug4_MagneticFieldServices_MagneticFieldServiceStandard_service
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
