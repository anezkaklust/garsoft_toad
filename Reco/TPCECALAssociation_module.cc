////////////////////////////////////////////////////////////////////////
// Class:       TPCECALAssociation
// Plugin Type: producer (art v2_11_02)
// File:        TPCECALAssociation_module.cc
//
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Persistency/Common/PtrMaker.h"

#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "Geometry/Geometry.h"
#include "DetectorInfo/DetectorClocksService.h"
#include "DetectorInfo/DetectorPropertiesService.h"

#include "ReconstructionDataProducts/Track.h"
#include "ReconstructionDataProducts/Cluster.h"
#include "RecoAlg/TrackPropagator.h"

#include "art_root_io/TFileService.h"
#include "TH2D.h"



namespace gar {
    namespace rec {



        class TPCECALAssociation : public art::EDProducer {
        public:
            explicit TPCECALAssociation(fhicl::ParameterSet const & p);
            // The compiler-generated destructor is fine for non-base
            // classes without bare pointers or other resource use.

            // Plugins should not be copied or assigned.
            TPCECALAssociation(TPCECALAssociation const &) = delete;
            TPCECALAssociation(TPCECALAssociation &&) = delete;
            TPCECALAssociation & operator = (TPCECALAssociation const &) = delete;
            TPCECALAssociation & operator = (TPCECALAssociation &&) = delete;

            // Required functions.
            void beginJob() override;
            void produce(art::Event & e) override;

        private:

            // Declare member data here.
            std::string fTrackLabel;    ///< label to find the reco tracks
            std::string fClusterLabel;  ///< label to find the right reco caloclusters
            int fVerbosity;
            float fTrackEndXCut;        ///< Extrapolate only track ends outside central drift
            float fTrackEndRCut;        ///< Extrapolate only track ends outside central region
            float fTrackPerpRCut;       ///< Max dist cluster center to circle of track (z,y) only
            float fTrackBarrelXCut;     ///< Max dist cluster center (drift time compensated) track in x

            const detinfo::DetectorProperties*  fDetProp;    ///< detector properties
            const detinfo::DetectorClocks*      fClocks;     ///< Detector clock information
            const geo::GeometryCore*            fGeo;        ///< pointer to the geometry
            float                               maxXdisplacement;

            TH1F* radClusTrack;                              ///< Cluster to track in transverse plane
            TH1F* debugHist;
        };



        TPCECALAssociation::TPCECALAssociation(fhicl::ParameterSet const & p) : EDProducer{p} {

            fTrackLabel      = p.get<std::string>("TrackLabel", "track");
            fClusterLabel    = p.get<std::string>("ClusterLabel","calocluster");
            fVerbosity       = p.get<int>("Verbosity", 0);
            fTrackEndXCut    = p.get<float>("TrackEndXCut",    240.0);
            fTrackEndRCut    = p.get<float>("TrackEndRCut",    238.0);
            fTrackPerpRCut   = p.get<float>("fTrackPerpRCut",   10.0);
	        fTrackBarrelXCut = p.get<float>("fTrackBarrelXCut", 10.0);
            fDetProp = gar::providerFrom<detinfo::DetectorPropertiesService>();
            fClocks  = gar::providerFrom<detinfo::DetectorClocksService>();
            fGeo     = gar::providerFrom<geo::Geometry>();

            produces< art::Assns<gar::rec::Cluster, gar::rec::Track, gar::rec::TrackEnd > >();
        }



        void TPCECALAssociation::beginJob() {
            if (fVerbosity>0) {
                art::ServiceHandle< art::TFileService > tfs;
                radClusTrack = tfs->make<TH1F>("radClusTrack",
                    "(z,y) distance from cluster to track", 100, 0.0,60.0);
                debugHist = tfs->make<TH1F>("debugHist"," ",
                    100,-200.0,+200.0);
            }
        }



