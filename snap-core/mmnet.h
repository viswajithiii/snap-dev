#ifndef MMNET_H
#define MMNET_H


class TMMNet;


typedef TPt<TMMNet> PMMNet;


///A single mode in a multimodal directed attributed multigraph
class TModeNet;

///A single cross net in a multimodal directed attributed multigraph
class TCrossNet;


//#//////////////////////////////////////////////
/// The nodes of one particular mode in a TMMNet, and their neighbor vectors as TIntV attributes ##TModeNet::Class
class TModeNet : public TNEANet {
public:
  typedef TModeNet TNetMM;
public:
  /// Node iterator. Only forward iteration (operator++) is supported.
  class TNodeI : public TNEANet::TNodeI {
  public:
    TNodeI() : TNEANet::TNodeI() { }
    TNodeI(const THashIter& NodeHIter, const TModeNet* GraphPt) : TNEANet::TNodeI(NodeHIter, GraphPt) { }
    TNodeI(const TNodeI& NodeI) : TNEANet::TNodeI(NodeI) { }
    TNodeI& operator = (const TNodeI& NodeI) { NodeHI = NodeI.NodeHI; Graph=NodeI.Graph; return *this; }
    /// Increments the iterator.
    TNodeI& operator++ (int) { NodeHI++; return *this; }
    bool operator < (const TNodeI& NodeI) const { return NodeHI < NodeI.NodeHI; }
    bool operator == (const TNodeI& NodeI) const { return NodeHI == NodeI.NodeHI; }
    void GetNeighborsByLinkType(TStr& Name, TIntV& Neighbors, const bool isOutEId=false) {
    		const TModeNet *TMGraph = static_cast<const TModeNet *>(Graph); TMGraph->GetNeighborsByLinkType(GetId(), Name, Neighbors); }
    void GetLinkTypeNames(TStrV& Names) { const TModeNet *TMGraph = static_cast<const TModeNet *>(Graph); TMGraph->GetLinkTypeNames(Names); }
    friend class TModeNet;
  };
private:
  TInt NModeId;
  TMMNet *MMNet; ///< A pointer to the parent MMNet
  THash<TStr, TBool> NeighborTypes;

public:
  //TODO: Update constructors with information from fields above.
  TModeNet() : TNEANet(), NModeId(-1), MMNet(), NeighborTypes() { }
  TModeNet(const int& TypeId) : TNEANet(), NModeId(TypeId), MMNet(), NeighborTypes() { }
  /// Constructor that reserves enough memory for a graph of nodes and edges.
  explicit TModeNet(const int& Nodes, const int& Edges) : TNEANet(Nodes, Edges),
    NModeId(-1), MMNet(), NeighborTypes(){ }
  explicit TModeNet(const int& Nodes, const int& Edges, const int& TypeId) : TNEANet(Nodes, Edges),
    NModeId(TypeId), MMNet(), NeighborTypes() { }
  TModeNet(const TModeNet& Graph) :  TNEANet(Graph), NModeId(Graph.NModeId), MMNet(Graph.MMNet), NeighborTypes(Graph.NeighborTypes) { }
  /// Constructor for loading the graph from a (binary) stream SIn.
  TModeNet(TSIn& SIn) : TNEANet(SIn), NModeId(SIn), MMNet(), NeighborTypes(SIn) { }
private:
  TModeNet(const TModeNet& Graph, bool isSubModeGraph) : TNEANet(Graph, isSubModeGraph), NModeId(Graph.NModeId), MMNet(), NeighborTypes() {}
public:


  void GetPartitionRanges(TIntPrV& Partitions, TInt NumPartitions) const ;

  size_t GetMemUsed() const {return sizeof(TMMNet *) + NeighborTypes.GetMemUsed() + NModeId.GetMemUsed() + TNEANet::GetMemUsed(); }
  /// Saves the graph to a (binary) stream SOut.
  void Save(TSOut& SOut) const {
    TNEANet::Save(SOut); NModeId.Save(SOut); NeighborTypes.Save(SOut); }

  int DelNode (const int& NId); //TODO(sramas15): finish implementing
  void GetLinkTypeNames(TStrV& Names) const { NeighborTypes.GetKeyV(Names); }
  void GetNeighborsByLinkType(const int& NId, TStr& Name, TIntV& Neighbors, const bool isOutEId=false) const;

