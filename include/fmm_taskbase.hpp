#ifndef FMM_TASKBASE_HPP
#define FMM_TASKBASE_HPP
#include "ductteip.hpp"
#include "fmm_database.hpp"
#include "fmm_admin.hpp"
#include "sg/superglue.hpp"
#include "sg/platform/atomic.hpp"

namespace FMM_3D{
  /*--------------------------------------------------------------------*/
  extern double work_min;
  /*--------------------------------------------------------------------*/
  typedef enum axs {Read,Write} Access;
  /*--------------------------------------------------------------------*/
  typedef enum key {MVP,
		    Interpolation_Key,
		    Green_Translate,
		    Green_Interpolate,
		    Receiving_Key,
		    FarField_key, //5
		    DT_FFL,        DT_C2P,        DT_XLT,        DT_P2C,        DT_NFL,        DT_RCV,//11
		    DT_ffl,        DT_c2p,        DT_xlt,        DT_p2c,        DT_nfl,        DT_rcv,//17
		    SG_ffl,        SG_c2p,        SG_xlt,        SG_p2c,        SG_nfl,        SG_rcv,//23
		    DT_NVD,
		    NUM_TASK_KEYS
  } TaskKey;
  /*======================================================================*/
  /*
    class TopLayerData: public IData {
    private:
    int M,N,y,x;
    uint32_t part_no,ng,np;
    vector<TopLayerData*> groups,parts;
    byte *memory_p;
    int mem_size_in_bytes;
    typedef TopLayerData GData;
    GData *parent;
    public:
    BoxList boxes;
    TopLayerData(int row, int col,string parent_name):y(row),x(col){
    memory_type = USER_ALLOCATED;
    IData::parent_data = NULL;
    setDataHandle( fmm_engine->createDataHandle(this));
    setDataHostPolicy( glbCtx.getDataHostPolicy() ) ;
    setLocalNumBlocks(1,1);
    IData::Mb = 0;
    IData::Nb = 0;
    setParent(fmm_engine);
    fmm_engine->addInputData(this);
    setRunTimeVersion("0.0",0);
    std::stringstream ss;
    ss << parent_name << "(" << y << "," << x << ")";
    setName(ss.str());
    memory_type = USER_ALLOCATED;
    setHostType(SINGLE_HOST);
    memory_p = new byte[100];
    mem_size_in_bytes = 100;
    allocateMemory();
    LOG_INFO(LOG_FMM,"Data created:%s\n",getName().c_str());
    }
    TopLayerData(int row, int col, int part,GData &p):y(row),x(col),part_no(part){
    parent=static_cast<GData *>(&p);
    std::stringstream ss;
    ss << "Box(" << row << "," << col << ")[" << part <<"]";
    setName(ss.str());
    }
    void getExistingMemoryInfo(byte **b, int *s, int *l){
    LOG_INFO(LOG_FMM,"Reported memory:%p with size %d for data %s.\n",
    memory_p,mem_size_in_bytes,getName().c_str());
    *b = (byte *)memory_p;
    *s = mem_size_in_bytes;
    *l = 1;
    }
    void setNewMemoryInfo(MemoryItem*mi){
    memory_p = (byte*)(mi->getAddress()+getHeaderSize());
    }
    int get_row(){return y;}
    int get_col(){return x;}
    int part_count(){return parts.size();}
    void make_groups();
    void make_partitions();
    void make_hosts();
    TopLayerData(int M_, int N_,int np_,const char* n):M(M_),N(N_),ng(N_),np(np_){
    setName(n);
    LOG_INFO(LOG_FMM,"Data:%s\n",n);
    printf("Data:%s\n",n);
    for(int j=0;j<N;j++){
    for(int i=0;i<M;i++){
    groups.push_back(new TopLayerData(i,j,n));
    }
    }
    }
    TopLayerData &operator()(int i,int j){
    return *groups[j*M+i];
    }
    bool contains(Box *b){
    for( Box *my_b: boxes){
    if (my_b->index == b->index)
    return true;
    }
    return false;
    }
    TopLayerData &operator[](int i){
    int n = parts.size();
    (void)n;
    return *parts[i];
    }
    void show_hierarchy(){
    for (int L=0;L<L_max;L++ ){
    for (int gi=0;gi<Parameters.groups;gi++){
    TopLayerData &D=(*this)(L,gi);
    cout << "Boxes in D("<< L << "," << gi<<")\n" ;
    for(Box *b:D.boxes){
    D.show_box(b);
    }
    for (uint32_t pi=0;pi<D.parts.size();pi++ ){
    cout << "Boxes in D("<< L << "," << gi<<"," << pi << ")\n" ;
    for( Box *bp:D[pi].boxes){
    D[pi].show_box(bp);
    }
    }
    }
    }
    }
    void show_box(Box *b){
    cout << "B("<< b->level << "," << b->index << ")" << endl;
    }

    };
    typedef TopLayerData GData;
  */
  template <typename T>
  class GNode{
  public:
    GNode *parent;
    vector<GNode*> children;
    T* object;
  };
  class XData: public GNode<pair<Box*,XData*>> , public IData  {
  public:
    vector<Box *> boxes;
    XData *getXData(        ){return object->second;}
    void   setXData(XData *x){object->second = x;   }

