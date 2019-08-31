void Run_runcuts(
		const char *filein = "file.lst"
){
	gSystem->Load("/gpfs/mnt/gpfs02/phenix/hhj/hhj1/shlim/work/PHENIX_muon_ana_intro/Analysis/RunCuts/install/lib/libruncuts.so");
	gSystem->Load("libfun4all");

  Fun4AllServer *se = Fun4AllServer::instance();

  mRunCutsDiMuon *mrun = new mRunCutsDiMuon("mRunCutsDiMuon");
	se->registerSubsystem(mrun);

	Fun4AllInputManager *inMan = new Fun4AllDstInputManager("Background","DST","TOP");
	se->registerInputManager(inMan);
	inMan->AddListFile(filein);

	se->run(0);
	se->End();

	delete se;
}

