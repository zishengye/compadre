#include "Compadre_LinearAlgebra_Definitions.hpp"
#include "Compadre_Functors.hpp"

#include "KokkosBatched_Copy_Decl.hpp"
#include "KokkosBatched_ApplyPivot_Decl.hpp"
#include "KokkosBatched_Gemv_Decl.hpp"
#include "KokkosBatched_Trsv_Decl.hpp"
#include "KokkosBatched_UTV_Decl.hpp"
#include "KokkosBatched_SolveUTV_Decl_Compadre.hpp"

using namespace KokkosBatched;

namespace Compadre{
namespace GMLS_LinearAlgebra {

  template<typename DeviceType,
           typename AlgoTagType,
           typename MatrixViewType,
           typename PivViewType,
           typename VectorViewType>
  struct Functor_TestBatchedTeamVectorSolveUTV {
    MatrixViewType _a;
    VectorViewType _b;

    int _pm_getTeamScratchLevel_0;
    int _pm_getTeamScratchLevel_1;
    int _M, _N, _NRHS;

    KOKKOS_INLINE_FUNCTION
    Functor_TestBatchedTeamVectorSolveUTV(
                      const int M,
                      const int N,
                      const int NRHS,
                      const MatrixViewType &a,
                      const VectorViewType &b)
      : _a(a), _b(b), _M(M), _N(N), _NRHS(NRHS) { _pm_getTeamScratchLevel_0 = 0; _pm_getTeamScratchLevel_1 = 0; }