    Box   *getBox(     ){return object->first;}
    void   setBox(Box*b){object->first = b;   }

    XData *getParentXData(){return parent->object->second;}
    Box   *getParentBox  (){return parent->object->first; }
    XData *getChildXData(int i){
      return children[i]->object->second;
    }
    // Todo begin 
    void make_groups(){}
    void make_partitions(){} 
    void make_hosts(){}
    int part_count(){return 0;}
    XData(int,int,int,string){}
    XData(){}
    XData *operator[](int){return this;}
    int get_row(){return 0;}
    // Todo end
  };
  class XBox: public GNode<pair<Box*,XData*>>{
  public:
    XData *getXData(        ){return object->second;}
    void   setXData(XData *x){object->second = x;   }

    Box   *getBox(     ){return object->first;}
    void   setBox(Box*b){object->first = b;   }

    XData *getParentXData(){return parent->object->second;}
    Box   *getParentBox  (){return parent->object->first; }
    
  };
  template <typename T>
  class GLevel{
  public:
    vector<T*> node_list;
    GLevel &operator <<(T* a){
      node_list.push_back(a);
      return *this;
    }
    void addNode(T*n){
      *this << n;
    }
  };
  template < typename T>
  class GTree{
  public:
    vector<GLevel<T> *> level_list;
    T* operator ()(int i, int j ){
      return (T*)level_list[i]->node_list[j];
    }
    GLevel<T> &addLevel(){
      GLevel<T> *L = new GLevel<T>;
      level_list.push_back(L);
      return *L;
    }
  };
  typedef GNode<Box> GNodeBox;
  typedef GTree<XBox> GBoxTree;
  typedef GTree<XData> GGroupTree;

  void make_group(GBoxTree *bt, GGroupTree *gt);
  class GTreeData {
  public:
    GBoxTree *BT;
    GGroupTree *GT;
    string h;
    GTreeData(GBoxTree *bt){
      BT=bt;
      GT=new GGroupTree ;
      make_group(bt,GT);
    }
    XData * operator ()(int i,int j){
      XData *d = nullptr;
      d=(*GT)(i,j);
      return d;
    }
    void getParent(){}
    int getGroupCount(int level){
      return GT->level_list[level]->node_list.size();
    }
  };

