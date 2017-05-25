#include "setup.h"
#include <cassert>
#include <complex>
#include <random>
using std::vector;

namespace FMM_3D{
    void import_boxes_binary(const string fn){
        uint32_t L,l,nb,nc,nf,ff;
        FILE *f=fopen(fn.c_str(),"rb");

        fread(&L ,sizeof(uint32_t),1,f);

        K = new int[L];
        M = new int[L];
        L_max=L;

        for(uint32_t ii=0;ii<L;ii++)
            tree->Level.push_back(new LevelBase);
        //tree->Level.resize(L);


        for(uint32_t i=0;i<L;i++){
            fread(&l ,sizeof(uint32_t),1,f);
            assert(l==i+1);
            //LevelBase *ll=new LevelBase();
            //tree->Level[i] = ll;
            fread(&nb,sizeof(uint32_t),1,f);
            tree->Level[i]->boxes.resize(nb);
            for(uint32_t ii=0;ii<nb;ii++){
                Box *b = new Box();
                tree->Level[i]->boxes[ii] = b;
            }
            M[i]=nb;
            for(uint32_t b=0;b<nb;b++){
                Box &B=*tree->Level[i]->boxes[b];
                B.level = i+1;
                B.index = b+1;
                fread(&nc,sizeof(uint32_t),1  ,f);
                fread( &B.center,sizeof(ElementType)  ,3  ,f);
                fread( &B.diagonal,sizeof(ElementType)  ,1  ,f);

                B.children_idx.resize(nc);
                fread( &B.children_idx[0] ,sizeof(uint32_t),nc ,f);

                fread(&ff,sizeof(uint32_t),1  ,f);
                if (ff){
                    B.ff_int_list_idx.resize(ff);
                    fread( &B.ff_int_list_idx[0] ,sizeof(uint32_t),ff ,f);
                    for(auto bb:B.ff_int_list_idx){
                        Box *bx = tree->Level[i]->boxes[bb-1];
                        assert(bx);
                        B.ff_int_list.push_back(bx);
                    }
                }

                fread(&nf,sizeof(uint32_t),1  ,f);
                if(nf){
                    B.nf_int_list_idx.resize(nf);
                    fread( &B.nf_int_list_idx[0] ,sizeof(uint32_t),nf ,f);
                    for(auto bb:B.nf_int_list_idx){
                        Box *bx=tree->Level[i]->boxes[bb-1];
                        assert(bx);
                        B.nf_int_list.push_back (bx);
                    }
                }

            }
            if (i != 0){
                BoxList &b1=tree->Level[i-1]->boxes;
                BoxList &b2=tree->Level[i  ]->boxes;
                for(uint ib1=0;ib1<b1.size();ib1++){
                    for(uint j=0;j<b1[ib1]->children_idx.size();j++){
                        int cidx=b1[ib1]->children_idx[j];
                        b1[ib1]->children.push_back(b2[cidx-1]);
                    }
                }
            }
        }
        fclose(f);
    }
    /*---------------------------------------------------------------------------*/