  /// Returns an iterator referring to the first node in the graph.
  TNodeI BegMMNI() const { return TNodeI(NodeH.BegI(), this); }
  /// Returns an iterator referring to the past-the-end node in the graph.
  TNodeI EndMMNI() const { return TNodeI(NodeH.EndI(), this); }
  /// Returns an iterator referring to the node of ID NId in the graph.
  TNodeI GetMMNI(const int& NId) const { return TNodeI(NodeH.GetI(NId), this); }


private:
  ///method to add neighbors; will be called by TMMNet AddEdge function; outEdge == true iff NId(which is of the type of the TModeNet; i.e. it should refer to a node in this graph) is the source node.
  int AddNeighbor(const int& NId, const int& EId, const bool outEdge, const int linkId, const bool sameMode, bool isDir);
  int AddNeighbor(const int& NId, const int& EId, const bool outEdge, const TStr& linkName, const bool sameMode, bool isDir);
  int DelNeighbor(const int& NId, const int& EId, bool outEdge, const TStr& LinkName, const bool sameMode, bool isDir);
  int DelNeighbor(const int& NId, const int& EId, bool outEdge, const TInt& linkId, const bool sameMode, bool isDir);
  TStr GetNeighborLinkName(const TStr& LinkName, bool isOutEdge, const bool sameMode, bool isDir) const;
  void SetParentPointer(TMMNet* parent);
  int AddNbrType(const TStr& LinkName, const bool sameMode, bool isDir);
  int AddNbrType(const TStr& LinkName, const bool sameMode, bool isDir, TVec<TIntV>& Neighbors);
  /// Adds a new IntV node attribute to the hashmap.
  int AddIntVAttrByVecN(const TStr& attr, TVec<TIntV>& Attrs);
  void RemoveLinkTypes(TModeNet& Result, TStrV& LinkTypes);
  int GetAttrTypeN(const TStr& attr) const;
public:

  ///When we create a new link type, we need to add a new neighbor type here.

  /// Deletes all nodes and edges from the graph.
  void Clr() { TNEANet::Clr(); NModeId = -1; MMNet = NULL; NeighborTypes.Clr(); }

  friend class TMMNet;
  friend class TCrossNet;
};


//TODO: Rename source and destination so as to make it clear that it can be directed or undirected

//#///////////////////////////////////////////////
///Implements a single CrossNet consisting of edges between two TModeNets (could be the same TModeNet) ##TCrossNet::Class
class TCrossNet {

public:
  //#///////////////////////////////////////////////
  /// A single edge in the cross net. Has an Edge Id, and the source and destination node ids. (Mode ids are implicit from TCrossNet)
  class TCrossEdge {
  private:
    TInt EId;
    TInt SrcNId, DstNId;
  public:
    TCrossEdge() : EId(-1), SrcNId(-1), DstNId(-1) { }
    TCrossEdge(const int& Id, const int& SourceNId, const int& DestNId) :
      EId(Id), SrcNId(SourceNId), DstNId(DestNId) { }
    TCrossEdge(const TCrossEdge& MultiEdge) : EId(MultiEdge.EId), SrcNId(MultiEdge.SrcNId),
        DstNId(MultiEdge.DstNId) { }
    TCrossEdge(TSIn& SIn) : EId(SIn), SrcNId(SIn), DstNId(SIn) { }
    void Save(TSOut& SOut) const { EId.Save(SOut); SrcNId.Save(SOut); DstNId.Save(SOut); }
    int GetId() const { return EId; }
    int GetSrcNId() const { return SrcNId; }
    int GetDstNId() const { return DstNId; }
    size_t GetMemUsed() const { return EId.GetMemUsed() + SrcNId.GetMemUsed() + DstNId.GetMemUsed(); }
    friend class TCrossNet;
  };
   /// Edge iterator. Only forward iteration (operator++) is supported.
  class TCrossEdgeI {
  private:
    typedef THash<TInt, TCrossEdge>::TIter THashIter;
    THashIter LinkHI;
    const TCrossNet *Graph;
  public:
    TCrossEdgeI() : LinkHI(), Graph(NULL) { }
    TCrossEdgeI(const THashIter& LinkHIter, const TCrossNet *GraphPt) : LinkHI(LinkHIter), Graph(GraphPt) { }
    TCrossEdgeI(const TCrossEdgeI& EdgeI) : LinkHI(EdgeI.LinkHI), Graph(EdgeI.Graph) { }
    TCrossEdgeI& operator = (const TCrossEdgeI& EdgeI) { if (this!=&EdgeI) { LinkHI=EdgeI.LinkHI; Graph=EdgeI.Graph; }  return *this; }
    /// Increment iterator.
    TCrossEdgeI& operator++ (int) { LinkHI++; return *this; }
    bool operator < (const TCrossEdgeI& EdgeI) const { return LinkHI < EdgeI.LinkHI; }
    bool operator == (const TCrossEdgeI& EdgeI) const { return LinkHI == EdgeI.LinkHI; }
    /// Returns edge ID.
    int GetId() const { return LinkHI.GetDat().GetId(); }
    /// Returns the source of the edge.
    int GetSrcNId() const { return LinkHI.GetDat().GetSrcNId(); }
    /// Returns the destination of the edge.
    int GetDstNId() const { return LinkHI.GetDat().GetDstNId(); }

    /// Returns the source mode of the crossnet
    int GetSrcModeId() const { return Graph->GetMode1(); }
    /// Returns the destination mode of the crossnet
    int GetDstModeId() const { return Graph->GetMode2(); }
    /// Returns whether the edge is directed.
    int IsDirected() const { return Graph->IsDirected(); } //TODO


    friend class TCrossNet;
  };

