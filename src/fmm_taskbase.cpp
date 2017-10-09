#include "fmm_taskbase.hpp"
#include "fmm_computations.hpp"


namespace FMM_3D{
  TopLayerData *mainF,*mainG,*mainV,*copyV;
  double  IterTask::norm_V_diff  = 1e6;
  int     IterTask::last_iter_no = 0;
  IterData *IterTask::D = nullptr;
  /*------------------------------------------------------------------------*/
  void prepare(GData *g){
    g->make_groups();
    g->make_partitions();
    g->make_hosts();
  }
  void fmm_taskified(){
    int np   = Parameters.part_count;
    int g    = Parameters.group_count;
    LOG_INFO(LOG_FMM,"Group count:%d.\n",g);
    mainF    = new TopLayerData(L_max,g,np,"F"); prepare(mainF);
    mainG    = new TopLayerData(L_max,g,np,"G"); prepare(mainG);
    mainV    = new TopLayerData(L_max,g,np,"V"); prepare(mainV);
    copyV    = new TopLayerData(L_max,g,np,"Vc");prepare(copyV);
    IterTask::D = new IterData;
    for( int iter=0;iter<Parameters.iter_batch_count; iter++){	  
      fmm_engine->add_task(new IterTask());
    }
  }
  /*------------------------------------------------------------------------*/
  void IterTask::runKernel(){
    GBoxTree *bt = convert_tree(tree);
    GTreeData F(bt);

    int L=bt->level_list.size();
    int  g=F.getGroupCount(L);
    LOG_INFO(LOG_FMM,"Group count:%d.\n",g);
    for(int gi = 0;gi<g; gi++){
        fmm_engine->add_task( new FFLTask (F(L,gi)->getChildXData(0),F(L,gi)->getParentXData(),this));
    }
    //    GData &F = *mainF;
    GData &G = *mainG;

    GData &V  = (iter %2 ==0)?*mainV:*copyV;
    GData &V2 = (iter %2 ==1)?*mainV:*copyV;
    LOG_INFO(LOG_FMM,"Group count:%d.\n",g);
    /*
    for(int gi = 0;gi<g; gi++){
      LOG_INFO(LOG_FMM,"submit FFLTask F(%d,%d).\n",L_max-1,gi);
      fmm_engine->add_task( new FFLTask (F(L_max,gi-1),F(L_max-1,gi),this));
    }
    for(int L=L_max-1;L>0;L--){
      for(int gi = 0;gi<g; gi++){
	LOG_INFO(LOG_FMM,"submit C2PTask F(%d,%d), F(%d,%d).\n",L,gi,L-1,(L==1)?0:gi);
	if ( gi ==0)
	  fmm_engine->add_task( new C2PTask (          F(L,gi),F(L-1,(L==1)?0:gi),this));
	else
	  fmm_engine->add_task( new C2PTask (F(L,gi-1),F(L,gi),F(L-1,(L==1)?0:gi),this));
	for(int gj = 0;gj<g; gj++){
	  LOG_INFO(LOG_FMM,"submit XLTTask F(%d,%d), G(%d,%d).\n",L,gi,L,gj);
	  fmm_engine->add_task( new XLTTask (F(L,gi),G(L,gj),this));
	}
      }
    }
    for(int L=1;L<L_max;L++){
      for(int gi = 0;gi<g; gi++){
	LOG_INFO(LOG_FMM,"submit P2CTask G(%d,%d), G(%d,%d).\n",L-1,gi,L,gi);
	fmm_engine->add_task ( new P2CTask(G(L-1,gi),G(L,gi),this));
      }
    }
    for(int gi = 0;gi<g; gi++){
      LOG_INFO(LOG_FMM,"submit RCVTask G(%d,%d), V(%d,%d).\n",L_max-1,gi,L_max-1,gi);
      if ( gi ==0)
	fmm_engine->add_task( new RCVTask (                G(L_max-1,gi),V(L_max-1,gi),this));
      else
	fmm_engine->add_task( new RCVTask (G(L_max-1,gi-1),G(L_max-1,gi),V(L_max-1,gi),this));

      LOG_INFO(LOG_FMM,"submit NFLTask F(%d,%d), V(%d,%d).\n",L_max/2,gi,L_max-1,gi);
      fmm_engine->add_task( new NFLTask (F(L_max/2,gi),V(L_max-1,gi),gi,this));

      LOG_INFO(LOG_FMM,"submit NVDTask V2(%d,%d), V(%d,%d).\n",L_max-1,gi,L_max-1,gi);
      fmm_engine->add_task( new NVDTask (V2(L_max-1,gi),V(L_max-1,gi),this));
    }
    */
  }
  /*------------------------------------------------------------------------*/
  void FFLTask::runKernel(){
    GData &B = *d2;
    for(int i=0;i<B.part_count() ;i++){
      LOG_INFO(LOG_FMM,"submit fflTask %s.\n",B[i]->getName().c_str());
      fmm_engine->add_task( new fflTask(B[i],this));
    }
  }
  /*------------------------------------------------------------------------*/
  void fflTask::runKernel(){
    BoxList * batch = new BoxList;
    double work=0.0;
    LOG_INFO(LOG_FMM,"fflTask::runKernel \n");
    for (Box *b: d->boxes){
      work += get_ffl_work(b);
      if ( work < Parameters.work_min){
	batch->push_back(b);
      }
      else{
	LOG_INFO(LOG_FMM,"submit ffl_task with batch of %d boxes.\n",batch->size() );
	fmm_engine->add_task (new ffl_task (batch,this) );
	work = 0.0;
	batch = new BoxList;
      }
    }
    if (work >0.0 or batch->size() > 0 ){
      LOG_INFO(LOG_FMM,"submit ffl_task with batch of %d boxes.\n",batch->size());
      fmm_engine->add_task (new ffl_task (batch,this) );
    }

  }
  /*------------------------------------------------------------------------*/
  void C2PTask::runKernel(){
    GData &A = *d2;
    GData &B = *d3;
    int n = A.part_count() ;
    for(int i=0;i<n;i++){
      int j=i;
      if ( B.get_row() ==0 ) { // if top most level
	j = 0;
      }
      LOG_INFO(LOG_FMM,"submit c2pTask (%s,%s).\n",A[i]->getName().c_str(),B[j]->getName().c_str());
      fmm_engine->add_task( new c2pTask(A[i],B[j],this));
    }
  }
  /*------------------------------------------------------------------------*/
  void c2pTask::runKernel(){// DuctTeip level, every parent box can be a SG task

    BoxList &parents= d2->boxes;
    BoxList *batch = new BoxList;
    double batch_work = 0;
    for(Box *parent : parents ){

      if(batch_work < Parameters.work_min){
	batch->push_back(parent);
	batch_work += get_c2p_work(parent);
      }
      else{
	LOG_INFO(LOG_FMM,"submit c2p_task with bach size %d.\n",batch->size());
	fmm_engine->add_task ( new c2p_task(batch,this));
	batch = new BoxList;
	batch_work=0.0;
      }
    }
    if ( batch_work > 0.0 or batch->size() > 0){
      LOG_INFO(LOG_FMM,"submit c2p_task with bach size %d.\n",batch->size());
      fmm_engine->add_task ( new c2p_task(batch,this));
    }
  }
  /*------------------------------------------------------------------------*/
  void XLTTask::runKernel(){
    GData &A = *d1;
    GData &B = *d2;
    for ( int i=0;i<A.part_count();i++){
      for(int j=0;j<B.part_count();j++){
	int n1 = 0;///A[i]->boxes.size(); TODO 
	int n2 = 0; //B[j]->boxes.size(); TODO 
	if (n1 != 0 and n2 != 0 ){
	  LOG_INFO(LOG_FMM,"submit xltTask (%s,%s).\n",A[i]->getName().c_str(),B[j]->getName().c_str());
	  fmm_engine->add_task( new xltTask(A[i],B[j],this));
	}
      }
    }
  }
  /*------------------------------------------------------------------------*/
  void xltTask::runKernel(){//DT Level , every target G can be separate SG task

    F2GList *batch = new F2GList;
    double batch_work = 0.0;
    for(Box *b1: d1->boxes){
      for(Box *b2: d2->boxes){
	if ( is_box_in_list(b2->ff_int_list,b1)){
	  F2GPair *f2gp= new F2GPair(b1,b2);
	  batch->push_back(f2gp);
	  batch_work += get_xlt_work(f2gp);
	}
	if ( batch_work > Parameters.work_min){
	  LOG_INFO(LOG_FMM,"submit xlt_task with bach size %d.\n",batch->size());
	  fmm_engine->add_task ( new xlt_task (batch,this)) ;
	  batch = new F2GList;
	  batch_work = 0.0;
	}
      }
    }
    if ( batch_work > 0.0 or batch->size() >0){
      LOG_INFO(LOG_FMM,"submit xlt_task with bach size %d.\n",batch->size());
      fmm_engine->add_task ( new xlt_task (batch,this)) ;
    }
  }
  /*------------------------------------------------------------------------*/
  void P2CTask::runKernel(){
    GData &A = *d1;
    GData &B = *d2;
    for(int i=0;i<A.part_count();i++){
      LOG_INFO(LOG_FMM,"submit p2cTask (%s,%s).\n",A[i]->getName().c_str(),B[i]->getName().c_str());
      fmm_engine->add_task ( new p2cTask(A[i],B[i],this));
    }
  }
  /*------------------------------------------------------------------------*/
  void RCVTask::runKernel(){
    GData &A = *d2;
    GData &B = *d3;
    for(int i=0;i<A.part_count();i++){
      LOG_INFO(LOG_FMM,"submit rcvTask (%s,%s).\n",A[i]->getName().c_str(),B[i]->getName().c_str());
      fmm_engine->add_task ( new rcvTask(A[i],B[i],this));
    }
  }
  /*------------------------------------------------------------------------*/
  void rcvTask::runKernel(){
    double work =0.0;
    X2YList *batch = new X2YList;
    for( Box * b1: d2->boxes){
      for ( Box *b2: d3->boxes){
	work += get_rcv_work(b1);
	if ( work < Parameters.work_min){
	  batch->push_back(new BoxPair (b1,b2) );
	}
	else{
	  LOG_INFO(LOG_FMM,"submit rcv_task with bach size %d.\n",batch->size());
	  fmm_engine->add_task(new rcv_task(batch,this));
	  batch = new X2YList;
	  work =0.0;
	}
      }
    }
    if ( work > 0 or batch->size()>0 ) {
      LOG_INFO(LOG_FMM,"submit rcv_task with bach size %d.\n",batch->size());
      fmm_engine->add_task(new rcv_task(batch,this));
    }

  }
  /*------------------------------------------------------------------------*/
  void NFLTask::runKernel(){
    for (int i=0;i<Parameters.part_count ;i++){
      LOG_INFO(LOG_FMM,"submit nflTask (%d,%d).\n",group , i);
      fmm_engine->add_task(new nflTask(group,i,this));
    }
  }
  /*------------------------------------------------------------------------*/
  void nflTask::runKernel(){
    /*
    GData &F = *mainF;
    BoxList &boxes =  F(L_max-1,group)[part]->boxes; /// No Boxes member error
    double work =0.0;
    X2YList *batch = new X2YList;
    for ( Box *b1: boxes){
      for (Box *b2 : b1->nf_int_list ){
	BoxPair * bp = new BoxPair ( b1,b2);
	work += get_nfl_work( bp);
	if ( work <Parameters.work_min){
	  batch->push_back( bp);
	}
	else{
	  LOG_INFO(LOG_FMM,"submit nfl_task with bach size %d.\n",batch->size());
	  fmm_engine->add_task( new nfl_task(batch,this));
	  batch = new X2YList;
	  work = 0.0;
	}
      }
    }
    if(work > 0.0 or batch->size() > 0 ){
      LOG_INFO(LOG_FMM,"submit nfl_task with bach size %d.\n",batch->size());
      fmm_engine->add_task( new nfl_task(batch,this));
    }
    */
  }
  /*------------------------------------------------------------------------*/
  void p2cTask::runKernel(){

    BoxList &parents= d1->boxes;
    BoxList *batch = new BoxList;
    double batch_work = 0;

    for(Box* parent:parents){
      for ( Box * c: parent->children){
	if(batch_work < Parameters.work_min){
	  batch->push_back(c);
	  batch_work += get_p2c_work(c);
	}
	else{
	LOG_INFO(LOG_FMM,"submit p2c_task with bach size %d.\n",batch->size());
	  fmm_engine->add_task ( new p2c_task(batch,this));
	  batch = new BoxList;
	  batch_work=0.0;
	}
      }
    }
    if ( batch_work > 0.0)
      LOG_INFO(LOG_FMM,"submit p2c_task with bach size %d.\n",batch->size());
      fmm_engine->add_task ( new p2c_task(batch,this));
  }
  /*------------------------------------------------------------------------*/
  double get_c2p_work(Box *c){
    LevelBase &L=*tree->Level[c->level-1];
    int nnz1=L.C2P->A1->data.size();
    int nnz2=L.C2P->A2->data.size();
    int m,n;
    c->Ft->get_dims(m,n);
    return m*n*nnz1*nnz2;
  }
  /*----------------------------------------------------------------------*/
  double get_xlt_work(F2GPair *boxes){
    int l = boxes->first->level-1;
    LevelBase &L = *tree->Level[l];
    int m,n,k,p;
    int z = L.T.size();
    if ( z )
      L.T[0]->get_dims(m,n);
    boxes->first->Ft->get_dims(k,p);
    return (double)(m*n*k)/3.0;
  }
  /*----------------------------------------------------------------------*/
  double get_p2c_work(Box *c){
    LevelBase &L=*tree->Level[c->level-1];
    int nnz1=L.P2C->A1->data.size();
    int nnz2=L.P2C->A2->data.size();
    int m,n;
    c->Ft->get_dims(m,n);
    return m*n*nnz1*nnz2;

  }
  /*----------------------------------------------------------------------*/
  double get_ffl_work(Box *b){
    int M,N,K;
    b->F->get_dims(M,N);
    b->I->get_dims(N,K);
    return (double)(M*N*K)/3.0;
  }
  /*----------------------------------------------------------------------*/
  double get_nfl_work(BoxPair* boxes){
    int M,N,K,P;
    int idx = boxes->second->index-1;
    int z = boxes->first->Z.data.size();
    if ( idx < z ){
      if(boxes->first->Z.data[idx])
	boxes->first->Z.data[idx]->get_dims(M,N);
    }
    else{
      M = N = 0;
    }

    boxes->first->I->get_dims(K,P);
    return (double)(M*N*K)/3.0;
  }
  /*----------------------------------------------------------------------*/
  double get_rcv_work(Box *c){
    return 0.0;
  }
  /*----------------------------------------------------------------------*/
  void NFL_tasks ( GData & dep,int chunk_no, int chunks_count){
  }
  /*----------------------------------------------------------------------*/
  /*
  void TopLayerData::make_groups(){
    int bpg,nbl;
    TopLayerData &Root=(*this)(0,0);
    LevelBase &TopLevel=*tree->Level[0];
    Root.boxes.push_back(TopLevel.boxes[0]);
    TopLevel.boxes[0]->group = 0;
    int L = 1;
    {
      nbl = Root.boxes[0]->children.size();
      bpg = nbl/ng+1;
      for (int bi=0;bi<nbl;bi++){
	Box *c=Root.boxes[0]->children[bi];
	GData &cd=(*this)(L,bi/bpg);
	cd.boxes.push_back(c);
	c->group = bi/bpg;
      }
    }
    for(L=2;L<L_max;L++){
      for ( Box *pb: tree->Level[L-1]->boxes){
	for( Box *cb: pb->children){
	  int pg=cb->parent->group;
	  GData &node=(*this)(L,pg);
	  node.boxes.push_back(cb);
	  cb->group = pg;
	}
      }
    }
  }
  */
  /*---------------------------------------------------*/
  /*
  void TopLayerData::make_partitions(){
    // assumptions : this is at the top layer
    TopLayerData &Root=(*this)(0,0);
    LevelBase &Level=*tree->Level[0];
    //create a single part for the root
    Root.parts.push_back(new GData(0,0,0,Root));
    Root.parts[0]->boxes.push_back(Level.boxes[0]);
    Level.boxes[0]->part  = 0;
    int L = 1;
    for (uint32_t gi=0;gi<ng;gi++){
      GData &node = (*this)(L,gi);
      for ( uint32_t ci = 0; ci<np;ci++)
	node.parts.push_back(new GData (L,gi,ci,node) );
      int nb=node.boxes.size();
      int bpp = nb/np+1;
      for(int bi=0;bi<nb;bi++){
	Box *b=node.boxes[bi];
	node.parts[bi/bpp]->boxes.push_back(b);
	b->part = bi/bpp;
      }
    }
    for (L=2;L<L_max;L++){
      for (uint32_t gi=0;gi<ng;gi++){
	GData &cd = (*this)(L  ,gi);
	for ( uint32_t ci = 0; ci<np;ci++)
	  cd.parts.push_back(new GData (L,gi,ci,cd) );
	for(Box *cb: cd.boxes){
	  int pp=cb->parent->part;
	  cd.parts[pp]->boxes.push_back(cb);
	  cb->part = pp;
	}
      }
    }
  }
  */
  /*---------------------------------------------------*/
  /*
  void TopLayerData::make_hosts(){
    for (int i=0;i<M;i++){
      for(int j=0;j<N;j++){
	GData &T = (*this)(i,j);
	int npp = T.parts.size();
	int h;
	if ( npp > config.P){
	  h = npp % config.P ;	  
	}
	else{
	  h = ALL_HOST ;
	}
	T.setHost(h);
	for ( int k =0;k<npp;k++){
	  GData &pp=T[k];	  
	  T[k].setHost(h);
	}
      }
    }
  }
  */
  /*---------------------------------------------------*/
  bool is_box_in_list(BoxList &boxes, Box * b){
    for ( Box * lb : boxes){
      if ( b->index == lb-> index )
	return true;
    }
    return false;
  }
  /*---------------------------------------------------*/
  void for_each_pair_in_batch(X2YList *b, BoxPairFcn func,string s){
    X2YList &batch=*b;
    for (uint32_t i=0;i<batch.size();i++){
      func(s, batch[i]);
    }
  }
  /*---------------------------------------------------*/
  void print_box_pair(string prefix , BoxPair *bp){
    cout << prefix << " " ;
    cout << "B("<< bp->first->level << "," << bp->first->index <<")--B(";
    cout << bp->second->level << "," << bp->second->index <<")\n";
  }
  /*---------------------------------------------------*/
  void for_each_box_in_batch(BoxList *b, BoxFcn func,string s){
    BoxList &batch=*b;
    for (uint32_t i=0;i<batch.size();i++){
      func(s,batch[i]);
    }
  }
  /*---------------------------------------------------*/
  void print_box(string prefix , Box *bp){
    cout << prefix << " " ;
    cout << "B("<< bp->level << "," << bp->index <<")\n";
  }