  typedef XData GData;
  typedef GData TopLayerData;
  /*====================================================================*/
  class DTTask : public IDuctteipTask  {
  public:
    int key;
    std::string name;
    int axs[4];
    DTBase *args[4];
    DTTask *parent;
    std::atomic<size_t> child_count;
    bool is_submitting;
    /*------------------------------------------------------------*/
    DTTask &operator <<(GData &d1){ // Read data
      DataAccess *daxs = new DataAccess;
      IData *d = &d1;
      daxs->data = d;
      daxs->required_version = d->getWriteVersion();
      daxs->required_version.setContext( glbCtx.getLevelString() );
      d->getWriteVersion().setContext( glbCtx.getLevelString() );
      d->getReadVersion() .setContext( glbCtx.getLevelString() );
      daxs->type = IData::READ;
      data_list->push_back(daxs);
      d->incrementVersion(IData::READ);
      return *this;
	
    }
    /*------------------------------------------------------------*/
    DTTask &operator >>(GData &d1){// Write Data 
      DataAccess *daxs = new DataAccess;
      IData *d = &d1;
      daxs->data = d;
      daxs->required_version = d->getReadVersion();
      daxs->required_version.setContext( glbCtx.getLevelString() );
      d->getWriteVersion().setContext( glbCtx.getLevelString() );
      d->getReadVersion() .setContext( glbCtx.getLevelString() );
      daxs->type = IData::WRITE;
      data_list->push_back(daxs);
      d->incrementVersion(IData::WRITE);
      host = d1.getHost();
      return *this;
    }
    /*------------------------------------------------------------*/
    DTTask(){
      data_list = new list<DataAccess*>;
      child_count = 0;
      parent = nullptr;
      is_submitting = false;
    }
    /*--------------------------------------------------------------------*/
    bool is_still_submitting(){return is_submitting;}
    /*--------------------------------------------------------------------*/
    virtual void runKernel() = 0 ;
    /*--------------------------------------------------------------------*/
    virtual void finished(){
      if ( state >= Finished  ) return;
      setFinished(true);
      if (parent){
	while ( parent->is_still_submitting() ){};
	//	if ( Atomic::decrease_nv(&parent->child_count) ==0){
	if ( --parent->child_count ==0){
	  parent->finished();
	}
      }	
    }
    /*--------------------------------------------------------------------*/
    virtual void export_it(fstream&)=0;
    /*--------------------------------------------------------------------*/
  };
  /*====================================================================*/
  class SGTask : public Task<Options>  {
  public:
    double work;
    int key;
    DTTask *parent;
    /*---------------------------------------------------------*/
    void set_parent(DTTask *p){
      parent = p;
      if ( p)
	parent->child_count++;	
    }
    /*---------------------------------------------------------*/
    ~SGTask(){
      if (parent){
	while ( parent->is_still_submitting() ) {};
	
	//	if ( sg::Atomic::decrease_nv(&parent->child_count) ==0 )
	if ( --parent->child_count ==0 )
	  parent->finished();
      }	
    }
    virtual void run(TaskExecutor<Options> &)=0;
  };
  /*--------------------------------------------------------------------*/
  typedef std::pair<Box *,Box*> F2GPair;
  typedef std::pair<Box *,Box*> BoxPair;
  typedef std::vector<F2GPair*>  F2GList;
  typedef std::vector<BoxPair*>  X2YList;
  typedef void (*BoxPairFcn)(string , BoxPair *);
  typedef void (*BoxFcn)(string , Box *);
  /*--------------------------------------------------------------------*/
  typedef list<DataAccess*> DataAccessList ;
  void data_access(DataAccessList *dlist,IData *d,IData::AccessType rw){
    DataAccess *dx1 = new DataAccess;
    dx1->data = d;
    if ( rw == Data::READ){
      dx1->required_version = d->getWriteVersion();
      dx1->type = IData::READ;
    }
    else{
      dx1->required_version = d->getReadVersion();
      dx1->type = IData::WRITE;
    }
    dx1->required_version.setContext( glbCtx.getLevelString() );
    d->getWriteVersion().setContext( glbCtx.getLevelString() );
    d->getReadVersion().setContext( glbCtx.getLevelString() );
    dlist->push_back(dx1);
    d->incrementVersion(rw);
  }
  /*======================================================================*/
  class  IterData : public IData {
  public:
    IterData(){
      memory_type = USER_ALLOCATED;
      host_type=ALL_HOST;
      IData::parent_data = NULL;
      setDataHandle( fmm_engine->createDataHandle(this));
      setDataHostPolicy( glbCtx.getDataHostPolicy() ) ;
      setLocalNumBlocks(1,1);
      IData::Mb = 0;
      IData::Nb = 0;
      setHostType(ALL_HOST);
      setParent(fmm_engine);
      fmm_engine->addInputData(this);
      LOG_INFO(LOG_FMM,"Data handle for new dtswdata:%d\n",my_data_handle->data_handle);
      setRunTimeVersion("0.0",0);
    }    
  };
  /*======================================================================*/  
  class IterTask: public DTTask{
  public:
    int iter;
    static double norm_V_diff;
    static int last_iter_no;
    static IterData  *D;
    IterTask(){
      iter = last_iter_no++;
      key=-1;
      register_data();
    }
    void finished(){
      if ( norm_V_diff > 1e-6)
	fmm_engine->add_task ( new IterTask () );
    }
    void runKernel();
    void export_it(fstream &){}
    /*----------------------------------------------------------------*/
    void register_data(){
      parent_context = fmm_engine;
      setName("Iter");
      D->setName("IterData");
      if ( last_iter_no ==1)
	D->setRunTimeVersion("0.0",0);      
      IDuctteipTask::key = key;
      DataAccessList *dlist = new DataAccessList;    
      data_access(dlist,D,(last_iter_no==1)?IData::READ:IData::WRITE);
      setDataAccessList(dlist);
      host = me;
    }
  };
  /*---------------------------------------------------*/
  extern FMM_3D::GData *mainF,*mainG;
  /*======================================================================*/
  class FFLTask2: public DTTask{
  public:
    XData *d1,*d2;
    /*----------------------------------------------------------------------*/
    FFLTask2(XData *d1_,XData *d2_,DTTask *p){
      d1 = d1_;d2 = d2_;
      key= DT_FFL;
      parent = p;
      parent->child_count ++;
    }

