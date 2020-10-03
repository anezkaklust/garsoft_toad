////////////////////////////////////////////////////////////////////////////////
// Class:	    MatchingPerformance
// Plugin Type: analyzer (art v2_11_02)
// File:		MatchingPerformance_module.cc
//
// Generated 25 Sep 2020 by Leo Bellantoni
// 
// For each reco track
//     Use backtracker to get matching MCparticles.  Take charged & stable
//         matching MCParticle with largest energy fraction for the track
//         Does this MCParticle go eventually into the ECAL?
//         Use backtracker info to determine "starting" end of the track.
//     Record chi-2 and NTPC clusters in the track (minus 5 track par NDoFs)
//     If the MCparticle goes into ECAL,
//         For each ECAL cluster,
//             Does the cluster come from the track's MCParticle?
//             Is the cluster matched to the initial track?
//             ...and of course, are both true?
// 
////////////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art_root_io/TFileService.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "art/Persistency/Common/PtrMaker.h"
#include "canvas/Persistency/Common/FindOne.h"
#include "canvas/Persistency/Common/FindOneP.h"
#include "canvas/Persistency/Common/FindMany.h"
#include "canvas/Persistency/Common/FindManyP.h"

#include "nusimdata/SimulationBase/MCTruth.h"
#include "MCCheater/BackTracker.h"
#include "nusimdata/SimulationBase/MCParticle.h"
#include "MCCheater/BackTracker.h"
#include "ReconstructionDataProducts/Track.h"
#include "ReconstructionDataProducts/Cluster.h"

#include "TTree.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"

#include <string>
#include <vector>
#include <unordered_map>



namespace gar {

	class MatchingPerformance : public art::EDAnalyzer {
	public:
		explicit MatchingPerformance(fhicl::ParameterSet const & p);
		// The compiler-generated destructor is fine for non-base
		// classes without bare pointers or other resource use.

		// Plugins should not be copied or assigned.
		MatchingPerformance(MatchingPerformance const &) = delete;
		MatchingPerformance(MatchingPerformance &&) = delete;
		MatchingPerformance & operator = (MatchingPerformance const &) = delete;
		MatchingPerformance & operator = (MatchingPerformance &&) = delete;

		virtual void beginJob() override;

		// Required functions.
		void analyze(art::Event const & e) override;



	private:
		void ClearVectors();
		void FillVectors(art::Event const & e);



		// Position of TPC from geometry service; 1 S Boston Ave.
		double ItsInTulsa[3];

		// Input data labels
		std::string fGeantLabel;
		std::string fTrackLabel;
		std::string fClusterLabel;
		std::string fECALAssnLabel;

		// the analysis tree
		TTree *fTree;

		// Geometry service
		const gar::geo::GeometryCore* fGeo;
		// Backtracker service
		cheat::BackTrackerCore* fBack;
		// PDG database from ROOT
		TDatabasePDG* pdgInstance;



		// The analysis tree. Use Rtypes.h here, as these data get used by root
		// global event info
		Int_t	fRun;
		Int_t	fSubRun;
		Int_t	fEvent;

		// This analysis starts from reco'd tracks
		std::vector<ULong64_t>			fTrackIDNumber;
		std::vector<Float_t>			fTrackX;
		std::vector<Float_t>			fTrackY;
		std::vector<Float_t>			fTrackZ;
		std::vector<Float_t>			fTrackPX;
		std::vector<Float_t>			fTrackPY;
		std::vector<Float_t>			fTrackPZ;
		std::vector<Int_t>				fTrackQ;
		std::vector<Float_t>			fTrackLen;
		std::vector<Float_t>			fTrackChi2;
		std::vector<Int_t>				fNTPCClustersOnTrack;
		std::vector<Double_t>			fTrackTime;
		// Which match to qualified MCParticles
        std::vector<Int_t>              fMCPDG;
        std::vector<Int_t>              fMCPDGMother;
        std::vector<Float_t>            fMCPStartX;
        std::vector<Float_t>            fMCPStartY;
        std::vector<Float_t>            fMCPStartZ;
        std::vector<Float_t>            fMCPStartPX;
        std::vector<Float_t>            fMCPStartPY;
        std::vector<Float_t>            fMCPStartPZ;
        std::vector<std::string>        fMCPProc;
        std::vector<std::string>        fMCPEndProc;
        std::vector<Float_t>            fMCPTime;



	};
}



