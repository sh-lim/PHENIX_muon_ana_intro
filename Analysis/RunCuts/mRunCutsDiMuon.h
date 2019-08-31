#include <SubsysReco.h>

class VtxOut;
class DiMuonContainer;
class MCDiMuonContainer;
class MutrRefitSingleMuonContainer;
class TrigLvl1;
class TrigRunLvl1;
class TTree;
class TH3D;
class TFile;
class SyncObject;
class TriggerHelper;
class RunHeader;

class mRunCutsDiMuon : public SubsysReco
{ 
 public:

  //! default constructor
  mRunCutsDiMuon(const char *name);
  
  //! destructor
  virtual 
  ~mRunCutsDiMuon();

  //! global initialization
  int Init(PHCompositeNode *topNode);
  
  //! Run initialization
  int InitRun(PHCompositeNode *topNode);
  
  //! event method
  int process_event(PHCompositeNode *topNode);
  
  //! global termination
  int End(PHCompositeNode *topNode);

	//! book histograms
	void book_histos();

 private:

	TFile *file_out;

	//Jpsi info.
	TH3D *h3d_mass_pT_rapidity[2][3]; 

};