    template<typename MemberType>
    KOKKOS_INLINE_FUNCTION
    void operator()(const MemberType &member) const {

      const int k = member.league_rank();

      // workspace vectors
      scratch_vector_type ww_fast(member.team_scratch(_pm_getTeamScratchLevel_0), 3*_M);
      scratch_vector_type ww_slow(member.team_scratch(_pm_getTeamScratchLevel_1), _N*_NRHS);

      scratch_matrix_right_type aa(_a.data() + TO_GLOBAL(k)*TO_GLOBAL(_a.extent(1))*TO_GLOBAL(_a.extent(2)), 
              _a.extent(1), _a.extent(2));
      scratch_matrix_right_type bb(_b.data() + TO_GLOBAL(k)*TO_GLOBAL(_b.extent(1))*TO_GLOBAL(_b.extent(2)), 
              _b.extent(1), _b.extent(2));

      // if sizes don't match extents, then copy to a view with extents matching sizes
      if (_M!=_a.extent(1) || _N!=_a.extent(2)) {
        scratch_matrix_right_type tmp(ww_slow.data(), _M, _N);
        auto aaa = scratch_matrix_right_type(_a.data() + TO_GLOBAL(k)*TO_GLOBAL(_a.extent(1))*TO_GLOBAL(_a.extent(2)), _M, _N);
        // copy A to W, then back to A
        Kokkos::parallel_for(Kokkos::TeamThreadRange(member,0,_M),[&](const int &i) {
          Kokkos::parallel_for(Kokkos::ThreadVectorRange(member,0,_N),[&](const int &j) {
              tmp(i,j) = aa(i,j);
          });
        });
        member.team_barrier();
        Kokkos::parallel_for(Kokkos::TeamThreadRange(member,0,_M),[&](const int &i) {
          Kokkos::parallel_for(Kokkos::ThreadVectorRange(member,0,_N),[&](const int &j) {
              aaa(i,j) = tmp(i,j);
          });
        });
        member.team_barrier();
        aa = aaa;
      }

      if (_M==_N) {
        scratch_matrix_right_type tmp(ww_slow.data(), _N, _NRHS);
        // coming from LU
        // then copy B to W, then back to B
        auto bb_left = 
            scratch_matrix_left_type(_b.data() + TO_GLOBAL(k)*TO_GLOBAL(_b.extent(1))*TO_GLOBAL(_b.extent(2)), 
                    _b.extent(1), _b.extent(2));
        Kokkos::parallel_for(Kokkos::TeamThreadRange(member,0,_N),[&](const int &i) {
          Kokkos::parallel_for(Kokkos::ThreadVectorRange(member,0,_NRHS),[&](const int &j) {
              tmp(i,j) = bb_left(i,j);
          });
        });
        member.team_barrier();
        Kokkos::parallel_for(Kokkos::TeamThreadRange(member,0,_N),[&](const int &i) {
          Kokkos::parallel_for(Kokkos::ThreadVectorRange(member,0,_NRHS),[&](const int &j) {
              bb(i,j) = tmp(i,j);
          });
        });
      }

      scratch_matrix_right_type uu(member.team_scratch(_pm_getTeamScratchLevel_1), _M, _N /* only N columns of U are filled, maximum */);
      scratch_matrix_right_type vv(member.team_scratch(_pm_getTeamScratchLevel_1), _N, _N);
      scratch_local_index_type pp(member.team_scratch(_pm_getTeamScratchLevel_0), _N);

      bool do_print = false;
      if (do_print) {
        Kokkos::single(Kokkos::PerTeam(member), [&] () {
          //print a
          printf("a=zeros(%lu,%lu);\n", aa.extent(0), aa.extent(1));
              for (int j=0; j<aa.extent(0); ++j) {
                  for (int k=0; k<aa.extent(1); ++k) {
                      printf("a(%d,%d)= %f;\n", j+1,k+1, aa(j,k));
                  }
              }
          //print b
          printf("b=zeros(%lu,%lu);\n", bb.extent(0), bb.extent(1));
              for (int j=0; j<bb.extent(0); ++j) {
                  for (int k=0; k<bb.extent(1); ++k) {
                      printf("b(%d,%d)= %f;\n", j+1,k+1, bb(j,k));
                  }
              }
        });
      }
      do_print = false;

      /// Solving Ax = b using UTV transformation
      /// A P^T P x = b
      /// UTV P x = b;

      /// UTV = A P^T
      int matrix_rank(0);
      member.team_barrier();
      TeamVectorUTV<MemberType,AlgoTagType>
        ::invoke(member, aa, pp, uu, vv, ww_fast, matrix_rank);
      member.team_barrier();

      if (do_print) {
        Kokkos::single(Kokkos::PerTeam(member), [&] () {
        printf("matrix_rank: %d\n", matrix_rank);
        //print u
        printf("u=zeros(%lu,%lu);\n", uu.extent(0), uu.extent(1));
        for (int j=0; j<uu.extent(0); ++j) {
            for (int k=0; k<uu.extent(1); ++k) {
                printf("u(%d,%d)= %f;\n", j+1,k+1, uu(j,k));
            }
        }
        });
      }
      TeamVectorSolveUTVCompadre<MemberType,AlgoTagType>
        ::invoke(member, matrix_rank, _M, _N, _NRHS, uu, aa, vv, pp, bb, bb, ww_slow, ww_fast);
      member.team_barrier();

    }