    void finished(){
      if (--parent->child_count ==0)
	parent->finished();
    }
    void run();
    void runKernel(){}
    void export_it(fstream &f){}
  };
  void FFLTask2::run(){}
  class FFLTask: public DTTask{
  public:
    GData *d1,*d2;
    /*----------------------------------------------------------------------*/
    FFLTask(GData *d1_,GData *d2_,DTTask *p){
      d1 = static_cast<GData *>(d1_);
      d2 = static_cast<GData *>(d2_);
      key= DT_FFL;
      setName("FFLTask");
      parent = p;
      if ( parent ) 
	parent->child_count ++;
      *this << *d1_ >> *d2_;
    }
    FFLTask(GData *d1_,DTTask *p){
      d2 = static_cast<GData *>(d1_);
      d1 = nullptr;
      key= DT_FFL;
      setName("FFLTask");
      parent = p;
      if ( parent )
	parent ->child_count ++;
      *this >> *d1_;
    }

    void runKernel();
    void export_it(fstream &f){}
  };
  /*======================================================================*/
  class C2PTask: public DTTask{
  public:
    GData *d1,*d2,*d3;
    /*----------------------------------------------------------------------*/
    C2PTask(GData *d1_,GData *d2_, GData *d3_,DTTask *p){
      d1 = static_cast<GData *>(d1_);
      d2 = static_cast<GData *>(d2_);
      d3 = static_cast<GData *>(d3_);
      key= DT_C2P;
      setName("C2PTask");
      parent = p;
      if(parent)
	parent->child_count ++;
      *this << *d1 << *d2 >> *d3;
    }
    C2PTask(GData *d1_,GData *d2_,DTTask *p){
      d2 = static_cast<GData *>(d1_);
      d3 = static_cast<GData *>(d2_);
      d1 = nullptr;
      key= DT_C2P;
      setName("C2PTask");
      parent = p;
      if(parent)
	parent->child_count ++;
      *this << *d2 >> *d3;
    }
    void runKernel();
    void export_it(fstream &f){}
  };
  /*======================================================================*/
  class XLTTask: public DTTask {
  public:
    GData *d1,*d2;
    XLTTask(GData *d1_,GData *d2_,DTTask *p){
      d1 = static_cast<GData *>(d1_);
      d2 = static_cast<GData *>(d2_);
      key= DT_XLT;
      setName("XLTTask");
      parent = p;
      if ( parent ) 
	parent->child_count ++;
      *this << *d1 >> *d2;
    }
    void runKernel();
    void export_it(fstream &){}
  };
  /*======================================================================*/
  class P2CTask: public DTTask {
  public:
    GData *d1,*d2;
    P2CTask(GData &d1_,GData &d2_,DTTask *p){
      d1 = static_cast<GData *>(&d1_);
      d2 = static_cast<GData *>(&d2_);
      key= DT_P2C;
      setName("P2CTask");
      parent =p;
      if ( parent )
	parent->child_count ++;
      *this << *d1 >> *d2;
    }
    void runKernel();
    void export_it(fstream &){}
  };
  /*======================================================================*/
  class RCVTask: public DTTask {
  public:
    GData *d1,*d2,*d3;
    RCVTask(GData *d1_,GData *d2_, DTTask *p){
      d3 = static_cast<GData *>(d2_);
      d2 = static_cast<GData *>(d1_);
      d1 = nullptr;
      key= DT_RCV;
      setName("RCVTask");
      parent=p;
      if ( parent )
	parent->child_count++;
      *this << *d2 >> *d3;
    }

    RCVTask(GData *d1_,GData *d2_,GData *d3_,DTTask *p){
      d1 = static_cast<GData *>(d1_);
      d2 = static_cast<GData *>(d2_);
      d3 = static_cast<GData *>(d3_);
      key= DT_RCV;
      setName("RCVTask");
      parent = p;
      if ( parent )
	parent->child_count ++;
      *this << *d1 << *d2  >> *d3;

    }
    void runKernel();
    void export_it(fstream &){}
  };
  /*======================================================================*/
  class NVDTask: public DTTask {
  public:
    GData *d1,*d2,*d3;
    NVDTask(GData *d1_,GData *d2_,DTTask *p){
      d3 = static_cast<GData *>(d2_);
      d2 = static_cast<GData *>(d1_);
      d1 = nullptr;
      key= DT_NVD;
      setName("NVDTask");
      parent = p;
      if ( parent )
	p->child_count++;
      *this << *d2 >> *d3;
    }