        void TPCECALAssociation::produce(art::Event & e) {
        
            // Get tracks and clusters.  If either is missing, just skip this event
            // processing.  That's not an exception
            art::Handle< std::vector<gar::rec::Cluster> > theClusterHandle;
            e.getByLabel(fClusterLabel, theClusterHandle);
            if (!theClusterHandle.isValid()) return;
            art::Handle< std::vector<gar::rec::Track> >   theTrackHandle;
            e.getByLabel(fTrackLabel, theTrackHandle);
            if (!theTrackHandle.isValid()) return;

            // fClocks service provides this info on event-by-event basis not at beginJob.
            maxXdisplacement = fDetProp->DriftVelocity(fDetProp->Efield(),fDetProp->Temperature())
                              *fClocks->SpillLength();


            std::unique_ptr<art::Assns<gar::rec::Cluster, gar::rec::Track, gar::rec::TrackEnd>>
                            ClusterTrackAssns(new art::Assns<gar::rec::Cluster,gar::rec::Track, gar::rec::TrackEnd>);
            // LATER auto const clusterPtrMaker = art::PtrMaker<rec::Cluster>(e, theClusterHandle.id());
            // MAYBE auto const trackPtrMaker   = art::PtrMaker<rec::Track>  (e, theTrackHandle.id());

            for (size_t iCluster=0; iCluster<theClusterHandle->size(); ++iCluster) {
                gar::rec::Cluster cluster = (*theClusterHandle)[iCluster];
                float yClus = cluster.Position()[1];    float zClus = cluster.Position()[2];
                float rClus = std::hypot(zClus,yClus);    // LATER float xClus = cluster.Position()[0];

                for (size_t iTrack=0; iTrack<theTrackHandle->size(); ++iTrack) {
                    gar::rec::Track track = (*theTrackHandle)[iTrack];

                    // Which if any ends of the track are near the outer edges of the TPC?
                    // These specific cuts could perhaps be increased
                    bool outside[2]; outside[0] = outside[1] = false;
                    
                    if ( abs(track.Vertex()[0]) > fTrackEndXCut ||
                        std::hypot(track.Vertex()[1],track.Vertex()[2]) > fTrackEndRCut ) {
                        outside[TrackEndBeg] = true;
                    }
                    if ( abs(track.End()[0]) > fTrackEndXCut ||
                        std::hypot(track.End()[1],track.End()[2]) > fTrackEndRCut ) {
                        outside[TrackEndEnd] = true;
                    }

                    // Plot and cut on the distance of cluster to helix in transverse plane.
                    for (TrackEnd tEnd = TrackEndBeg; tEnd >= TrackEndEnd; --tEnd) {
                        if (outside[tEnd]) {
                            float trackPar[5];    float trackEnd[3];
                            if (tEnd==TrackEndBeg) {
                                for (int i=0; i<5; ++i) trackPar[i] = track.TrackParBeg()[i];
                                for (int i=0; i<3; ++i) trackEnd[i] = track.Vertex()[i];
                            } else {
                                for (int i=0; i<5; ++i) trackPar[i] = track.TrackParEnd()[i];
                                for (int i=0; i<3; ++i) trackEnd[i] = track.End()[i];
                            }
                            float radius = 1.0/trackPar[2];
                            float zCent = trackPar[1] - radius*sin(trackPar[3]);                        
                            float yCent = trackPar[0] + radius*cos(trackPar[3]);

                            float distRadially = std::hypot(zClus-zCent,yClus-yCent) -abs(radius);
                            distRadially = abs(distRadially);
                            radClusTrack->Fill( distRadially );
                            
                            if ( distRadially > fTrackPerpRCut ) {
                                // This track-cluster match fails
                                outside[tEnd] = false;
                                continue;
                            }

                            // Require plausible extrapolation in x as well
                            TVector3 clusterCenter(cluster.Position());
                            if (fGeo->PointInECALBarrel(clusterCenter)) {
                                // Extrapolate track to that radius
                                float retXYZ[3];
                                int errcode = util::TrackPropagator::PropagateToCylinder(
                                    trackPar,trackEnd,rClus, 0.0, 0.0, retXYZ);
                                if ( errcode!=0 ) {
                                    // This track-cluster match fails.  Error code 1, there is
									// no intersection at all, is possible
                                    outside[tEnd] = false;
                                    continue;
                                }
								float extrapXerr = retXYZ[0] -clusterCenter.X();
								debugHist->Fill(extrapXerr);
                            } else {
                                // Figure you are in an endcap
                    
                    
                            }    
                        }
                    }



                    // Cut on some kind of info from X.  Perhaps:  Determine if the cluster is on
                    // the endcap or the barrel.  If on the barrel, extrapolate the track out
                    // to a cylinder (diameter take from position of cluster), and cut on distance 
                    // in X from the extrapolated track to the cluster.  If on an endcap...
                    // maybe only can require that tan(lambda) & endpoint are consistent with
                    // whichever endcap the cluster is actually in.


                    // What if there are multiple tracks matching the cluster?  What is a good
                    // Quality of association cut?  Well right now I just put multiple track
                    // to cluster associations in the art record.


                    
                    /*if ( extrapDist<fExtrapDistCut && matchHelixDist<fMatchHelixCut ) {
                        // Make the cluster-track association
                        art::Ptr<gar::rec::Cluster> const clusterPtr = clusterPtrMaker(iCluster);
                        art::Ptr<gar::rec::Track>   const   trackPtr = trackPtrMaker(iTrack);
                        ClusterTrackAssns->addSingle(clusterPtr,trackPtr,whichEnd);
                    }*/
                }
            }

            e.put(std::move(ClusterTrackAssns));
            return;
        }