//==============================================================================
//==============================================================================
//==============================================================================
// constructor
gar::MatchingPerformance::MatchingPerformance(fhicl::ParameterSet const & p) : EDAnalyzer(p) {

	fGeantLabel    = p.get<std::string>("GEANTLabel",   "geant");
	fTrackLabel    = p.get<std::string>("TrackLabel",   "track");
	fClusterLabel  = p.get<std::string>("ClusterLabel", "calocluster");
	fECALAssnLabel = p.get<std::string>("ECALAssnLabel","trkecalassn");

	pdgInstance = TDatabasePDG::Instance();

	consumesMany<std::vector<simb::MCTruth> >();
	consumes<std::vector<simb::MCParticle> >(fGeantLabel);
	consumes<std::vector<rec::Track> >(fTrackLabel);
	consumes<std::vector<rec::Cluster> >(fClusterLabel);
    consumes<art::Assns<rec::Cluster, rec::Track>>(fECALAssnLabel);

} // end constructor



//==============================================================================
//==============================================================================
//==============================================================================
void gar::MatchingPerformance::beginJob() {

	fGeo = gar::providerFrom<geo::Geometry>();
	ItsInTulsa[0] = fGeo->TPCXCent();		// 1 S Boston Ave
	ItsInTulsa[1] = fGeo->TPCYCent();
	ItsInTulsa[2] = fGeo->TPCZCent();

	art::ServiceHandle<art::TFileService> tfs;
	fTree = tfs->make<TTree>("GArAnaTree","GArAnaTree");



	fTree->Branch("Run",					&fRun,				"Run/I");
	fTree->Branch("SubRun",					&fSubRun,			"SubRun/I");
	fTree->Branch("Event",					&fEvent,			"Event/I");

	fTree->Branch("TrackIDNumber",			&fTrackIDNumber);
	fTree->Branch("TrackX",					&fTrackX);
	fTree->Branch("TrackY", 				&fTrackY);
	fTree->Branch("TrackZ", 				&fTrackZ);
	fTree->Branch("TrackPX",				&fTrackPX);
	fTree->Branch("TrackPY",				&fTrackPY);
	fTree->Branch("TrackPZ",				&fTrackPZ);
	fTree->Branch("TrackQ", 				&fTrackQ);
	fTree->Branch("TrackLen",				&fTrackLen);
	fTree->Branch("TrackChi2",				&fTrackChi2);
	fTree->Branch("NTPCClustersOnTrack",	&fNTPCClustersOnTrack);
	fTree->Branch("TrackTime",				&fTrackTime);

    fTree->Branch("PDG",                    &fMCPDG);
    fTree->Branch("PDGMother",              &fMCPDGMother);
    fTree->Branch("MCPStartX",              &fMCPStartX);
    fTree->Branch("MCPStartY",              &fMCPStartY);
    fTree->Branch("MCPStartZ",              &fMCPStartZ);
	fTree->Branch("MCPStartPX",             &fMCPStartPX);
    fTree->Branch("MCPStartPY",             &fMCPStartPY);
    fTree->Branch("MCPStartPZ",             &fMCPStartPZ);
    fTree->Branch("MCPProc",                &fMCPProc);
    fTree->Branch("MCPEndProc",             &fMCPEndProc);
    fTree->Branch("MCPTime",                &fMCPTime);

	return;
}  // End of :MatchingPerformance::beginJob



//==============================================================================
//==============================================================================
//==============================================================================
void gar::MatchingPerformance::analyze(art::Event const & event) {

	/// Is the backtracker good to go?
	cheat::BackTrackerCore const* const_bt = gar::providerFrom<cheat::BackTracker>();
	fBack = const_cast<cheat::BackTrackerCore*>(const_bt);
	if ( !fBack->HasTracks() || !fBack->HasClusters() ) {
		throw cet::exception("MatchingPerformance") << " Not enough backtracker info"
            << " Line " << __LINE__ << " in file " << __FILE__ << std::endl;
	}

	ClearVectors();
	FillVectors(event);
	fTree->Fill();
	return;
}



