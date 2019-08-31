#include <Fun4AllReturnCodes.h>
#include <PHCompositeNode.h>
#include <getClass.h>
#include <MWGConsts.h>
#include <Tools.h>
#include <Fun4AllServer.h>
#include <SyncObject.h>
#include <RunHeader.h>
#include <TrigLvl1.h>
#include <SingleMuon.h>
#include <SingleMuonContainer.h>
#include <DiMuon.h>
#include <DiMuonContainer.h>
#include <VtxOut.h>
#include <PHGlobal.h>
#include <PHTFileServer.h>
#include <PHPoint.h>
#include <TFile.h>
#include <TH3.h>
#include <TTree.h>

#include "mRunCutsDiMuon.h"

using namespace std;

//___________________________________________________________________
mRunCutsDiMuon::mRunCutsDiMuon(const char *name):
  SubsysReco(name)
{

	return;
}

//___________________________________________________________________
mRunCutsDiMuon::~mRunCutsDiMuon()
{

	return;
}

//___________________________________________________________________
int mRunCutsDiMuon::Init(PHCompositeNode *top_node)
{  

	MUTOO::PRINT(std::cout,"mRunCutsDiMuon::Init");

	file_out = new TFile("RunCuts.root","RECREATE");

	book_histos();

	MUTOO::PRINT(std::cout,"**");
  return 0;
}

//______________________________________________________
int mRunCutsDiMuon::InitRun(PHCompositeNode *top_node)
{
	MUTOO::PRINT(std::cout,"mRunCutsDiMuon::InitRun");

	MUTOO::PRINT(std::cout,"**");
	return 0;
}

//______________________________________________________
int mRunCutsDiMuon::process_event(PHCompositeNode *top_node)
{

	// get nodes
	DiMuonContainer *dimucon = findNode::getClass<DiMuonContainer>(top_node,"DiMuonContainer");
	if (!dimucon){
		cout << PHWHERE << "mFillAnaTree_DiMuon:: DiMuonContainer not in Node Tree" << endl;
		return ABORTRUN;
	}

	TrigLvl1 *triglvl1 = findNode::getClass<TrigLvl1>(top_node,"TrigLvl1");
	if (!triglvl1){
		std::cout << PHWHERE << "mRunCutsDiMuon:: TrigLvl1 not in Node Tree" << std::endl;
		return ABORTRUN;
	}

	VtxOut *vtx = findNode::getClass<VtxOut>(top_node,"VtxOut");
	if (!vtx){
		std::cout << PHWHERE << "mRunCutsDiMuon:: VtxOut not in Node Tree" << std::endl;
		return ABORTRUN;
	}

	// event selection
	// z-vertex selection
	float bbcZ = vtx->get_Vertex("BBC").getZ(); 
	if ( !(fabs(bbcZ)<30.0) ) return DISCARDEVENT;

	// trigger selection
	unsigned int trigbit_scaled = triglvl1->get_lvl1_trigscaled();
	bool scaled_MUIDN2D = trigbit_scaled&0x00100000;
	bool scaled_MUIDS2D = trigbit_scaled&0x00200000;
	if ( !(scaled_MUIDN2D || scaled_MUIDS2D) ) return DISCARDEVENT;

	// dimuon selection
	int ndimuon = dimucon->get_nDiMuons();

	for (int imu=0; imu<ndimuon; imu++){
		DiMuon* dimu = dimucon->get_DiMuon(imu);

		// dimuon info
		bool same_event = dimu->get_same_event();
		short charge = dimu->get_charge();
		float mass = dimu->get_mass();
		float rapidity = dimu->get_rapidity();
		float pT = dimu->get_pT();

		// dimuon cut
		if ( !same_event ) continue;
		if ( fabs(rapidity)<1.2 || fabs(rapidity)>2.2 ) continue;
		if ( mass<2.0 || mass>5.0 ) continue;
		if ( pT>10.0 ) continue;

		// single muon info
		short tr0_lastgap = dimu->get_Tr0_lastgap();
		float tr0_trchi2 = dimu->get_Tr0_trchi2();
		float tr0_DG0 = dimu->get_Tr0_DG0();
		float tr0_DDG0 = dimu->get_Tr0_DDG0();
		float tr0_pz = dimu->get_Tr0_pz();

		short tr1_lastgap = dimu->get_Tr1_lastgap();
		float tr1_trchi2 = dimu->get_Tr1_trchi2();
		float tr1_DG0 = dimu->get_Tr1_DG0();
		float tr1_DDG0 = dimu->get_Tr1_DDG0();
		float tr1_pz = dimu->get_Tr1_pz();

		// single muon cut
		if ( tr0_lastgap<3 || tr1_lastgap<3 ) continue;
		if ( fabs(tr0_pz)<2.5 || fabs(tr1_pz)<2.5 ) continue;
		if ( tr0_trchi2>20.0 || tr1_trchi2>20.0 ) continue;
		if ( tr0_DDG0>60.0 || tr1_DDG0>60.0 ) continue;
		if ( tr0_DG0>80.0 || tr1_DG0>80.0 ) continue;

		// fill histogram
		int index_arm = rapidity<0 ? 0 : 1;
		int index_charge = 0;
		if ( charge<0 ) index_charge = 1;
		else if ( charge>0 ) index_charge = 2;

		h3d_mass_pT_rapidity[index_arm][index_charge]->Fill(mass, pT, fabs(rapidity));

	}


	return 0;
}

//______________________________________________________
int mRunCutsDiMuon::End(PHCompositeNode *top_node)
{
  MUTOO::PRINT(std::cout,"mRunCutsDiMuon::End");

	file_out->cd();
	file_out->Write();
	//file_out->Close();

	delete file_out;

	MUTOO::PRINT(std::cout,"**");
  return 0;
}

//______________________________________________________
void mRunCutsDiMuon::book_histos()
{

  MUTOO::PRINT(std::cout,"mRunCutsDiMuon::book_histos");

	for (int iarm=0; iarm<2; iarm++){
		for (int ichg=0; ichg<3; ichg++){

			h3d_mass_pT_rapidity[iarm][ichg] = new TH3D(Form("h3d_mass_pT_rapidity_arm%d_chg%d",iarm,ichg),"",60,2.0,5.0,50,0,10,10,1.2,2.2);

		}//ichg
	}//iarm

	return;

}
