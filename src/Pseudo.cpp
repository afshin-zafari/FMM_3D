#include <vector>
int L_max;
struct Point { double x,y,z;};
typedef vector<Box*> BoxList;
struct Box{
  Point    cemter;
  int      level,index;
  BoxList  nf_int_list;
  
  Box(int level_ , int index_){
  }
};
struct Z_near{
  Z_near(Box b1, Box b2){}
};
struct GeneralVector{
  int size;
};
typedef GeneralVector I_vect;
typedef GeneralVector V_vect;

int box_count(int level){return  1;}//TODO

void FMM_main(){
  int L  = L_max;
  for (k=0;k< box_count(L);k++ ){
    Box b_k(L,k);
    BoxList nf = b_k.nf_int_list;
    for ( int i=0;i< nf.size();i++){
      Box &b_l = *nf[i];      
      Z_near Z_kl(b_k,b_l);
      I_vect I_l(b_l);
      V_vect V_k (b_k);
      MVP_ZI_to_V(Z_kl,I_l,V_k);    
    }    
  }
}
int main()
{
}