    void runKernel(){}
    void export_it(fstream &){}
  };
  /*======================================================================*/
  class rcvTask: public DTTask {
  public:
    GData *d1,*d2,*d3;
    rcvTask(GData *d1_,GData *d2_,DTTask *p){
      d3 = static_cast<GData *>(d2_);
      d2 = static_cast<GData *>(d1_);
      d1 = nullptr;
      key= DT_rcv;
      setName("rcvTask");
      parent = p;
      if ( parent )
	p->child_count++;
      *this << *d2 >> *d3;
    }

    rcvTask(GData *d1_,GData *d2_,GData *d3_,DTTask *p){
      d1 = static_cast<GData *>(d1_);
      d2 = static_cast<GData *>(d2_);
      d3 = static_cast<GData *>(d3_);
      key= DT_rcv;
      setName("rcvTask");
      parent = p;
      if ( parent )
	p->child_count++;
      *this << *d1 << *d2 >> *d3;
      
    }
    void runKernel();
    void export_it(fstream &){}
  };

  /*======================================================================*/
  class NFLTask: public DTTask {
  public:
    GData *d1,*d2;
    int group;
    NFLTask(GData * d1_,GData *d2_,int g,DTTask *p ):group(g){
      d1 = static_cast<GData *>(d1_);
      d2 = static_cast<GData *>(d2_);
      key= DT_NFL;
      setName("NFLTask");
      parent = p;
      if ( parent )
	parent->child_count ++;
      *this << *d1 >> *d2;
    }
    void runKernel();
    void export_it(fstream &){}
  };
  /*======================================================================*/
  class nflTask : public DTTask {
  public:
    int group,part;
    nflTask(int g,int i,DTTask *p):group(g),part(i){
      key= DT_nfl;
      parent = p;
      if ( parent )
	parent->child_count ++;
    }
    void runKernel();
    void export_it(fstream &){}
  };
  /*======================================================================*/
  class fflTask: public DTTask{
  public:
    GData *d;
    fflTask(GData *d_,DTTask *p){
      d = static_cast<GData *>(d_);
      key = DT_ffl;
      setName("fflTask");
      parent = p;
      if ( parent )
	p->child_count++;
      *this >> *d;
    }
    void runKernel();
    void export_it ( fstream &){}
  };
  /*======================================================================*/
  class c2pTask: public DTTask{
  public:
    GData *d1,*d2;
    c2pTask(GData *c, GData *p,DTTask *father){
      d1 = static_cast<GData *>(c);
      d2 = static_cast<GData *>(p);
      key = DT_c2p;
      setName("c2pTask");
      parent = father;
      if ( parent )
	father->child_count ++;
      *this << *d1 >> *d2;
    }
    void runKernel();
    void export_it ( fstream &){}
  };
  /*======================================================================*/
  class xltTask: public DTTask{
  public:
    GData *d1,*d2;
    xltTask(GData *from, GData *to,DTTask *p){
      d1 = static_cast<GData *>(from);
      d2 = static_cast<GData *>(to);
      key = DT_xlt;
      setName("xltTask");
      parent = p;
      if ( parent )
	p->child_count ++;
      *this << *d1 >> *d2;
    }
    void runKernel();
    void export_it ( fstream &){}
  };
  /*======================================================================*/
  class p2cTask: public DTTask{
  public:
    GData *d1,*d2;
    p2cTask(GData *p, GData *c,DTTask *father){
      d1 = static_cast<GData *>(p);
      d2 = static_cast<GData *>(c);
      key = DT_p2c;
      setName("p2cTask");
      parent = father;
      if ( parent )
	father->child_count ++;
      *this << *d1 >> *d2;
    }
    void runKernel();
    void export_it ( fstream &){}
  };
  /*======================================================================*/
  double get_c2p_work(Box *);
  double get_xlt_work(F2GPair *);
  double get_p2c_work(Box *);
  double get_ffl_work(Box *);
  double get_rcv_work(Box *);
  double get_nfl_work(BoxPair *);
  void fmm_taskified();
  void NFL_tasks ( GData & dep,int chunk_no, int chunks_count);
  void for_each_pair_in_batch(X2YList *b, BoxPairFcn func,string);
  void for_each_box_in_batch(BoxList *b, BoxFcn func,string );
  void print_box_pair(string prefix , BoxPair *bp);
  void print_box(string prefix , Box *bp);
  bool is_box_in_list(BoxList &boxes, Box * b);
  GBoxTree*convert_tree(Tree *t);
}
#endif // FMM_TASKBASE_HPP
