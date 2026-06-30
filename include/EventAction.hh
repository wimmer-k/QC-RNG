/// \file EventAction.hh
/// \brief Definition of the QCRNG::EventAction class

#ifndef QCRNGEventAction_h
#define QCRNGEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include <map>


class G4Event;

namespace QCRNG{

  class RunAction;

  /// Event action class

  class EventAction : public G4UserEventAction{
  public:
    EventAction(RunAction* runAction);
    ~EventAction() override = default;

    void BeginOfEventAction(const G4Event* event) override;
    void EndOfEventAction(const G4Event* event) override;

    void AddHit(int det, double edep, double time);

  private:
    RunAction* fRunAction = nullptr;
    std::map<int,double> fDetEnergy;
    std::map<int,double> fDetTime;
  };

}  // namespace QCRNG

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
