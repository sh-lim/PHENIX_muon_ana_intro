void g3tog4(
	    int nevents = 0, 
	    std::string inputname = "pythia_mn.root"
	    )
{

  gROOT->LoadMacro("g4libs.C");
  g4libs();

  gSystem->Load("libfun4all.so");	// framework + reco modules
  gSystem->Load("libfvtx_subsysreco.so");
  gSystem->Load("libsimreco.so");
  gSystem->Load("libPHHepMCNode.so");
  gSystem->Load("/gpfs/mnt/gpfs02/phenix/hhj/hhj1/shlim/work/PHENIX_muon_ana_intro/Simulation/PHG3toG4/install/lib/libPHG3toG4.so");

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);
  
  SubsysReco *sync = new SyncSimreco();
  se->registerSubsystem(sync);

	//init with files for magnetic field and geometry file
	PHG3toG4 *myG3toG4 = new PHG3toG4("PHG3toG4","Sim3D++.root","geom_run15.root");
	//node name of Pythia8 event
  myG3toG4->AddNode("PHHepMCGenEvent_SIGNAL");
  se->registerSubsystem(myG3toG4);

	myG3toG4->ReportEvery(1);

  // DST output manager
  Fun4AllDstInputManager *dst_input_mgr  = new Fun4AllDstInputManager("HepMC","DST");
  se->registerInputManager(dst_input_mgr);
  se->fileopen( dst_input_mgr->Name(), inputname.c_str() );

	gSystem->ListLibraries();

  // run over all events
  se->run(nevents);  
  se->End();

}

