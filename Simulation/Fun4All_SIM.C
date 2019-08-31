void Fun4All_SIM(
		 int nEvents = 0,
		 int run = 431846, //reference run
		 std::string pisaFile = "PISAEvent.root",
		 std::string pythiaFile = "pythia_mn.root",
		 std::string outputName  = "dimuon_pdst.root"
		 )
{

  /////////////////////////
  bool use_fvtx = true;
  bool use_svx = true;
  bool fill_mc_info = true;
  bool use_eval = true;
  bool write_pdst = false;

  bool debug = false;
  int verbosity = 0;
  if(debug) verbosity = 5;
  /////////////////////////

  if(outputName.empty())
	{
		cout << "outputName is empty! Exiting..." << endl;
		return;
	}

  std::string outputPDST = outputName;

  // load libraries
  gSystem->Load("libfun4all.so");
  gSystem->Load("libPISARoot.so");
  gSystem->Load("libfun4allfuncs.so");
  gSystem->Load("libmutoo_modules.so");
  gSystem->Load("libmutoo_subsysreco.so");
  gSystem->Load("libfvtx_subsysreco.so");
  gSystem->Load("libMWG_interface.so");
  gSystem->Load("libPHGeant" );
  gSystem->Load("libsimreco_base");
  gSystem->Load("libmuon_subsysreco");
  gSystem->Load("librpc_subsysreco");
  gSystem->Load("librpc_muotrackreco");
  gSystem->Load("libfun4allfuncs_muons");
  gSystem->Load("libsvx");
  gSystem->Load("liblvl2");
  gSystem->Load("libPythia6.so");
  gSystem->Load("libPHPythia.so");
  gSystem->Load("libPHPythiaEventGen.so");
  gSystem->Load("/gpfs/mnt/gpfs02/phenix/hhj/hhj1/shlim/work/PHENIX_muon_ana_intro/Simulation/picoDST_object/install/lib/libpicodst_object.so");
  
  cout << ">>>> libraries loaded" << endl;
  
  ///////////////////////////////////////////
  // recoConsts setup
  //////////////////////////////////////////
  recoConsts *rc = recoConsts::instance();
  rc->set_IntFlag("SVXACTIVE", use_svx);
  rc->set_IntFlag("PRINT_MUTOO_PARAMETERS",1);
  rc->set_IntFlag("RUNNUMBER", run);
  rc->set_IntFlag("MUONFUN4SIM",1);
  rc->set_IntFlag("SIMULATIONFLAG",2);
  rc->set_IntFlag("EMBEDFLAG",0);
  rc->set_CharFlag("EMBED_MC_TOPNODE","TOP");  // need to use "TOP" to get MC inoformation                                                                                                                                                                                                   

  ///////////////////////////////////////////
  // FVTX+MUTR setup
  //////////////////////////////////////////
  TFvtxGlobalParCntrl::set_bool_par("is_pp",true);
  TFvtxGlobalParCntrl::set_bool_par("use_svx",use_svx);
  TFvtxGlobalParCntrl::set_bool_par("is_sim",true);
  TFvtxGlobalParCntrl::set_bool_par("deadmap_use_calibration_database", true);
  TFvtxGlobalParCntrl::set_bool_par("deadmap_use_production_map_for_sim", true);
  TFvtxGlobalParCntrl::set_bool_par("geom_use_calibration_database", false); 
  TFvtxGlobalParCntrl::set_string_par("geom_root_file_path","./");
	TFvtxGlobalParCntrl::set_string_par("geom_root_file_name", "geom_run15_reco.root");

	TMutDatabaseCntrl::set_database_access("use_local_dead_HV_file",true); //mut.disabledAnodes.dat
	TString hv_file = Form("/gpfs/mnt/gpfs02/phenix/hhj/hhj1/shlim/work/07.genericQA/01.mutr_database/Run15pp200/mut.disabledAnodes_Run15pp200_updated/mut.disabledAnodes.dat_run%d",run);
  TMutDatabaseCntrl::set_filename("use_local_dead_HV_file", hv_file.Data());
	TMutDatabaseCntrl::set_database_access("use_local_dead_wire_file",true); //mut.disabledWires.dat
	TMutDatabaseCntrl::set_database_access("use_local_attenuated_chnl_file",true); //AttenuatedChannels.txt
	TMutDatabaseCntrl::set_database_access("use_local_dead_channel_file",true); //DeadChannels.dat
  TMutDatabaseCntrl::set_database_access("use_local_landau_parameters_file", true);
  TMutDatabaseCntrl::set_filename("use_local_landau_parameters_file","landau_parameters.txt");
	TMutDatabaseCntrl::set_database_access("internal_alignment_corrections", false);
	TMutDatabaseCntrl::set_database_access("global_alignment_corrections", false);

  ///////////////////////////////////////////
  // MUID setup
  //////////////////////////////////////////
	std::string muid_eff_south("muid_tube_eff_south_Run15pp200.txt");
	std::string muid_eff_north("muid_tube_eff_north_Run15pp200.txt");

	{
		std::cout << "using local two pack efficiciency files:" << muid_eff_south << ", "  << muid_eff_north << std::endl;
		TMuiHVMask::set_mode(TMuiHVMask::FROM_FILE);
		TMuiHVMask::set_filename_south(muid_eff_south.c_str());
		TMuiHVMask::set_filename_north(muid_eff_north.c_str());
	}

  TMutExtVtx::get().set_verbosity( MUTOO::SOME );
	TMutExtVtx::get().set_priorities( "SIM", 0 );

  ///////////////////////////////////////////
  // Make the Server
  //////////////////////////////////////////
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(verbosity);

  ///////////////////////////////////////////
  // Subsystems
  //////////////////////////////////////////

  // run header and trigger setting
  se->registerSubsystem( new HeadSimreco() );

  // vertex simulation for BBC z-vertex
  // puts the vertex from the pisa header node into vtxOut object
  VtxSimreco* vtx_reco = new VtxSimreco();
  vtx_reco->SmearZ( false );
  vtx_reco->SmearY( false );
  vtx_reco->SmearX( false );
  vtx_reco->UseXY( false );
  vtx_reco->OverwriteBBC( true );
  vtx_reco->XVertexSigma( 1.0 ); 
  vtx_reco->YVertexSigma( 1.0 );
  vtx_reco->ZVertexSigma( 2.0 );
  se->registerSubsystem( vtx_reco );

  // Counter
  se->registerSubsystem( new MuonCounter() );

  // global detectors subsystem
  se->registerSubsystem( new VtxReco() );

  if(use_svx)
	{

		// SVX reconstruction
		SvxParManager *svxpar = new SvxParManager();
		svxpar->Verbosity(verbosity);
		svxpar->set_UseRefDiffPixelMap(1);

		svxpar->set_OffsetVtxToCnt(0.0, 0.0, 0.0);
		svxpar->set_OffsetEastToWest(0.0, 0.0, 0.0);
		svxpar->set_ReadGeoParFromFile(1);
		svxpar->set_GeometryFileName("svxPISA.par"); 
		svxpar->set_UseStripThresholdDatbase(true);
		se->registerSubsystem(svxpar);

		SvxSimulator *svxsim = new SvxSimulator();
		svxsim->Verbosity(verbosity);
		se->registerSubsystem(svxsim);

		SvxApplyHotDead *svxhotdead = new SvxApplyHotDead();
		svxhotdead->Verbosity(verbosity);
		se->registerSubsystem(svxhotdead);

		SvxReco *svxrec = new SvxReco();
		svxrec->Verbosity(verbosity);
		svxrec->set_ThisIsSimulation();
		svxrec->set_StripixelAdcSumThreshold(0);
		se->registerSubsystem(svxrec);

		SvxPriVertexSeedFinder *svxvtxseedfinder = new SvxPriVertexSeedFinder();
		svxvtxseedfinder->Verbosity(verbosity);
		se->registerSubsystem(svxvtxseedfinder);

		SvxStandAloneReco *svxstandalone = new SvxStandAloneReco();
		svxstandalone->Verbosity(verbosity);
		svxstandalone->setVertexRecoFlag(2);
		svxstandalone->setPPFlag(true);
		se->registerSubsystem(svxstandalone);

		SvxPrimVertexFinder *svxprimvtxfinder = new SvxPrimVertexFinder();
		svxprimvtxfinder->Verbosity(verbosity);
		se->registerSubsystem(svxprimvtxfinder);
	}

  // muon prdf unpacker
  MuonUnpackPisa *muon_unpack_pisa =  new MuonUnpackPisa();
  muon_unpack_pisa->Verbosity(0);
  muon_unpack_pisa->set_flag(MuonUnpackPisa::DO_RESPONSE,1);
  se->registerSubsystem( muon_unpack_pisa );

  // muioo/mutoo reconstruction
  se->registerSubsystem( new MuiooReco() );

  MuonDev *muon_dev = new MuonDev();
  se->registerSubsystem( muon_dev );

  // fvtx prdf unpacker
  if (use_fvtx)
	{
		FvtxUnpackPisa *fvtx_unpack = new FvtxUnpackPisa();
		se->registerSubsystem( fvtx_unpack );

		FvtxReco* fvtxreco = new FvtxReco();
		se->registerSubsystem(fvtxreco);

		FvtxPrimVertex* fvtxprimvtx = new FvtxPrimVertex();
		fvtxprimvtx->set_source(FvtxPrimVertex::Tracks,FvtxPrimVertex::Segments);
		se->registerSubsystem(fvtxprimvtx);

		// Perform FVTX-Mutr track matching and refit track:
		FvtxRecoWithMut *fvtx_reco_withmut = new FvtxRecoWithMut();
		se->registerSubsystem( fvtx_reco_withmut );
	}

	//MC evaluation
  if(use_eval)
	{
		MuonEval* mueval = new MuonEval();
		mueval->set_flags(0); // no ntuple output needed
		se->registerSubsystem (mueval);
	}

  if (use_fvtx && use_eval)
	{
		FvtxEval* fvtxeval = new FvtxEval("FvtxEval","fvtx_eval_pisa.root");
		se->registerSubsystem(fvtxeval);
		FvtxMCEval* fvtxeval_mc = new FvtxMCEval("FvtxMCEval","fvtx_mc_eval_pisa.root");
		se->registerSubsystem(fvtxeval_mc);
	}

	se->registerSubsystem( new GlobalReco() );
	se->registerSubsystem( new GlobalReco_muons() );
  
  
  ///////////////////////////////////////////
  // IOManagers...
  ///////////////////////////////////////////

  // picoDST
  if( write_pdst )
	{
		// MWG
		gSystem->Load("libMWGOO.so");
		PHInclusiveNanoCuts *MWGcuts = new MWGInclusiveNanoCutsv2();
		se->registerSubsystem(new MWGFvtxReco(MWGcuts));

		//single muon container
		mFillSingleMuonContainer* msngl = new mFillSingleMuonContainer();
		msngl->set_is_sim(true);
		se->registerSubsystem(msngl);

		//dimuon container
		mFillDiMuonContainer* mdi = new mFillDiMuonContainer(false);
		mdi->set_is_sim(true);
		mdi->set_is_pp(true);
		mdi->set_mass_cut(1.0);
		se->registerSubsystem(mdi);

		//trigger emulator
		mFillTriggerEmulatorContainer *mtrig_emul = new mFillTriggerEmulatorContainer();
		se->registerSubsystem(mtrig_emul);

		if(fill_mc_info)
		{
			//MC single muon container
			mFillMCSingleMuonFvtxContainer* msngl_mc = new mFillMCSingleMuonFvtxContainer();
			se->registerSubsystem(msngl_mc);

			//MC dimuon container
			mFillMCDiMuonContainer* mdi_mc = new mFillMCDiMuonContainer();
			se->registerSubsystem(mdi_mc);

			//HEPMC container for Pythia8 event
			if(!pythiaFile.empty())
			{
				mFillMCHepMCParticleContainer* gen_event = new mFillMCHepMCParticleContainer();
				gen_event->SetNodeName("PHHepMCGenEvent_SIGNAL");
				se->registerSubsystem(gen_event); 
			}
		}

		//output manager
		Fun4AllOutputManager *outsmu = new Fun4AllDstOutputManager("Outsmu",outputPDST.c_str());
		outsmu->AddNode("SingleMuonContainer");
		outsmu->AddNode("DiMuonContainer");
		if (fill_mc_info) outsmu->AddNode("MCSingleMuonFvtxContainer");
		if (fill_mc_info) outsmu->AddNode("MCDiMuonContainer");
		outsmu->AddNode("Sync");
		outsmu->AddNode("TrigLvl1");
		outsmu->AddNode("VtxOut");
		outsmu->AddNode("PHGlobal");
		outsmu->AddNode("RunHeader");
		outsmu->AddNode("EventHeader");
		outsmu->AddNode("TriggerEmulatorContainer");
		//outsmu->AddEventSelector("mFillDiMuonContainer");
		se->registerOutputManager(outsmu);
	}

	Fun4AllPisaInputManager *inMan = new Fun4AllPisaInputManager("PisaIn");
	se->registerInputManager(inMan);
	TString tmp = pisaFile;
	std::string suffix = ".txt";
	if(tmp.EndsWith(suffix.c_str()))
	{
		int counter = 0;
		std::string theFile;
		ifstream inFile;
		inFile.open(pisaFile.c_str());
		while(inFile >> theFile)
		{   
			inMan->AddFile(theFile.c_str());
		}   
		inFile.close();
	}
	else
	{
		se->fileopen(inMan->Name(),pisaFile.c_str());
	}

	if (!pythiaFile.empty())
	{
		Fun4AllDstInputManager *ipythia = new Fun4AllNoSyncDstInputManager("DSTin","DST");
		se->registerInputManager(ipythia);
		se->fileopen(ipythia->Name(),pythiaFile.c_str());
	}

  se->run(nEvents);
  se->End();

  cout << ">>>> Completed reconstruction." << endl;
}