    inline
    void run(ParallelManager pm) {
      typedef typename MatrixViewType::non_const_value_type value_type;
      std::string name_region("KokkosBatched::Test::TeamVectorSolveUTVCompadre");
      std::string name_value_type = ( std::is_same<value_type,float>::value ? "::Float" :
                                      std::is_same<value_type,double>::value ? "::Double" :
                                      std::is_same<value_type,Kokkos::complex<float> >::value ? "::ComplexFloat" :
                                      std::is_same<value_type,Kokkos::complex<double> >::value ? "::ComplexDouble" : "::UnknownValueType" );
      std::string name = name_region + name_value_type;
      Kokkos::Profiling::pushRegion( name.c_str() );

      _pm_getTeamScratchLevel_0 = pm.getTeamScratchLevel(0);
      _pm_getTeamScratchLevel_1 = pm.getTeamScratchLevel(1);
      
      int scratch_size = scratch_matrix_right_type::shmem_size(_N, _N); // V
      scratch_size += scratch_matrix_right_type::shmem_size(_M, _N /* only N columns of U are filled, maximum */); // U
      scratch_size += scratch_vector_type::shmem_size(_N*_NRHS); // W (for SolveUTV)

      int l0_scratch_size = scratch_vector_type::shmem_size(_N); // P (temporary)
      l0_scratch_size += scratch_vector_type::shmem_size(3*_M); // W (for UTV)

      pm.clearScratchSizes();
      pm.setTeamScratchSize(0, l0_scratch_size);
      pm.setTeamScratchSize(1, scratch_size);

      pm.CallFunctorWithTeamThreadsAndVectors(*this, _a.extent(0));
      Kokkos::fence();

      Kokkos::Profiling::popRegion();
    }
  };



void batchQRFactorize(ParallelManager pm, double *P, int lda, int nda, double *RHS, int ldb, int ndb, int M, int N, int NRHS, const int num_matrices) {

    //printf("lda: %d, nda %d, ldb %d, ndb %d, M %d, N %d, NRHS %d\n", lda, nda, ldb, ndb, M, N, NRHS);
    typedef Algo::UTV::Unblocked algo_tag_type;
    typedef Kokkos::View<double***, layout_right, Kokkos::MemoryTraits<Kokkos::Unmanaged> >
                    MatrixViewType;

    MatrixViewType A(P, num_matrices, lda, nda);

    typedef Kokkos::View<int**, layout_right, Kokkos::MemoryTraits<Kokkos::Unmanaged> >
                    PivViewType;

    typedef Kokkos::View<double***, layout_right, Kokkos::MemoryTraits<Kokkos::Unmanaged> >
                    VectorViewType;
    VectorViewType B(RHS, num_matrices, ldb, ndb);

    Functor_TestBatchedTeamVectorSolveUTV
      <device_execution_space, algo_tag_type, MatrixViewType, PivViewType, VectorViewType>(M,N,NRHS,A,B).run(pm);

}

void batchSVDFactorize(ParallelManager pm, double *P, int lda, int nda, double *RHS, int ldb, int ndb, int M, int N, int NRHS, const int num_matrices) {

    //printf("lda: %d, nda %d, ldb %d, ndb %d, M %d, N %d, NRHS %d\n", lda, nda, ldb, ndb, M, N, NRHS);
    typedef Algo::UTV::Unblocked algo_tag_type;
    typedef Kokkos::View<double***, layout_right, Kokkos::MemoryTraits<Kokkos::Unmanaged> >
                    MatrixViewType;

    MatrixViewType A(P, num_matrices, lda, nda);

    typedef Kokkos::View<int**, layout_right, Kokkos::MemoryTraits<Kokkos::Unmanaged> >
                    PivViewType;


    typedef Kokkos::View<double***, layout_right, Kokkos::MemoryTraits<Kokkos::Unmanaged> >
                    VectorViewType;
    VectorViewType B(RHS, num_matrices, ldb, ndb);
    Functor_TestBatchedTeamVectorSolveUTV
      <device_execution_space, algo_tag_type, MatrixViewType, PivViewType, VectorViewType>(M,N,NRHS,A,B).run(pm);

}

void batchLUFactorize(ParallelManager pm, double *P, int lda, int nda, double *RHS, int ldb, int ndb, int M, int N, int NRHS, const int num_matrices) {

    //printf("lda: %d, nda %d, ldb %d, ndb %d, M %d, N %d, NRHS %d\n", lda, nda, ldb, ndb, M, N, NRHS);
    typedef Algo::UTV::Unblocked algo_tag_type;
    typedef Kokkos::View<double***, layout_right, Kokkos::MemoryTraits<Kokkos::Unmanaged> >
                    MatrixViewType;

    MatrixViewType A(P, num_matrices, lda, nda);

    typedef Kokkos::View<int**, layout_right, host_execution_space>
                    PivViewType;

    typedef Kokkos::View<double***, layout_left, Kokkos::MemoryTraits<Kokkos::Unmanaged> >
                    VectorViewType;
    VectorViewType B(RHS, num_matrices, ldb, ndb);

    Functor_TestBatchedTeamVectorSolveUTV
      <device_execution_space, algo_tag_type, MatrixViewType, PivViewType, VectorViewType>(M,N,NRHS,A,B).run(pm);

}

} // GMLS_LinearAlgebra
} // Compadre