  /// Node/edge integer attribute iterator. Iterates through all nodes/edges for one integer attribute.
  class TAIntI {
  private:
    typedef TIntV::TIter TIntVecIter;
    TIntVecIter HI;
    TStr attr;
    const TCrossNet *Graph;
  public:
    TAIntI() : HI(), attr(), Graph(NULL) { }
    TAIntI(const TIntVecIter& HIter, TStr attribute, const TCrossNet* GraphPt) : HI(HIter), attr(), Graph(GraphPt) { attr = attribute; }
    TAIntI(const TAIntI& I) : HI(I.HI), attr(I.attr), Graph(I.Graph) { }
    TAIntI& operator = (const TAIntI& I) { HI = I.HI; Graph=I.Graph; attr = I.attr; return *this; }
    bool operator < (const TAIntI& I) const { return HI < I.HI; }
    bool operator == (const TAIntI& I) const { return HI == I.HI; }
    /// Returns an attribute of the node.
    TInt GetDat() const { return HI[0]; }
    /// Returns true if the attribute has been deleted.
    bool IsDeleted() const { return GetDat() == Graph->GetIntAttrDefaultE(attr); };
    TAIntI& operator++(int) { HI++; return *this; }
    friend class TCrossNet;
  };

  /// Node/edge string attribute iterator. Iterates through all nodes/edges for one string attribute.
  class TAStrI {
  private:
    typedef TStrV::TIter TStrVecIter;
    TStrVecIter HI;
    TStr attr;
    const TCrossNet *Graph;
  public:
    TAStrI() : HI(), attr(), Graph(NULL) { }
    TAStrI(const TStrVecIter& HIter, TStr attribute, const TCrossNet* GraphPt) : HI(HIter), attr(), Graph(GraphPt) { attr = attribute; }
    TAStrI(const TAStrI& I) : HI(I.HI), attr(I.attr), Graph(I.Graph) { }
    TAStrI& operator = (const TAStrI& I) { HI = I.HI; Graph=I.Graph; attr = I.attr; return *this; }
    bool operator < (const TAStrI& I) const { return HI < I.HI; }
    bool operator == (const TAStrI& I) const { return HI == I.HI; }
    /// Returns an attribute of the node.
    TStr GetDat() const { return HI[0]; }
    /// Returns true if the attribute has been deleted.
    bool IsDeleted() const { return GetDat() == Graph->GetStrAttrDefaultE(attr); };
    TAStrI& operator++(int) { HI++; return *this; }
    friend class TCrossNet;
  };

  /// Node/edge float attribute iterator. Iterates through all nodes/edges for one float attribute.
  class TAFltI {
  private:
    typedef TFltV::TIter TFltVecIter;
    TFltVecIter HI;
    TStr attr;
    const TCrossNet *Graph;
  public:
    TAFltI() : HI(), attr(), Graph(NULL) { }
    TAFltI(const TFltVecIter& HIter, TStr attribute, const TCrossNet* GraphPt) : HI(HIter), attr(), Graph(GraphPt) { attr = attribute; }
    TAFltI(const TAFltI& I) : HI(I.HI), attr(I.attr), Graph(I.Graph) { }
    TAFltI& operator = (const TAFltI& I) { HI = I.HI; Graph=I.Graph; attr = I.attr; return *this; }
    bool operator < (const TAFltI& I) const { return HI < I.HI; }
    bool operator == (const TAFltI& I) const { return HI == I.HI; }
    /// Returns an attribute of the node.
    TFlt GetDat() const { return HI[0]; }
    /// Returns true if the attribute has been deleted.
    bool IsDeleted() const { return GetDat() == Graph->GetFltAttrDefaultE(attr); };
    TAFltI& operator++(int) { HI++; return *this; }
    friend class TCrossNet;
  };
public:
  TCRef CRef; //Reference counter. Necessary for pointers. //TODO: This is not necessary, right?
private:
  THash<TInt,TCrossEdge> LinkH; ///< The HashTable from Edge id to the corresponding Edge
  TInt MxEId;
  TInt Mode1; ///<The first mode. In the case of directed crossnets, this is implicitly understood to be the source mode.
  TInt Mode2; ///<The second mode. In the case of directed crossnets, this is implicitly understood to be the destination mode.
  TBool IsDirect;
  TInt LinkTypeId;
  TMMNet* Net;
  TStrIntPrH KeyToIndexTypeE;
  THash<TStr, TInt> IntDefaultsE;
  THash<TStr, TStr> StrDefaultsE;
  THash<TStr, TFlt> FltDefaultsE;
  TVec<TIntV> VecOfIntVecsE;
  TVec<TStrV> VecOfStrVecsE;
  TVec<TFltV> VecOfFltVecsE;
  enum { IntType, StrType, FltType };
  //Constructors
public:
  TCrossNet() : CRef(), LinkH(), MxEId(0), Mode1(-1), Mode2(-1), IsDirect(), LinkTypeId(), Net(), KeyToIndexTypeE(), IntDefaultsE(), StrDefaultsE(),
    FltDefaultsE(), VecOfIntVecsE(), VecOfStrVecsE(), VecOfFltVecsE() {}
  TCrossNet(TInt MId1, TInt MId2, TInt LId) : LinkH(), MxEId(0), Mode1(MId1), Mode2(MId2), IsDirect(true),LinkTypeId(LId), Net(),
    KeyToIndexTypeE(), IntDefaultsE(), StrDefaultsE(), FltDefaultsE(), VecOfIntVecsE(), VecOfStrVecsE(), VecOfFltVecsE() {}
  TCrossNet(TInt MId1, TInt MId2, TBool IsDir, TInt LId) : LinkH(), MxEId(0), Mode1(MId1), Mode2(MId2), IsDirect(IsDir),LinkTypeId(LId), Net(),
    KeyToIndexTypeE(), IntDefaultsE(), StrDefaultsE(), FltDefaultsE(), VecOfIntVecsE(), VecOfStrVecsE(), VecOfFltVecsE() {}
  TCrossNet(TSIn& SIn) : LinkH(SIn), MxEId(SIn), Mode1(SIn), Mode2(SIn), IsDirect(SIn), LinkTypeId(SIn), Net(),
    KeyToIndexTypeE(SIn), IntDefaultsE(SIn), StrDefaultsE(SIn), FltDefaultsE(SIn), VecOfIntVecsE(SIn), VecOfStrVecsE(SIn), VecOfFltVecsE(SIn) {}
  TCrossNet(const TCrossNet& OtherTCrossNet) : LinkH(OtherTCrossNet.LinkH), MxEId(OtherTCrossNet.MxEId), Mode1(OtherTCrossNet.Mode1),
    Mode2(OtherTCrossNet.Mode2), IsDirect(OtherTCrossNet.IsDirect), LinkTypeId(OtherTCrossNet.LinkTypeId),Net(OtherTCrossNet.Net), KeyToIndexTypeE(OtherTCrossNet.KeyToIndexTypeE), 
    IntDefaultsE(OtherTCrossNet.IntDefaultsE), StrDefaultsE(OtherTCrossNet.StrDefaultsE), FltDefaultsE(OtherTCrossNet.FltDefaultsE), VecOfIntVecsE(OtherTCrossNet.VecOfIntVecsE),
    VecOfStrVecsE(OtherTCrossNet.VecOfStrVecsE), VecOfFltVecsE(OtherTCrossNet.VecOfFltVecsE) {}

