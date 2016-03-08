#include <gtest/gtest.h>

#include "Snap.h"

TEST(TMMNet, DefaultConstructor) {
  PMMNet Net;
  Net = TMMNet::New();
}

TEST(TMMNet, AddMode) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddMode(ModeName);
}

TEST(TMMNet, AddLinkType) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  Net->AddMode(ModeName1);
  TStr ModeName2("TestMode2");
  Net->AddMode(ModeName2);
  TStr LinkName("TestLink");
  Net->AddLinkType(ModeName1, ModeName2, LinkName);
}

TEST(TMMNet, GetModeId) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  Net->AddMode(ModeName1);
  TStr ModeName2("TestMode2");
  Net->AddMode(ModeName2);
  TInt ModeId1(0);
  TInt ModeId2(1);
  EXPECT_EQ(ModeId1, Net->GetModeId(ModeName1));
  EXPECT_EQ(ModeId2, Net->GetModeId(ModeName2));
}

TEST(TMMNet, GetModeName) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  Net->AddMode(ModeName1);
  TStr ModeName2("TestMode2");
  Net->AddMode(ModeName2);
  TInt ModeId1(0);
  TInt ModeId2(1);
  EXPECT_EQ(ModeName1, Net->GetModeName(ModeId1));
  EXPECT_EQ(ModeName2, Net->GetModeName(ModeId2));
}

TEST(TMMNet, GetLinkId) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  Net->AddMode(ModeName1);
  TStr ModeName2("TestMode2");
  Net->AddMode(ModeName2);
  TInt LinkId1(0);
  TInt LinkId2(1);
  TStr LinkName1("TestLink1");
  Net->AddLinkType(ModeName1, ModeName2, LinkName1);
  TStr LinkName2("TestLink2");
  Net->AddLinkType(ModeName1, ModeName2, LinkName2);
  EXPECT_EQ(LinkId1, Net->GetLinkId(LinkName1));
  EXPECT_EQ(LinkId2, Net->GetLinkId(LinkName2));
}

TEST(TMMNet, GetLinkName) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  Net->AddMode(ModeName1);
  TStr ModeName2("TestMode2");
  Net->AddMode(ModeName2);
  TInt LinkId1(0);
  TInt LinkId2(1);
  TStr LinkName1("TestLink1");
  Net->AddLinkType(ModeName1, ModeName2, LinkName1);
  TStr LinkName2("TestLink2");
  Net->AddLinkType(ModeName1, ModeName2, LinkName2);
  EXPECT_EQ(LinkName1, Net->GetLinkName(LinkId1));
  EXPECT_EQ(LinkName2, Net->GetLinkName(LinkId2));
}

TEST(TMMNet, GetModeNet) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  TInt ModeId1(0);
  Net->AddMode(ModeName1);
  TModeNet& M1 = Net->GetModeNet(ModeName1);
  TModeNet& M2 = Net->GetModeNet(ModeId1);
  EXPECT_EQ(0, M2.GetNodes());
  M1.AddNode(0);
  EXPECT_EQ(1, M2.GetNodes());
}

TEST(TMMNet, GetCrossNet) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  Net->AddMode(ModeName1);
  TStr ModeName2("TestMode2");
  Net->AddMode(ModeName2);
  TInt LinkId1(0);
  TInt LinkId2(1);
  TStr LinkName1("TestLink1");
  Net->AddLinkType(ModeName1, ModeName2, LinkName1);
  TCrossNet& C1 = Net->GetCrossNet(LinkName1);
  TCrossNet& C2 = Net->GetCrossNet(LinkId1);
  EXPECT_EQ(&C1, &C2);
}

TEST(TMMNet, BegModeNetI) {
	PMMNet Net;
  	Net = TMMNet::New();
  	for(int i = 0; i < 10; i++) {
  		TInt Id(i);
  		Net->AddMode(Id.GetStr());
  	}
  	TMMNet::TModeNetI MI = Net->BegModeNetI();
  	EXPECT_EQ(0, MI.GetModeId());
  	TModeNet& M0 = MI.GetModeNet();
  	TModeNet& M0Exp = Net->GetModeNet(TStr("0"));
  	EXPECT_EQ(&M0Exp, &M0);
}

