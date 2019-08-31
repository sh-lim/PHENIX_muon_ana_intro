void phpythia8(
	       int nevents = 50, 
				 int seed = 0,
				 float minpT_jpsi = 1.0,
	       std::string outputname = "pythia_mn"
		)
{
  gSystem->Load("libfun4all.so");	// framework + reco modules
  gSystem->Load("libPHHepMCNode.so");
	gSystem->Load("/gpfs/mnt/gpfs02/phenix/hhj/hhj1/shlim/work/PHENIX_muon_ana_intro/Simulation/PHPythia8/install/lib/libPHPythia8.so");
  gSystem->Load("libPHPythia.so");
  gSystem->Load("libPHPythiaEventGen.so");
  gSystem->Load("libLHAPDF");
  gSystem->Load("libPHPyTrigger.so");		// For PHPyTrigger derived classes
  gSystem->Load("libPHPyParticleSelect.so");	// For PHPyParticleSelect derived classes
  gSystem->Load("libsimreco.so");	// framework + reco modules
  gSystem->Load("libfvtx_subsysreco.so");

  
  ////////////////////////////
	float ptMin = 0.0;
  bool debug = false;
  int verbosity = 0;
  ////////////////////////////

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(verbosity);
  
  SubsysReco *sync = new SyncSimreco();
  se->registerSubsystem(sync);

	//particle trigger for J/psi->mumu
  PHPy8ParticleTrigger *pTrig = new PHPy8ParticleTrigger();
  pTrig->Verbosity(0);
	pTrig->AddParents("443"); //Jpsi
	pTrig->AddParticles("13, -13"); //muon
	pTrig->SetParticleNum(2); //trigger if there are 2+ particles from Jpsi 
  pTrig->SetPtLow(ptMin); //single muon pt cut
  pTrig->PrintConfig();

  //Pythia 
  PHPythia8 *phpythia = new PHPythia8("Pythia8",PHPythia8::HEPMC);
  TRandom3 *r = new TRandom3(seed);
  int rand = r->Uniform(1,1000000);
  phpythia->Verbosity(verbosity);
  phpythia->SetSeed(rand);			
	phpythia->SetConfigFile("phpythia8_Jpsi.cfg"); //configuration file
  phpythia->registerTrigger(pTrig); //particle trigger
  phpythia->SetNodeName("PHHepMCGenEvent_SIGNAL"); //node name
	phpythia->BeamDiamondVtx(15); //event vertex
	phpythia->UseRealVtx(false);
  se->registerSubsystem(phpythia);

  // DST output manager
  std::string outName = outputname+".root";
  Fun4AllDstOutputManager *dst_output_mgr  = new Fun4AllDstOutputManager("PHPYTHIA",outName.c_str());
  dst_output_mgr->AddNode("Sync");
  dst_output_mgr->AddNode("EventHeader");
  dst_output_mgr->AddNode("PHHepMCGenEvent_SIGNAL");
  se->registerOutputManager(dst_output_mgr);

  cout << "NEVENTS: " << nevents << endl;
  if(debug)rc->Print();

	gSystem->ListLibraries();

  // run over all events
  se->run(nevents,true);  
  se->End();
}