  TCrossNet& operator=(const TCrossNet& OtherTCrossNet) {
    LinkH = OtherTCrossNet.LinkH;
    MxEId = OtherTCrossNet.MxEId;
    Mode1 = OtherTCrossNet.Mode1;
    Mode2 = OtherTCrossNet.Mode2;
    LinkTypeId = OtherTCrossNet.LinkTypeId;
    IsDirect = OtherTCrossNet.IsDirect;
    Net = OtherTCrossNet.Net;
    return *this;
  }

private:
  void SetParentPointer(TMMNet* parent);
 /// Get Int edge attribute val.  If not a proper attr, return default.
  TInt GetIntAttrDefaultE(const TStr& attribute) const { return IntDefaultsE.IsKey(attribute) ? IntDefaultsE.GetDat(attribute) : (TInt) TInt::Mn; }
  /// Get Str edge attribute val.  If not a proper attr, return default.
  TStr GetStrAttrDefaultE(const TStr& attribute) const { return StrDefaultsE.IsKey(attribute) ? StrDefaultsE.GetDat(attribute) : (TStr) TStr::GetNullStr(); }
  /// Get Flt edge attribute val.  If not a proper attr, return default.
  TFlt GetFltAttrDefaultE(const TStr& attribute) const { return FltDefaultsE.IsKey(attribute) ? FltDefaultsE.GetDat(attribute) : (TFlt) TFlt::Mn; }
  int GetAttrTypeE(const TStr& attr) const;
public:
  /// Tests whether an edge with edge ID EId exists in the graph.
  bool IsEdge(const int& EId) const { return LinkH.IsKey(EId); }

  void GetPartitionRanges(TIntPrV& Partitions, TInt NumPartitions) const ;
  int GetMxEId() const { return MxEId; }
  /// Returns the number of edges in the graph.
  int GetEdges() const { return LinkH.Len(); }

  /// Returns the TCrossEdge object corresponding to edge id EId
  TCrossEdge GetCrossEdge(const int& EId) { return LinkH.GetDat(EId);}
  int AddEdge(const int& sourceNId, const int& destNId, int EId=-1);
  TCrossEdgeI GetEdgeI(const int& EId) const { return TCrossEdgeI(LinkH.GetI(EId), this); }
  TCrossEdgeI BegEdgeI() const { return TCrossEdgeI(LinkH.BegI(), this); }
  TCrossEdgeI EndEdgeI() const { return TCrossEdgeI(LinkH.EndI(), this); }
  int DelEdge(const int& EId);
  int GetMode1() const { return Mode1; }
  int GetMode2() const {return Mode2; }
  void Save(TSOut& SOut) const { LinkH.Save(SOut); MxEId.Save(SOut); Mode1.Save(SOut); Mode2.Save(SOut); IsDirect.Save(SOut); LinkTypeId.Save(SOut); 
    KeyToIndexTypeE.Save(SOut); IntDefaultsE.Save(SOut); StrDefaultsE.Save(SOut); FltDefaultsE.Save(SOut); VecOfIntVecsE.Save(SOut);
    VecOfStrVecsE.Save(SOut); VecOfFltVecsE.Save(SOut); }

