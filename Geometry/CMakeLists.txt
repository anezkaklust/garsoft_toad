include_directories( $ENV{CLHEP_INCLUDE_DIR} )
include_directories( $ENV{ART_INC} )

art_make( LIB_LIBRARIES     ${MF_MESSAGELOGGER}
                            ${FHICLCPP}
                            ${CETLIB}
                            ${CETLIB_EXCEPT}
                            ${CLHEP}
                            ROOT::Core
                            ROOT::Hist
                            ROOT::Tree              
		            ROOT::MathCore
                            ROOT::Geom
			    ROOT::EG
                            ${ROOT_XMLIO}
                            ${ROOT_GDML}
                            ${ROOT_TREEPLAYER}
                            ${ROOT_FFTW}
                            ${ROOT_REFLEX}
                            ${ROOT_EGPYTHIA6}
                            ${ROOT_GUI}
                            ${Boost_SYSTEM_LIBRARY}
                            SummaryDataProducts
          SERVICE_LIBRARIES Geometry
                            GArChannelMapAlg
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

add_subdirectory(gdml)
add_subdirectory(ChannelMapAlgs)
add_subdirectory(FieldMaps)
