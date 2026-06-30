/// \file RunAction.hh
/// \brief Definition of the QCRNG::RunAction class

#ifndef QCRNGRunAction_h
#define QCRNGRunAction_h 1

#include "G4UserRunAction.hh"

#include "globals.hh"

class G4Run;

namespace QCRNG{

  /// Run action class

  class RunAction : public G4UserRunAction{
  public:
    RunAction(){};
    ~RunAction() override = default;

    void BeginOfRunAction(const G4Run*) override;
    void EndOfRunAction(const G4Run*) override;

  };

}  // namespace QCRNG

#endif
