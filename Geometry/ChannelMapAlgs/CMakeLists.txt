include_directories( $ENV{CLHEP_INCLUDE_DIR}     )

art_make( BASENAME_ONLY
          LIBRARY_NAME      GArChannelMapAlg
          LIB_LIBRARIES     ${MF_MESSAGELOGGER}
                            SimulationDataProducts
                            ${FHICLCPP}
                            ${CETLIB}
                            ${CETLIB_EXCEPT}
                            ${CLHEP}
                    ROOT::Core
                    ROOT::Hist
                    ROOT::Tree              
                            ${ROOT_GEOM}
                            ${ROOT_XMLIO}
                            ${ROOT_GDML}
                            ${ROOT_EG}
                            ${ROOT_TREEPLAYER}
                            ${ROOT_FFTW}
                            ${ROOT_REFLEX}
                            ${ROOT_EGPYTHIA6}
                            ${ROOT_GUI}
                            ${Boost_SYSTEM_LIBRARY}
                            Geometry
          SERVICE_LIBRARIES Geometry
                            SimulationDataProducts
                            ${ART_FRAMEWORK_CORE}
                            ${ART_FRAMEWORK_IO_SOURCES}
                            ${ART_FRAMEWORK_PRINCIPAL}
                            art_Persistency_Common
                            art_Persistency_Provenance
                            art_Utilities
                            canvas
                            ${ART_FRAMEWORK_SERVICES_REGISTRY}
                            ${MF_MESSAGELOGGER}
                            ${FHICLCPP}
                            ${CETLIB}
                            ${CETLIB_EXCEPT}

          )

install_headers()
install_fhicl()
install_source()