    GBoxTree*convert_tree(Tree *t){
        GBoxTree* bt=new GBoxTree;
        return bt;
    }

   void fx(){
    GBoxTree *bt = convert_tree(tree);
    GTreeData F(bt);

    int L=bt->level_list.size();
    int g=F.getGroupCount(L);
    for(int gi = 0;gi<g; gi++){
        fmm_engine->add_task( new FFLTask (F(L,gi)->getChildXData(0),F(L,gi)->getParentXData(),nullptr));
    }

    XData * f=F(0,0);
    XData *p = f->getParentXData();
    p=F(0,0)->getParentXData();
    (void)p;
   }

   void make_group(GBoxTree *bt, GGroupTree *gt){
       XBox *root=(*bt)(0,0);
       XData *node =new XData;
       gt->addLevel()<<node;
       gt->addLevel().addNode(node);
       root->object->second = node;
       XBox *B=(*bt)(1,0);

       if (B->getParentBox()== node->getParentBox() )
        node = new XData;
        node->boxes.push_back(B->getBox());
        B->setXData(node);
        node->parent = B->getParentXData();
        node->parent->children.push_back(node);
    /*
    FFL Xi, F
    C2P F.children[i], F
    XLT Fi,Gj
    C2P G.children[i],G
    P2C G,G.children[i]
    RCV Gi,Vi

    Box{
        parent
        child_list
        node
    }
    Node{
       BList
       parent
       child_nodes
    }
     make the Root
     node = new Node
     Root.node = node
     node << Root


    for L in 1:Levels
        node = new Node
        for B in L
            if B.parent != node.parent && node.parent != null
                node = new Node
            node << B
            B.node = node
            node.parent = B.parent.node
            node.parent.children.add(node)
            if nb>BPG node=new Node
    */
   }
}