  bool IsDirected() const { return IsDirect;}
  /// Returns a vector of attr names for edge EId.
  void AttrNameEI(const TInt& EId, TStrV& Names) const {
    AttrNameEI(EId, KeyToIndexTypeE.BegI(), Names);}
  void AttrNameEI(const TInt& EId, TStrIntPrH::TIter LinkHI, TStrV& Names) const;
  /// Returns a vector of attr values for edge EId.
  void AttrValueEI(const TInt& EId, TStrV& Values) const {
    AttrValueEI(EId, KeyToIndexTypeE.BegI(), Values);}
  void AttrValueEI(const TInt& EId, TStrIntPrH::TIter LinkHI, TStrV& Values) const;
  /// Returns a vector of int attr names for edge EId.
  void IntAttrNameEI(const TInt& EId, TStrV& Names) const {
    IntAttrNameEI(EId, KeyToIndexTypeE.BegI(), Names);}
  void IntAttrNameEI(const TInt& EId, TStrIntPrH::TIter LinkHI, TStrV& Names) const;
  /// Returns a vector of attr values for edge EId.
  void IntAttrValueEI(const TInt& EId, TIntV& Values) const {
    IntAttrValueEI(EId, KeyToIndexTypeE.BegI(), Values);}
  void IntAttrValueEI(const TInt& EId, TStrIntPrH::TIter LinkHI, TIntV& Values) const;


  /// Returns a vector of str attr names for node NId.
  void StrAttrNameEI(const TInt& EId, TStrV& Names) const {
    StrAttrNameEI(EId, KeyToIndexTypeE.BegI(), Names);}
  void StrAttrNameEI(const TInt& EId, TStrIntPrH::TIter LinkHI, TStrV& Names) const;
  /// Returns a vector of attr values for node NId.
  void StrAttrValueEI(const TInt& EId, TStrV& Values) const {
    StrAttrValueEI(EId, KeyToIndexTypeE.BegI(), Values);}
  void StrAttrValueEI(const TInt& EId, TStrIntPrH::TIter LinkHI, TStrV& Values) const;
  /// Returns a vector of int attr names for node NId.
  void FltAttrNameEI(const TInt& EId, TStrV& Names) const {
    FltAttrNameEI(EId, KeyToIndexTypeE.BegI(), Names);}
  void FltAttrNameEI(const TInt& EId, TStrIntPrH::TIter LinkHI, TStrV& Names) const;
  /// Returns a vector of attr values for node NId.
  void FltAttrValueEI(const TInt& EId, TFltV& Values) const {
    FltAttrValueEI(EId, KeyToIndexTypeE.BegI(), Values);}
  void FltAttrValueEI(const TInt& EId, TStrIntPrH::TIter LinkHI, TFltV& Values) const;


  int AddIntAttrDatE(const TCrossEdgeI& EdgeI, const TInt& value, const TStr& attr) { return AddIntAttrDatE(EdgeI.GetId(), value, attr); }
  int AddIntAttrDatE(const int& EId, const TInt& value, const TStr& attr);
  /// Attribute based add function for attr to Str value.
  int AddStrAttrDatE(const TCrossEdgeI& EdgeI, const TStr& value, const TStr& attr) { return AddStrAttrDatE(EdgeI.GetId(), value, attr); }
  int AddStrAttrDatE(const int& EId, const TStr& value, const TStr& attr);
  /// Attribute based add function for attr to Flt value.
  int AddFltAttrDatE(const TCrossEdgeI& EdgeI, const TFlt& value, const TStr& attr) { return AddFltAttrDatE(EdgeI.GetId(), value, attr); }
  int AddFltAttrDatE(const int& EId, const TFlt& value, const TStr& attr);

  /// Gets the value of int attr from the edge attr value vector.
  TInt GetIntAttrDatE(const TCrossEdgeI& EdgeI, const TStr& attr) { return GetIntAttrDatE(EdgeI.GetId(), attr); }
  TInt GetIntAttrDatE(const int& EId, const TStr& attr);
  /// Gets the value of str attr from the edge attr value vector.
  TStr GetStrAttrDatE(const TCrossEdgeI& EdgeI, const TStr& attr) { return GetStrAttrDatE(EdgeI.GetId(), attr); }
  TStr GetStrAttrDatE(const int& EId, const TStr& attr);
  /// Gets the value of flt attr from the edge attr value vector.
  TFlt GetFltAttrDatE(const TCrossEdgeI& EdgeI, const TStr& attr) { return GetFltAttrDatE(EdgeI.GetId(), attr); }
  TFlt GetFltAttrDatE(const int& EId, const TStr& attr);

  /// Returns an iterator referring to the first edge's int attribute.
  TAIntI BegEAIntI(const TStr& attr) const {
    return TAIntI(VecOfIntVecsE[KeyToIndexTypeE.GetDat(attr).Val2].BegI(), attr, this);
  }
  /// Returns an iterator referring to the past-the-end edge's attribute.
  TAIntI EndEAIntI(const TStr& attr) const {
    return TAIntI(VecOfIntVecsE[KeyToIndexTypeE.GetDat(attr).Val2].EndI(), attr, this);
  }
  /// Returns an iterator referring to the edge of ID EId in the graph.
  TAIntI GetEAIntI(const TStr& attr, const int& EId) const {
    return TAIntI(VecOfIntVecsE[KeyToIndexTypeE.GetDat(attr).Val2].GetI(LinkH.GetKeyId(EId)), attr, this);
  }