//==============================================================================
//==============================================================================
//==============================================================================
void gar::MatchingPerformance::ClearVectors() {
	// clear out all our vectors

	fTrackIDNumber.clear();
	fTrackX.clear();
	fTrackY.clear();
	fTrackZ.clear();
	fTrackPX.clear();
	fTrackPY.clear();
	fTrackPZ.clear();
	fTrackQ.clear();
	fTrackLen.clear();
	fTrackChi2.clear();
	fNTPCClustersOnTrack.clear();
	fTrackTime.clear();

    fMCPDG.clear();
    fMCPDGMother.clear();
    fMCPStartX.clear();
    fMCPStartY.clear();
    fMCPStartZ.clear();
    fMCPStartPX.clear();
    fMCPStartPY.clear();
    fMCPStartPZ.clear();
    fMCPProc.clear();
    fMCPEndProc.clear();
    fMCPTime.clear();

	return;
} // end :MatchingPerformance::ClearVectors



//==============================================================================
//==============================================================================
//==============================================================================
void gar::MatchingPerformance::FillVectors(art::Event const& event) {



	// =============  Get art handles ==========================================
	// Get handles for Tracks, clusters, associations between them
	art::Handle< std::vector<rec::Track> > TrackHandle;
	if (!event.getByLabel(fTrackLabel, TrackHandle)) {
		throw cet::exception("MatchingPerformance") << " No rec::Track"
			<< " Line " << __LINE__ << " in file " << __FILE__ << std::endl;
	}
    art::Handle< std::vector<rec::Cluster> > RecoClusterHandle;
    if (!event.getByLabel(fClusterLabel, RecoClusterHandle)) {
        throw cet::exception("MatchingPerformance") << " No rec::Cluster branch."
            << " Line " << __LINE__ << " in file " << __FILE__ << std::endl;
    }
    // BUILD art::FindManyP<rec::Track, rec::TrackEnd>* findManyCALTrackEnd = NULL;
    // BUILD findManyCALTrackEnd = new art::FindManyP<rec::Track, rec::TrackEnd>
    // BUILD         (RecoClusterHandle,event,fECALAssnLabel);
 
	// Get handles for MCinfo, also good for MCPTrajectory.  Want to get all
	// MCTruths, regardless of generator label.
	std::vector< art::Handle< std::vector<simb::MCTruth> > > mctruthHandles;
	art::Handle< std::vector<simb::MCParticle> > MCPHandle;
	event.getManyByType(mctruthHandles);
	if (mctruthHandles.size()!=1) {
		throw cet::exception("MatchingPerformance") << " Need just 1 simb::MCTruth"
			<< " Line " << __LINE__ << " in file " << __FILE__ << std::endl;
	}

	if (!event.getByLabel(fGeantLabel, MCPHandle)) {
		throw cet::exception("MatchingPerformance") << " No simb::MCParticle"
			<< " Line " << __LINE__ << " in file " << __FILE__ << std::endl;
	}

	// Make the usual map for the MC info
    typedef int TrkId;
    std::unordered_map<TrkId, Int_t> TrackIdToIndex;
    Int_t index = 0;
    for ( auto const& mcp : (*MCPHandle) ) {
        int TrackId = mcp.TrackId();
        TrackIdToIndex[TrackId] = index++;
    }





	// =============  Pull art handles =========================================
	fRun	= event.run();
	fSubRun = event.subRun();
	fEvent  = event.id().event();



    for ( auto const& track : (*TrackHandle) ) {
		std::vector<std::pair<simb::MCParticle*,float>> MCsfromTrack;
		gar::rec::Track* nonconst_track = const_cast<gar::rec::Track*>(&track);
		MCsfromTrack = fBack->TrackToMCParticles(nonconst_track);
		int nMCsfromTrack = MCsfromTrack.size();
		if (nMCsfromTrack==0) continue;

		simb::MCParticle theMCPart;
		for (int iMCfromTrack =0; iMCfromTrack<nMCsfromTrack; ++iMCfromTrack) {
			// Plausible MCParticle to make this track?
			theMCPart = *(MCsfromTrack[iMCfromTrack].first);
			TParticlePDG* particle = pdgInstance->GetParticle(theMCPart.PdgCode());
			if ( particle->Charge()==0.0 ) continue;
			if ( particle->Stable()==0   ) continue;
			break;			// Take highest fraction candidate
		}

		// Does theMCParticle go out to the ECAL?
		int nTraj = theMCPart.NumberTrajectoryPoints();
		TVector3 doink;			bool whackThatECAL = false;
		for (int iTraj=0; iTraj<nTraj; ++iTraj) {
			doink.SetXYZ(theMCPart.Vx(iTraj),theMCPart.Vx(iTraj),theMCPart.Vx(iTraj));
			if ( fGeo->PointInECALBarrel(doink) ) {
				whackThatECAL = true;
				break;
			}
		}
		if (!whackThatECAL) continue;

		// Which end of this track corresponds to the start of the reco track?
		const TLorentzVector& positionMCP = theMCPart.Position(0);
		float distStart = std::hypot(track.Vertex()[0] -positionMCP[0],
									 track.Vertex()[1] -positionMCP[1],
									 track.Vertex()[2] -positionMCP[2]);
		float distEnd   = std::hypot(track.End()[0]	   -positionMCP[0],
									 track.End()[1]	   -positionMCP[1],
									 track.End()[2]	   -positionMCP[2]);
		rec::TrackEnd kate = (distStart<distEnd) ? rec::TrackEndBeg
												 : rec::TrackEndEnd;



		// Ready to record some info about the track & matching MC info
        fTrackIDNumber.push_back(track.getIDNumber());
        if (kate==rec::TrackEndBeg) {
            fTrackX.push_back   (track.Vertex()[0]);
            fTrackY.push_back   (track.Vertex()[1]);
            fTrackZ.push_back   (track.Vertex()[2]);
            fTrackPX.push_back  (track.Momentum_beg()*track.VtxDir()[0]);
            fTrackPY.push_back  (track.Momentum_beg()*track.VtxDir()[1]);
            fTrackPZ.push_back  (track.Momentum_beg()*track.VtxDir()[2]);
            fTrackQ.push_back   (track.ChargeBeg());
            fTrackLen.push_back (track.LengthBackward());
			fTrackChi2.push_back(track.ChisqBackward());
			
        } else {
            fTrackX.push_back  (track.End()[0]);
            fTrackY.push_back  (track.End()[1]);
            fTrackZ.push_back  (track.End()[2]);
            fTrackPX.push_back (track.Momentum_end()*track.EndDir()[0]);
            fTrackPY.push_back (track.Momentum_end()*track.EndDir()[1]);
            fTrackPZ.push_back (track.Momentum_end()*track.EndDir()[2]);
            fTrackQ.push_back  (track.ChargeEnd());
            fTrackLen.push_back(track.LengthForward());		
        }
        fNTPCClustersOnTrack.push_back(track.NHits());
		fTrackTime.push_back(track.Time());

		fMCPDG.push_back(theMCPart.PdgCode());
        TrkId momTrkId = theMCPart.Mother();
        int momPDG = 0;
        if (momTrkId>0) {
            int momIndex = TrackIdToIndex[momTrkId];
            momPDG   = (*MCPHandle).at(momIndex).PdgCode();
		}
        fMCPDGMother.push_back(momPDG);

        const TLorentzVector& position = theMCPart.Position(0);
        fMCPStartX.push_back(position.X());
        fMCPStartY.push_back(position.Y());
        fMCPStartZ.push_back(position.Z());
        const TLorentzVector& momentum = theMCPart.Momentum(0);
        fMCPStartPX.push_back(momentum.Px());
        fMCPStartPY.push_back(momentum.Py());
        fMCPStartPZ.push_back(momentum.Pz());

        fMCPProc.push_back(theMCPart.Process());
        fMCPEndProc.push_back(theMCPart.EndProcess());
	    fMCPTime.push_back(theMCPart.T());	

	} // end loop over TrackHandle





	return;
} // end MatchingPerformance::FillVectors





DEFINE_ART_MODULE(gar::MatchingPerformance)