TEST(TMMNet, GetModeNetI) {
	PMMNet Net;
  	Net = TMMNet::New();
  	for(int i = 0; i < 10; i++) {
  		TInt Id(i);
  		Net->AddMode(Id.GetStr());
  	}
  	for(int i = 0; i < 10; i++) {
  		TMMNet::TModeNetI MI = Net->GetModeNetI(i);
  		EXPECT_EQ(i, MI.GetModeId());
  	}
}

TEST(TMMNet, EndModeNetI) {
	PMMNet Net;
  	Net = TMMNet::New();
  	for(int i = 0; i < 10; i++) {
  		TInt Id(i);
  		Net->AddMode(Id.GetStr());
  	}
  	TMMNet::TModeNetI MI = Net->EndModeNetI();
  	for(int i = 0; i < 10; i++) {
  		TMMNet::TModeNetI MOther = Net->GetModeNetI(i);
  		EXPECT_NE(MI, MOther);
  	}
}

TEST(TMMNet, ModeNetI) {
	PMMNet Net;
  	Net = TMMNet::New();
  	for(int i = 0; i < 10; i++) {
  		TInt Id(i);
  		Net->AddMode(Id.GetStr());
  	}
  	int i = 0;
  	TMMNet::TModeNetI MI;
  	for(i = 0, MI = Net->BegModeNetI(); i < 10 && MI < Net->EndModeNetI(); i++, MI++) {
  		EXPECT_EQ(i, MI.GetModeId());
  	}
  	EXPECT_EQ(10, i);
}

TEST(TMMNet, BegCrossNetI) {
	PMMNet Net;
  	Net = TMMNet::New();
  	Net->AddMode("0");
  	for(int i = 1; i < 11; i++) {
  		TInt MPrev(i-1);
  		TInt MId(i);
  		TInt LId(i-1);
  		Net->AddMode(MId.GetStr());
  		Net->AddMode(MPrev.GetStr());
  		Net->AddLinkType(MPrev, MId, LId.GetStr());
  	}
  	TMMNet::TCrossNetI CI = Net->BegCrossNetI();
  	EXPECT_EQ(0, CI.GetCrossId());
  	TCrossNet& C0 = CI.GetCrossNet();
  	TCrossNet& C0Exp = Net->GetCrossNet(TStr("0"));
  	EXPECT_EQ(&C0Exp, &C0);
}

TEST(TMMNet, GetCrossNetI) {
	PMMNet Net;
  	Net = TMMNet::New();
  	Net->AddMode("0");
  	for(int i = 1; i < 11; i++) {
  		TInt MPrev(i-1);
  		TInt MId(i);
  		TInt LId(i-1);
  		Net->AddMode(MId.GetStr());
  		Net->AddMode(MPrev.GetStr());
  		Net->AddLinkType(MPrev, MId, LId.GetStr());
  	}
  	for(int i = 0; i < 10; i++) {
  		TMMNet::TCrossNetI CI = Net->GetCrossNetI(i);
  		EXPECT_EQ(i, CI.GetCrossId());
  	}
}

TEST(TMMNet, EndCrossNetI) {
	PMMNet Net;
  	Net = TMMNet::New();
  	Net->AddMode("0");
  	for(int i = 1; i < 11; i++) {
  		TInt MPrev(i-1);
  		TInt MId(i);
  		TInt LId(i-1);
  		Net->AddMode(MId.GetStr());
  		Net->AddMode(MPrev.GetStr());
  		Net->AddLinkType(MPrev, MId, LId.GetStr());
  	}
  	TMMNet::TCrossNetI CI = Net->EndCrossNetI();
  	for(int i = 0; i < 10; i++) {
  		TMMNet::TCrossNetI COther = Net->GetCrossNetI(i);
  		EXPECT_NE(CI, COther);
  	}
}

TEST(TMMNet, CrossNetI) {
	PMMNet Net;
  	Net = TMMNet::New();
  	Net->AddMode("0");
  	for(int i = 1; i < 11; i++) {
  		TInt MPrev(i-1);
  		TInt MId(i);
  		TInt LId(i-1);
  		Net->AddMode(MId.GetStr());
  		Net->AddMode(MPrev.GetStr());
  		Net->AddLinkType(MPrev, MId, LId.GetStr());
  	}
  	int i = 0;
  	TMMNet::TCrossNetI CI;
  	for(i = 0, CI = Net->BegCrossNetI(); i < 10 && CI < Net->EndCrossNetI(); i++, CI++) {
  		EXPECT_EQ(i, CI.GetCrossId());
  	}
  	EXPECT_EQ(10, i);
}