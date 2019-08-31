void Run_picodstobject_tutorial(const char *outFile = "picodstobject.root")
{
	gSystem->Load("libmutoo_subsysreco.so");
	gSystem->Load("libfvtx_subsysreco.so");
	gSystem->Load("libpicodst_object.so");

  Fun4AllServer *se = Fun4AllServer::instance();

	se->registerSubsystem(new MuonReadbackDST());

  FvtxReadbackDST *fvtxoo = new FvtxReadbackDST();
  fvtxoo->Verbosity(0);
  se->registerSubsystem(fvtxoo);

  /*
    VERSIONS FOR SingleMuonContainer
    1- very basic single muon
    2- minimum single muon information in order to reconstruct di-muons
    3- now is the same as version2, it is supposed to be an extended object for HF analysis
    4- first version with FVTX variables
    5- version with FVTX plus a minimum RPC information
    6- full FVTX+RPC object for W analysis 
    7- version with a complete set of FVTX variables
    9- version with FVTX swapped matching variables (DEFAULT)
  */

	//single muon container
	mFillSingleMuonContainer* msngl = new mFillSingleMuonContainer();
	msngl->set_is_sim(false);
	msngl->set_bbcz_cut(30.0); //bbcz cut
	msngl->set_p_cut(3.0); //p cut
	msngl->set_lastgap_cut(2); //lastgap cut
	msngl->set_chi2_cut(30); //MuTr chi2 cut
	msngl->set_DG0_cut(80); //DG0 cut
	msngl->set_DDG0_cut(60); //DDG0 cut
	msngl->set_match_fvtx(false); //require FVTX-MuTr matching
	se->registerSubsystem(msngl);

	//dimuon container
  mFillDiMuonContainer* mdi = new mFillDiMuonContainer(false);
	mdi->set_is_sim(false);
	mdi->set_bbcz_cut(30.0); //bbcz cut
	mdi->set_mass_cut(1.0); //mass cut for dimuon
	mdi->set_is_pp(false); //save centrality information
	se->registerSubsystem(mdi);

	//output manager
  Fun4AllOutputManager *outpico = new Fun4AllDstOutputManager("Outee",outFile);
  outpico->AddNode("SingleMuonContainer");
	outpico->AddNode("DiMuonContainer");
	outpico->AddNode("PHGlobal");
	outpico->AddNode("RunHeader");
	outpico->AddNode("EventHeader");
  outpico->AddNode("TrigLvl1");
  outpico->AddNode("Sync");
  outpico->AddNode("VtxOut");
  outpico->AddEventSelector("mFillDiMuonContainer");
  se->registerOutputManager(outpico);
	
}

void 
InputData(vector<string> &indata)
{
  indata.push_back("CNT");
  indata.push_back("MWG");
  return;
}