  /// Returns an iterator referring to the first edge's str attribute.
  TAStrI BegEAStrI(const TStr& attr) const {
    return TAStrI(VecOfStrVecsE[KeyToIndexTypeE.GetDat(attr).Val2].BegI(), attr, this);   }
  /// Returns an iterator referring to the past-the-end edge's attribute.
  TAStrI EndEAStrI(const TStr& attr) const {
    return TAStrI(VecOfStrVecsE[KeyToIndexTypeE.GetDat(attr).Val2].EndI(), attr, this);
  }
  /// Returns an iterator referring to the edge of ID EId in the graph.
  TAStrI GetEAStrI(const TStr& attr, const int& EId) const {
    return TAStrI(VecOfStrVecsE[KeyToIndexTypeE.GetDat(attr).Val2].GetI(LinkH.GetKeyId(EId)), attr, this);
  }
  /// Returns an iterator referring to the first edge's flt attribute.
  TAFltI BegEAFltI(const TStr& attr) const {
    return TAFltI(VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2].BegI(), attr, this);
  }
  /// Returns an iterator referring to the past-the-end edge's attribute.
  TAFltI EndEAFltI(const TStr& attr) const {
    return TAFltI(VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2].EndI(), attr, this);
  }
  /// Returns an iterator referring to the edge of ID EId in the graph.
  TAFltI GetEAFltI(const TStr& attr, const int& EId) const {
    return TAFltI(VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2].GetI(LinkH.GetKeyId(EId)), attr, this);
  }

  /// Deletes the edge attribute for NodeI.
  int DelAttrDatE(const TCrossEdgeI& EdgeI, const TStr& attr) { return DelAttrDatE(EdgeI.GetId(), attr); } 
  int DelAttrDatE(const int& EId, const TStr& attr); 

   /// Adds a new Int edge attribute to the hashmap.
  int AddIntAttrE(const TStr& attr, TInt defaultValue=TInt::Mn);
  /// Adds a new Str edge attribute to the hashmap.
  int AddStrAttrE(const TStr& attr, TStr defaultValue=TStr::GetNullStr());
  /// Adds a new Flt edge attribute to the hashmap.
  int AddFltAttrE(const TStr& attr, TFlt defaultValue=TFlt::Mn);

   /// Removes all the values for edge  attr.
  int DelAttrE(const TStr& attr);

  // Returns true if \c attr exists for edge \c EId and has default value.
  bool IsAttrDeletedE(const int& EId, const TStr& attr) const;
  // Returns true if Int \c attr exists for edge \c EId and has default value.
  bool IsIntAttrDeletedE(const int& EId, const TStr& attr) const;
  // Returns true if Str \c attr exists for edge \c NId and has default value.
  bool IsStrAttrDeletedE(const int& EId, const TStr& attr) const;
  // Returns true if Flt \c attr exists for edge \c NId and has default value.
  bool IsFltAttrDeletedE(const int& EId, const TStr& attr) const;

  // Returns true if EId attr deleted for current edge attr iterator.
  bool EdgeAttrIsDeleted(const int& EId, const TStrIntPrH::TIter& LinkHI) const;
  // Returns true if EId attr deleted for current edge int attr iterator.
  bool EdgeAttrIsIntDeleted(const int& EId, const TStrIntPrH::TIter& LinkHI) const;
  // Returns true if EId attr deleted for current edge str attr iterator.
  bool EdgeAttrIsStrDeleted(const int& EId, const TStrIntPrH::TIter& LinkHI) const;
  // Returns true if EId attr deleted for current edge flt attr iterator.
  bool EdgeAttrIsFltDeleted(const int& EId, const TStrIntPrH::TIter& LinkHI) const;

  // Returns edge attribute value, converted to Str type.
  TStr GetEdgeAttrValue(const int& EId, const TStrIntPrH::TIter& LinkHI) const;

  size_t GetMemUsed() const { return LinkH.GetMemUsed() + MxEId.GetMemUsed() + Mode1.GetMemUsed() + Mode2.GetMemUsed() + IsDirect.GetMemUsed() +
    LinkTypeId.GetMemUsed() + sizeof(TMMNet*) + KeyToIndexTypeE.GetMemUsed() + IntDefaultsE.GetMemUsed() + FltDefaultsE.GetMemUsed() +
    StrDefaultsE.GetMemUsed() + VecOfIntVecsE.GetMemUsed() + VecOfStrVecsE.GetMemUsed() + VecOfFltVecsE.GetMemUsed(); }

  friend class TMMNet;
  friend class TPt<TCrossNet>;
};

