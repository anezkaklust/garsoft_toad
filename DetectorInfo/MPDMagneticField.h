/// \file MPDMagneticField.h
/// \brief Describe the magnetic field structure of a detector
/// 
/// \version $Id: MPDMagneticField.h,v 1.3 2019-11-06 02:46:38 trj Exp $
/// \author trj@fnal.gov
//////////////////////////////////////////////////////////////////////////
/// \namespace mag
/// A namespace for simulated magnetic fields
//////////////////////////////////////////////////////////////////////////
#ifndef MAG_MPDMAGNETICFIELD_H
#define MAG_MPDMAGNETICFIELD_H

#include <string>

#include "TGeoVolume.h"
#include "TVector3.h"
#include "TH1.h"

// Geant4 includes
#include "Geant4/G4ThreeVector.hh"

// Framework includes
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"

namespace mag {

  typedef enum MPDMagneticFieldMode {
    kAutomaticBFieldMode=-1, // Used by DriftElectronsAlg
    kNoBFieldMode=0,         // no field
    kConstantBFieldMode=1,   // constant field
    kFieldRZMapMode=2        // read a map as a function of r and z
  } MagFieldMode_t;

  // assumes a square grid of R, Z samples for speed in lookup, starting at an origin 0,0

  struct RZFieldMap {
    TVector3 CoordOffset;
    TVector3 ZAxis;
    float dr;
    float dz;
    std::vector<std::vector<float> > br;
    std::vector<std::vector<float> > bz;
    size_t nr() { return br.size(); };
    size_t nz() { if (nr()>0) { return br[0].size(); } else { return 0;} }; 
  };

  struct MPDMagneticFieldDescription
  {
    MagFieldMode_t fMode;   ///< type of field used
    G4ThreeVector  fField;  ///< description of the field (uniform only)
    G4String       fVolume; ///< G4 volume containing the field
    TGeoVolume*    fGeoVol; ///< pointer to TGeoVolume with the field
    std::string    fRZFieldMapFilename; ///< file name for reading in the RZ field map
    RZFieldMap     fRZFieldMap;  ///< RZ field map if needed
    float          fScaleFactor; ///< Used to scale the magnetic field.
  };

  // Specifies the magnetic field over all space
  //
  // The default implementation, however, uses a nearly trivial,
  // non-physical hack.
  class MPDMagneticField {
  public:
    MPDMagneticField(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);
    ~MPDMagneticField(){};

    void reconfigure(fhicl::ParameterSet const& pset);

    std::vector<MPDMagneticFieldDescription> const& Fields()                   const { return fFieldDescriptions;            }
    size_t                                       NumFields()                const { return fFieldDescriptions.size();     }
    MagFieldMode_t                        const& UseField(size_t f)         const { return fFieldDescriptions[f].fMode;   }
    std::string                           const& MagnetizedVolume(size_t f) const { return fFieldDescriptions[f].fVolume; }

    // return the field at a particular point
    G4ThreeVector  const FieldAtPoint(G4ThreeVector const& p=G4ThreeVector(0)) const;

    // This method will only return a uniform field based on the input
    // volume name.  If the input volume does not have a uniform field
    // caveat emptor
    G4ThreeVector  const UniformFieldInVolume(std::string const& volName) const;
    
  private:

    void MakeCheckPlots();

    //bool fEnableField;
    //bool fUseUniformField;

    float fGlobalScaleFactor;

    std::vector<MPDMagneticFieldDescription> fFieldDescriptions; ///< Descriptions of the fields

    // parameters for check plots

    std::vector<int> fNBinsXCheckPlots;
    std::vector<int> fNBinsYCheckPlots;
    std::vector<int> fNBinsZCheckPlots;
    std::vector<float> fXLowCheckPlots;
    std::vector<float> fXHighCheckPlots;
    std::vector<float> fYLowCheckPlots;
    std::vector<float> fYHighCheckPlots;
    std::vector<float> fZLowCheckPlots;
    std::vector<float> fZHighCheckPlots;
    std::vector<float> fXCentCheckPlots;
    std::vector<float> fYCentCheckPlots;
    std::vector<float> fZCentCheckPlots;
    std::vector<TH1*> fCheckPlots;

  };

}

DECLARE_ART_SERVICE(mag::MPDMagneticField, LEGACY)
#endif // MAG_MPDMAGNETICFIELD_H