        DEFINE_ART_MODULE(TPCECALAssociation)

    } // namespace rec
} // namespace gar


/*
                    // Which trackend to use for the extrapolation?
                    float begDir[3];    float endDir[3];
                    for (int i=0; i<3; ++i) {
                        begDir[i] = -track.VtxDir()[i];        // Unit vectors continuing the track
                        endDir[i] = -track.EndDir()[i];
                    }

                    // Dot product of direction vector with distance from trackend to cluster
                    TVector3 begDelPoint = TVector3(cluster.Position())
                                          -TVector3(track.Vertex());
                    TVector3 begDelPointHat = begDelPoint.Unit();
                    TVector3 endDelPoint = TVector3(cluster.Position())
                                          -TVector3(track.End());
                    TVector3 endDelPointHat = endDelPoint.Unit();
                    float begDot    = begDelPoint.Dot( TVector3(begDir) );
                    float endDot    = endDelPoint.Dot( TVector3(endDir) );
                    float begDotHat = begDelPointHat.Dot( TVector3(begDir) );
                    float endDotHat = endDelPointHat.Dot( TVector3(endDir) );



                    // Plot extrapolation angle vs distance before cut on anything
                    if (fVerbosity>0) {
                        withWithoutDistS->Fill(begDot,begDotHat);
                        withWithoutDistS->Fill(endDot,endDotHat);
                        withWithoutDistL->Fill(begDot,begDotHat);
                        withWithoutDistL->Fill(endDot,endDotHat);
                    }

                    // Consider only extrapolations that pass the angle cut; neither does, we done.
                    bool considerBeg = begDotHat>fExtrapAngleCut;
                    bool considerEnd = endDotHat>fExtrapAngleCut;
                    if (!considerBeg &&!considerEnd ) continue;

                    TrackEnd whichEnd;
                    if        ( considerBeg &&!considerEnd ) {
                        whichEnd = TrackEndBeg;
                    } else if (!considerBeg && considerEnd ) {
                        whichEnd = TrackEndEnd;
                    } else if ( begDot<endDot ) {
                        // If both ends inside track angle cut, pick the shortest extrapolation distance
                        whichEnd = TrackEndBeg;
                    } else {
                        whichEnd = TrackEndEnd;
                    }



                    // Find the distance off the track, and the throw arm of the extrapolation
                    float matchHelixDist, extrapDist;
                    if ( whichEnd==TrackEndBeg ) {
                        util::TrackPropagator::DistXYZ(track.TrackParBeg(),track.Vertex(), cluster.Position(), matchHelixDist);
                        extrapDist = begDot;
                    } else {
                        util::TrackPropagator::DistXYZ(track.TrackParEnd(),track.End(),    cluster.Position(), matchHelixDist);
                        extrapDist = endDot;
                    }

                    // Histogram distance off track vs extrapolation distance and cut on it
                    if (fVerbosity>0) {
                       distOffvsExtrapS->Fill(extrapDist, matchHelixDist);
                       distOffvsExtrapL->Fill(extrapDist, matchHelixDist);
                    }
*/