//#///////////////////////////////////////////////
/// Multimodal networks. ##TMMNet::Class
class TMMNet {

public:
  /// TModeNet iterator. Only forward iteration (operator++) is supported.
  class TModeNetI {
  protected:
    typedef THash<TInt, TModeNet>::TIter THashIter;
    THashIter ModeNetHI;
    const TMMNet *Graph;
  public:
    TModeNetI() : ModeNetHI(), Graph(NULL) { }
    TModeNetI(const THashIter& ModeNetHIter, const TMMNet* GraphPt) : ModeNetHI(ModeNetHIter), Graph(GraphPt) { }
    TModeNetI(const TModeNetI& ModeNetI) : ModeNetHI(ModeNetI.ModeNetHI), Graph(ModeNetI.Graph) { }
    TModeNetI& operator = (const TModeNetI& ModeNetI) { ModeNetHI = ModeNetI.ModeNetHI; Graph=ModeNetI.Graph; return *this; }
    /// Increment iterator.
    TModeNetI& operator++ (int) { ModeNetHI++; return *this; }
    bool operator < (const TModeNetI& ModeNetI) const { return ModeNetHI < ModeNetI.ModeNetHI; }
    bool operator == (const TModeNetI& ModeNetI) const { return ModeNetHI == ModeNetI.ModeNetHI; }
    int GetModeId() { return ModeNetHI.GetKey(); }
    TModeNet& GetModeNet() { return Graph->GetModeNetbyId(GetModeId()); }
    //TODO: add method to get edge types
    friend class TMMNet;
  };

  /// TCrossNet iterator. Only forward iteration (operator++) is supported.
  class TCrossNetI {
  protected:
    typedef THash<TInt, TCrossNet>::TIter THashIter;
    THashIter CrossNetHI;
    const TMMNet *Graph;
  public:
    TCrossNetI() : CrossNetHI(), Graph(NULL) { }
    TCrossNetI(const THashIter& CrossNetHIter, const TMMNet* GraphPt) : CrossNetHI(CrossNetHIter), Graph(GraphPt) { }
    TCrossNetI(const TCrossNetI& CrossNetI) : CrossNetHI(CrossNetI.CrossNetHI), Graph(CrossNetI.Graph) { }
    TCrossNetI& operator = (const TCrossNetI& CrossNetI) { CrossNetHI = CrossNetI.CrossNetHI; Graph=CrossNetI.Graph; return *this; }
    /// Increments iterator.
    TCrossNetI& operator++ (int) { CrossNetHI++; return *this; }
    bool operator < (const TCrossNetI& CrossNetI) const { return CrossNetHI < CrossNetI.CrossNetHI; }
    bool operator == (const TCrossNetI& CrossNetI) const { return CrossNetHI == CrossNetI.CrossNetHI; }
    int GetCrossId() { return CrossNetHI.GetKey(); }
    TCrossNet& GetCrossNet() { return Graph->GetCrossNetbyId(GetCrossId()); }
    friend class TMMNet;
  };

private:

  TInt MxModeId; ///< Keeps track of the max mode id.
  TInt MxLinkTypeId; ///< Keeps track of the max crossnet id
  THash<TInt, TModeNet> TModeNetH;
  THash<TInt, TCrossNet> TCrossNetH;

  THash<TInt,TStr> ModeIdToNameH;
  THash<TStr,TInt> ModeNameToIdH;

  THash<TInt,TStr> LinkIdToNameH;
  THash<TStr,TInt> LinkNameToIdH;

public:
  TCRef CRef; //Reference counter. Necessary for pointers.
  friend class TCrossNet;
  friend class TModeNet;

public:
  TMMNet() : CRef(), MxModeId(0), MxLinkTypeId(0), TModeNetH(), TCrossNetH(), ModeIdToNameH(), ModeNameToIdH(), LinkIdToNameH(), LinkNameToIdH() {}
  TMMNet(const TMMNet& OtherTMMNet) : MxModeId(OtherTMMNet.MxModeId), MxLinkTypeId(OtherTMMNet.MxLinkTypeId), TModeNetH(OtherTMMNet.TModeNetH), 
    TCrossNetH(OtherTMMNet.TCrossNetH), ModeIdToNameH(OtherTMMNet.ModeIdToNameH), ModeNameToIdH(OtherTMMNet.ModeNameToIdH), LinkIdToNameH(OtherTMMNet.LinkIdToNameH), LinkNameToIdH(OtherTMMNet.LinkNameToIdH) {}
  TMMNet(TSIn& SIn) : MxModeId(SIn), MxLinkTypeId(SIn), TModeNetH(SIn), TCrossNetH(SIn), ModeIdToNameH(SIn), ModeNameToIdH(SIn), LinkIdToNameH(SIn), LinkNameToIdH(SIn) { 
    for (THash<TInt, TModeNet>::TIter it = TModeNetH.BegI(); it < TModeNetH.EndI(); it++) {
      it.GetDat().SetParentPointer(this);
    }
    for (THash<TInt, TCrossNet>::TIter it = TCrossNetH.BegI(); it < TCrossNetH.EndI(); it++) {
      it.GetDat().SetParentPointer(this);
    }
  }
  size_t GetMemUsed() const { return MxModeId.GetMemUsed() + MxLinkTypeId.GetMemUsed() + TModeNetH.GetMemUsed() +
    TCrossNetH.GetMemUsed() + ModeIdToNameH.GetMemUsed() + ModeNameToIdH.GetMemUsed() + LinkIdToNameH.GetMemUsed() +
    LinkNameToIdH.GetMemUsed(); }
  int AddMode(const TStr& ModeName);
  int DelMode(const TInt& ModeId); // TODO(sramas15): finish implementing
  int DelMode(const TStr& ModeName);
  //TODO: Rename below methods to say CrossNet instead of LinkType
  int AddLinkType(const TStr& ModeName1, const TStr& ModeName2, const TStr& LinkTypeName);
  int AddLinkType(const TInt& ModeId1, const TInt& ModeId2, const TStr& LinkTypeName);
  int AddLinkType(const TStr& ModeName1, const TStr& ModeName2, bool isDir, const TStr& LinkTypeName);
  int AddLinkType(const TInt& ModeId1, const TInt& ModeId2, bool isDir, const TStr& LinkTypeName);
  int DelLinkType(const TInt& LinkTypeId); // TODO(sramas15): finish implementing
  int DelLinkType(const TStr& LinkType);