    void import_translators(const string fn){
        FILE *f=fopen(fn.c_str(),"rb");
        int L,level;
        fread(&L,sizeof(uint32_t),1,f);
        for(int i =0; i<L;i++){
            fread(&level,sizeof(uint32_t),1,f);
            TList &t=tree->Level[level-1]->T;
            uint32_t nd,M,N,d[3];
            ElementType *Trans;
            fread(&nd,sizeof(uint32_t),1,f);
            for ( uint32_t j=0;j<nd;j++){
                fread(&d[0],sizeof(uint32_t),3,f);
                fread(&M,sizeof(uint32_t),1,f);
                fread(&N,sizeof(uint32_t),1,f);
                Trans = new ElementType[M*N];
                fread(Trans,sizeof(ElementType),M*N,f);
                t.push_back(new Translator(d, M,N,Trans));
            }
        }
        fclose(f);
    }
    /*---------------------------------------------------------------------------*/
    void import_interpolators(const string fn){
        FILE *f=fopen(fn.c_str(),"rb");
        uint32_t N,level,n;
        SparseMat *A1,*A2,*B1,*B2;
        fread(&N,sizeof(uint32_t),1,f);
        for(uint32_t i=0;i<N;i++){
            fread(&level,sizeof(uint32_t),1,f);
            fread(&n,sizeof(uint32_t),1,f);
            A1 = new SparseMat;
            A1->index.resize(n);A1->data.resize(n);
            fread(&A1->index[0],sizeof(uint32_t),n*2,f);
            fread(&A1->data[0],sizeof(ElementType),n,f);

            fread(&n,sizeof(uint32_t),1,f);
            A2 = new SparseMat;
            A2->index.resize(n);A2->data.resize(n);
            fread(&A2->index[0],sizeof(uint32_t),n*2,f);
            fread(&A2->data[0],sizeof(ElementType),n,f);

            fread(&n,sizeof(uint32_t),1,f);
            B1= new SparseMat;
            B1->index.resize(n);B1->data.resize(n);
            fread(&B1->index[0],sizeof(uint32_t),n*2,f);
            fread(&B1->data[0],sizeof(ElementType),n,f);

            fread(&n,sizeof(uint32_t),1,f);
            B2 =new SparseMat;
            B2->index.resize(n);B2->data.resize(n);
            fread(&B2->index[0],sizeof(uint32_t),n*2,f);
            fread(&B2->data[0],sizeof(ElementType),n,f);

            LevelBase &L=*tree->Level[level-1];
            L.P2C = new Interpolation(A1,A2);
            L.C2P = new Interpolation(B1,B2);

        }
        fclose(f);
    }
    /*--------------------------------------------------------------------------------*/
    void import_kappa(const string fn){
        FILE *f=fopen(fn.c_str(),"rb");
        uint32_t L,level,n;
        fread(&L,sizeof(uint32_t),1,f);
        for(uint32_t i=0;i<L;i++){
            fread(&level,sizeof(uint32_t),1,f);
            LevelBase &Level=*tree->Level[level-1];

            fread(&Level.K_rows,sizeof(uint32_t),1,f);
            fread(&Level.K_cols,sizeof(uint32_t),1,f);
            n = Level.K_cols*Level.K_rows;
            Level.K_theta = new ElementType[n];
            K[level-1] = n;
            fread(Level.K_theta ,sizeof(ElementType),n,f);
            Level.K_phi = new ElementType[n];
            fread(Level.K_phi ,sizeof(ElementType),n,f);
        }
        fclose(f);
    }
    /*--------------------------------------------------------------------------------*/
    void import_Z(const string fn){
        FILE *f = fopen(fn.c_str(),"rb");
        uint32_t nb,level,M,N,box,near;
        level = tree->Level.size(); // The last level
        LevelBase &Level=*tree->Level[level-1];
        fread (&nb,sizeof(uint32_t),1,f);
        while (!feof(f)){
            fread (&box,sizeof(uint32_t),1,f);
            fread (&near,sizeof(uint32_t),1,f);
            fread (&M,sizeof(uint32_t),1,f);
            fread (&N,sizeof(uint32_t),1,f);
            Z_near * Z = new Z_near(M,N,true);
            fread (Z->data,sizeof(std::complex<ElementType>),M*N,f);
            Box &bBox=*Level.boxes[box-1];
            bBox.edges = M;
            bBox.Z.indx.push_back(near);
            bBox.Z.data.push_back(Z);
        }
        fclose(f);

    }
    /*--------------------------------------------------------------------------------*/
    void create_I(){
        uint32_t nb,level,M,N;
        level = tree->Level.size(); // The last level
        LevelBase &Level=*tree->Level[level-1];
        nb = Level.boxes.size();
        for ( uint32_t b=0;b<nb;b++){
            Box &box = *Level.boxes[b];
            M = box.edges;
            N = 1;
            I_vect *I = new I_vect(M,N,true);
            for ( uint32_t i=0;i<M;i++){
                for (uint32_t j=0;j<N;j++){
                    I->data[j*M+i] = std::rand();///std::RAND_MAX;
                }
            }
            box.I = I;
        }
    }
    /*--------------------------------------------------------------------------------*/
    void import_F(const string fn){
        FILE *f = fopen(fn.c_str(),"rb");
        uint32_t nb,level,M,N,box;
        level = tree->Level.size(); // The last level
        LevelBase &Level=*tree->Level[level-1];
        fread (&nb,sizeof(uint32_t),1,f);
        for(uint32_t i=0;i<nb;i++){
            fread (&box,sizeof(uint32_t),1,f);
            fread (&M,sizeof(uint32_t),1,f);
            fread (&N,sizeof(uint32_t),1,f);
            F_far *F = new F_far (M,N,true);
            fread (F->data,sizeof(ElementType),M*N,f);
            Box &bBox=*Level.boxes[box-1];
            bBox.F = F ;
        }
        fclose(f);
    }
    /*--------------------------------------------------------------------------------*/
    void import_R(const string fn){
        FILE *f = fopen(fn.c_str(),"rb");
        uint32_t nb,level,M,N,box;
        level = tree->Level.size(); // The last level
        LevelBase &Level=*tree->Level[level-1];
        fread (&nb,sizeof(uint32_t),1,f);
        for(uint32_t i=0;i<nb;i++){
            fread (&box,sizeof(uint32_t),1,f);
            fread (&M,sizeof(uint32_t),1,f);
            fread (&N,sizeof(uint32_t),1,f);
            Receiving *R = new Receiving (M,N,true);
            fread (R->data,sizeof(ElementType),M*N,f);
            Box &bBox=*Level.boxes[box-1];
            bBox.R = R ;
        }
        fclose(f);
    }
    /*--------------------------------------------------------------------------------*/
    void create_V(){
        uint32_t nb,level,M,N;
        level = tree->Level.size(); // The last level
        LevelBase &Level=*tree->Level[level-1];
        nb = Level.boxes.size();
        for(uint32_t i=0;i<nb;i++){
            GeneralArray *I=Level.boxes[i]->I;
            M = I->M;
            N = I->N;
            V_vect * V = new V_vect(M,N,true);
            Level.boxes[i]->V = V ;
        }
    }
    /*--------------------------------------------------------------------------------*/
    void create_Far_fields(){
        uint32_t L,n;
        L = L_max;
        for(uint32_t i=0;i<L;i++){
            n = K[i] ;
            uint32_t nb = tree->Level[i]->boxes.size();
            for(uint32_t b=0;b<nb;b++){
                Box &box=*tree->Level[i]->boxes[b];
                box.Ft = new F_far_tilde(n,1,true);
                box.G  = new Green(n,1,true);
                box.E  = new Exponential(n,1,true);
            }
        }
    }
    /*--------------------------------------------------------------------------------*/
    /*--------------------------------------------------------------------------------*/

}