  void Save(TSOut& SOut) const {MxModeId.Save(SOut); MxLinkTypeId.Save(SOut); TModeNetH.Save(SOut); 
    TCrossNetH.Save(SOut); ModeIdToNameH.Save(SOut); ModeNameToIdH.Save(SOut); LinkIdToNameH.Save(SOut);
    LinkNameToIdH.Save(SOut); }
  static PMMNet Load(TSIn& SIn) { return PMMNet(new TMMNet(SIn)); }
  static PMMNet New() { return PMMNet(new TMMNet()); }

  int GetModeId(const TStr& ModeName) const { if (ModeNameToIdH.IsKey(ModeName)) { return ModeNameToIdH.GetDat(ModeName); } else { return -1; }  }
  TStr GetModeName(const TInt& ModeId) const { if (ModeIdToNameH.IsKey(ModeId)) { return ModeIdToNameH.GetDat(ModeId); } else {return TStr::GetNullStr();} }
  int GetLinkId(const TStr& LinkName) const { if (LinkNameToIdH.IsKey(LinkName)) { return LinkNameToIdH.GetDat(LinkName); } else { return -1; }   }
  TStr GetLinkName(const TInt& LinkId) const { if (LinkIdToNameH.IsKey(LinkId)) { return LinkIdToNameH.GetDat(LinkId); } else { return TStr::GetNullStr(); }  }

  TModeNet& GetModeNet(const TStr& ModeName) const;
  TModeNet& GetModeNetbyId(const TInt& ModeId) const;

  TCrossNet& GetCrossNet(const TStr& LinkName) const;
  TCrossNet& GetCrossNetbyId(const TInt& LinkId) const;

  TCrossNetI GetCrossNetI(const int& Id) const { return TCrossNetI(TCrossNetH.GetI(Id), this); }
  TCrossNetI BegCrossNetI() const { return TCrossNetI(TCrossNetH.BegI(), this); }
  TCrossNetI EndCrossNetI() const { return TCrossNetI(TCrossNetH.EndI(), this); }
 
  TModeNetI GetModeNetI(const int& Id) const { return TModeNetI(TModeNetH.GetI(Id), this); }
  TModeNetI BegModeNetI() const { return TModeNetI(TModeNetH.BegI(), this); }
  TModeNetI EndModeNetI() const { return TModeNetI(TModeNetH.EndI(), this); }

  ///Gets the induced subgraph given a vector of crossnet type names.
  PMMNet GetSubgraphByCrossNet(TStrV& CrossNetTypes);
  ///Gets the induced subgraph given a vector of mode type names
  PMMNet GetSubgraphByModeNet(TStrV& ModeNetTypes);

  PNEANet ToNetwork(TIntV& CrossNetTypes, TVec<TTriple<TInt, TStr, TStr> >& NodeAttrMap, TVec<TTriple<TInt, TStr, TStr> >& EdgeAttrMap);
  PNEANet ToNetwork2(TIntV& CrossNetTypes, THash<TInt, TVec<TPair<TStr, TStr> > >& NodeAttrMap, THash<TInt, TVec<TPair<TStr, TStr> > >& EdgeAttrMap);

private:
  int AddMode(const TStr& ModeName, const TInt& ModeId, const TModeNet& ModeNet);
  int AddLinkType(const TStr& LinkTypeName, const TInt& LinkTypeId, const TCrossNet& CrossNet);
  int AddNodeAttributes(PNEANet& NewNet, TModeNet& Net, TVec<TPair<TStr, TStr> >& Attrs, int ModeId, int oldId, int NId);
  int AddEdgeAttributes(PNEANet& NewNet, TCrossNet& Net, TVec<TPair<TStr, TStr> >& Attrs, int CrossId, int oldId, int EId);
};

// set flags
namespace TSnap {
template <> struct IsMultiGraph<TModeNet> { enum { Val = 1 }; };
template <> struct IsDirected<TModeNet> { enum { Val = 1 }; };
}
#endif // MMNET_H