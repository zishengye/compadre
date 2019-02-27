#ifndef _COMPADRE_GMLS_HPP_
#define _COMPADRE_GMLS_HPP_

#include "Compadre_Config.h"
#include "Compadre_Typedefs.hpp"

#include "Compadre_Misc.hpp"
#include "Compadre_Operators.hpp"
#include "Compadre_LinearAlgebra_Definitions.hpp"


namespace Compadre {

//!  Generalized Moving Least Squares (GMLS)
/*!
*  This class sets up a batch of GMLS problems from a given set of neighbor lists, target sites, and source sites.
*  GMLS requires a target functional, reconstruction space, and sampling functional to be specified. 
*  For a given choice of reconstruction space and sampling functional, multiple targets can be generated with very little
*  additional computation, which is why this class allows for multiple target functionals to be specified. 
*/
class GMLS {
protected:

    // random numbe generator pool
    pool_type _random_number_pool;

    // matrices that may be needed for matrix factorization on the device
    // supports batched matrix factorization dispatch

    //! contains weights for all problems
    Kokkos::View<double*> _w; 

    //! P*sqrt(w) matrix for all problems
    Kokkos::View<double*> _P;

    //! sqrt(w)*Identity matrix for all problems, later holds polynomial coefficients for all problems
    Kokkos::View<double*> _RHS;

    //! Rank 3 tensor for high order approximation of tangent vectors for all problems. First rank is
    //! for the target index, the second is for the local direction to the manifolds 0..(_dimensions-1)
    //! are tangent, _dimensions is the normal, and the third is for the spatial dimension (_dimensions)
    Kokkos::View<double*> _T;

    //! tangent vectors information (host)
    Kokkos::View<double*>::HostMirror _host_T;

    //! _dimensions columns contains high order approximation of normal vector for all problems. This
    //! can be set by the user or calculated automatically in the case of manifold problems.
    Kokkos::View<double**> _N;

    //! metric tensor inverse for all problems
    Kokkos::View<double*> _manifold_metric_tensor_inverse;

    //! curvature polynomial coefficients for all problems
    Kokkos::View<double*> _manifold_curvature_coefficients;

    //! _dimension-1 gradient values for curvature for all problems
    Kokkos::View<double*> _manifold_curvature_gradient;

    
    //! contains local IDs of neighbors to get coordinates from _source_coordinates (device)
    Kokkos::View<int**, layout_type> _neighbor_lists; 

    //! contains local IDs of neighbors to get coordinates from _source_coordinates (host)
    Kokkos::View<int**, layout_type>::HostMirror _host_neighbor_lists;

    //! contains the # of neighbors for each target (host)
    Kokkos::View<int*, Kokkos::HostSpace> _number_of_neighbors_list; 

    //! all coordinates for the source for which _neighbor_lists refers (device)
    Kokkos::View<double**, layout_type> _source_coordinates; 

    //! all coordinates for the source for which _neighbor_lists refers (host)
    Kokkos::View<double**, layout_type>::HostMirror _host_source_coordinates; 

    //! coordinates for target sites for reconstruction same number of rows as _neighbor_lists (device)
    Kokkos::View<double**, layout_type> _target_coordinates; 

    //! coordinates for target sites for reconstruction same number of rows as _neighbor_lists (host)
    Kokkos::View<double**, layout_type>::HostMirror _host_target_coordinates; // same number of rows as _neighbor_lists

    //! h supports determined through neighbor search, same number of rows as _neighbor_lists (device)
    Kokkos::View<double*> _epsilons; 

    //! h supports determined through neighbor search, same number of rows as _neighbor_lists (host)
    Kokkos::View<double*>::HostMirror _host_epsilons; 

    //! generated alpha coefficients (device)
    Kokkos::View<double**, layout_type> _alphas; 

    //! generated alpha coefficients (host)
    Kokkos::View<const double**, layout_type>::HostMirror _host_alphas;
    
    //! generated weights for nontraditional samples required to transform data into expected sampling 
    //! functional form (device). 
    Kokkos::View<double*****, layout_type> _prestencil_weights; 

    //! generated weights for nontraditional samples required to transform data into expected sampling 
    //! functional form (host)
    Kokkos::View<const double*****, layout_type>::HostMirror _host_prestencil_weights;

    //! (OPTIONAL) user provided additional coordinates for target operation evaluation (device)
    Kokkos::View<double**, layout_type> _additional_evaluation_coordinates; 

    //! (OPTIONAL) contains indices of entries in the _additional_evaluation_coordinates view (device)
    Kokkos::View<int**, layout_type> _additional_evaluation_indices; 

    //! (OPTIONAL) contains indices of entries in the _additional_evaluation_coordinates view (host)
    Kokkos::View<int**, layout_type>::HostMirror _host_additional_evaluation_indices;

    //! (OPTIONAL) contains the # of additional coordinate indices for each target (host)
    Kokkos::View<int*, Kokkos::HostSpace> _number_of_additional_evaluation_indices; 


    //! reconstruction type
    int _type; 

    //! order of basis for polynomial reconstruction
    int _poly_order; 

    //! order of basis for curvature reconstruction
    int _curvature_poly_order;

    //! dimension of basis for polynomial reconstruction
    int _NP;

    //! spatial dimension of the points, set at class instantiation only
    int _global_dimensions;

    //! dimension of the problem, set at class instantiation only. For manifolds, generally _global_dimensions-1
    int _local_dimensions;

    //! dimension of the problem, set at class instantiation only
    int _dimensions;

    //! reconstruction space for GMLS problems, set at GMLS class instantiation
    ReconstructionSpace _reconstruction_space;

    //! actual rank of reconstruction basis
    int _reconstruction_space_rank;

    //! polynomial sampling functional used to construct P matrix, set at GMLS class instantiation
    SamplingFunctional _polynomial_sampling_functional;

    //! generally the same as _polynomial_sampling_functional, but can differ if specified at 
    //! GMLS class instantiation
    SamplingFunctional _data_sampling_functional;

    //! vector containing target functionals to be applied for curvature
    Kokkos::View<TargetOperation*> _curvature_support_operations;

    //! vector containing target functionals to be applied for reconstruction problem (device)
    Kokkos::View<TargetOperation*> _operations;

    //! vector containing target functionals to be applied for reconstruction problem (host)
    Kokkos::View<TargetOperation*>::HostMirror _host_operations;



    //! 1D quadrature weights for staggered approaches
    Kokkos::View<double*, layout_type> _quadrature_weights;

    //! 1D quadrature sites (reference [0,1]) for staggered approaches
    Kokkos::View<double*, layout_type> _parameterized_quadrature_sites;



    //! solver type for GMLS problem, can also be set to MANIFOLD for manifold problems
    DenseSolverType _dense_solver_type;

    //! weighting kernel type for GMLS
    WeightingFunctionType _weighting_type;

    //! weighting kernel type for curvature problem
    WeightingFunctionType _curvature_weighting_type;

    //! power to be used for weighting kernel
    int _weighting_power;

    //! power to be used for weighting kernel for curvature
    int _curvature_weighting_power;

    //! dimension of the reconstructed function 
    //! e.g. reconstruction of vector on a 2D manifold in 3D would have _basis_multiplier of 2
    int _basis_multiplier;

    //! dimension of the sampling functional
    //! e.g. reconstruction of vector on a 2D manifold in 3D would have _basis_multiplier of 2
    int _sampling_multiplier;

    //! determined by 1D quadrature rules
    int _number_of_quadrature_points;

    //! whether or not operator to be inverted for GMLS problem has a nontrivial nullspace (requiring SVD)
    bool _nontrivial_nullspace;

    //! whether or not the orthonormal tangent directions were provided by the user. If they are not,
    //! then for the case of calculations on manifolds, a GMLS approximation of the tangent space will
    //! be made and stored for use.
    bool _orthonormal_tangent_space_provided; 



    //! vector of user requested target operations
    std::vector<TargetOperation> _lro; 

    //! vector containing a mapping from a target functionals enum value to the its place in the list
    //! of target functionals to be applied
    std::vector<int> _lro_lookup; 

    //! index for where this operation begins the for _alpha coefficients (device)
    Kokkos::View<int*, layout_type> _lro_total_offsets; 

    //! index for where this operation begins the for _alpha coefficients (host)
    Kokkos::View<int*, layout_type>::HostMirror _host_lro_total_offsets; 

    //! dimensions ^ rank of tensor of output for each target functional (device)
    Kokkos::View<int*, layout_type> _lro_output_tile_size; 

    //! dimensions ^ rank of tensor of output for each target functional (host)
    Kokkos::View<int*, layout_type>::HostMirror _host_lro_output_tile_size; 

    //! dimensions ^ rank of tensor of output for each sampling functional (device)
    Kokkos::View<int*, layout_type> _lro_input_tile_size; 

    //! dimensions ^ rank of tensor of output for each sampling functional (host)
    Kokkos::View<int*, layout_type>::HostMirror _host_lro_input_tile_size; 

    //! tensor rank of target functional (device)
    Kokkos::View<int*, layout_type> _lro_output_tensor_rank;

    //! tensor rank of target functional (host)
    Kokkos::View<int*, layout_type>::HostMirror _host_lro_output_tensor_rank;

    //! tensor rank of sampling functional (device)
    Kokkos::View<int*, layout_type> _lro_input_tensor_rank;

    //! tensor rank of sampling functional (host)
    Kokkos::View<int*, layout_type>::HostMirror _host_lro_input_tensor_rank;

    //! used for sizing P_target_row and the _alphas view
    int _total_alpha_values;




    //! lowest level memory for Kokkos::parallel_for for team access memory
    int _scratch_team_level_a;
    int _team_scratch_size_a;

    //! higher (slower) level memory for Kokkos::parallel_for for team access memory
    int _scratch_thread_level_a;
    int _thread_scratch_size_a;

    //! lowest level memory for Kokkos::parallel_for for thread access memory
    int _scratch_team_level_b;
    int _team_scratch_size_b;

    //! higher (slower) level memory for Kokkos::parallel_for for thread access memory
    int _scratch_thread_level_b;
    int _thread_scratch_size_b;

    //! calculated number of threads per team
    int _threads_per_team;





/** @name Private Modifiers
 *  Private function because information lives on the device
 */
///@{

    //! Calls a parallel_for using the tag given as the first argument.
    //! parallel_for will break out over loops over teams with each vector lane executing code be default
    template<class Tag>
    void CallFunctorWithTeamThreadsAndVectors(const int threads_per_team, const int vector_lanes_per_thread, const int team_scratch_size_a, const int team_scratch_size_b, const int thread_scratch_size_a, const int thread_scratch_size_b) {
    if ( (_scratch_team_level_a != _scratch_team_level_b) && (_scratch_thread_level_a != _scratch_thread_level_b) ) {
            // all levels of each type need specified separately
            Kokkos::parallel_for(
                Kokkos::TeamPolicy<Tag>(_target_coordinates.dimension_0(), threads_per_team, vector_lanes_per_thread)
                .set_scratch_size(_scratch_team_level_a, Kokkos::PerTeam(team_scratch_size_a))
                .set_scratch_size(_scratch_team_level_b, Kokkos::PerTeam(team_scratch_size_b))
                .set_scratch_size(_scratch_thread_level_a, Kokkos::PerThread(thread_scratch_size_a))
                .set_scratch_size(_scratch_thread_level_b, Kokkos::PerThread(thread_scratch_size_b)),
                *this, typeid(Tag).name());
        } else if (_scratch_team_level_a != _scratch_team_level_b) {
            // scratch thread levels are the same
            Kokkos::parallel_for(
                Kokkos::TeamPolicy<Tag>(_target_coordinates.dimension_0(), threads_per_team, vector_lanes_per_thread)
                .set_scratch_size(_scratch_team_level_a, Kokkos::PerTeam(team_scratch_size_a))
                .set_scratch_size(_scratch_team_level_b, Kokkos::PerTeam(team_scratch_size_b))
                .set_scratch_size(_scratch_thread_level_a, Kokkos::PerThread(thread_scratch_size_a + thread_scratch_size_b)),
                *this, typeid(Tag).name());
        } else if (_scratch_thread_level_a != _scratch_thread_level_b) {
            // scratch team levels are the same
            Kokkos::parallel_for(
                Kokkos::TeamPolicy<Tag>(_target_coordinates.dimension_0(), threads_per_team, vector_lanes_per_thread)
                .set_scratch_size(_scratch_team_level_a, Kokkos::PerTeam(team_scratch_size_a + team_scratch_size_b))
                .set_scratch_size(_scratch_thread_level_a, Kokkos::PerThread(thread_scratch_size_a))
                .set_scratch_size(_scratch_thread_level_b, Kokkos::PerThread(thread_scratch_size_b)),
                *this, typeid(Tag).name());
        } else {
            // scratch team levels and thread levels are the same
            Kokkos::parallel_for(
                Kokkos::TeamPolicy<Tag>(_target_coordinates.dimension_0(), threads_per_team, vector_lanes_per_thread)
                .set_scratch_size(_scratch_team_level_a, Kokkos::PerTeam(team_scratch_size_a + team_scratch_size_b))
                .set_scratch_size(_scratch_thread_level_a, Kokkos::PerThread(thread_scratch_size_a + thread_scratch_size_b)),
                *this, typeid(Tag).name());
        }
    }

    //! Calls a parallel for using the tag given as the first argument. 
    //! parallel_for will break out over loops over teams with each thread executing code be default
    template<class Tag>
    void CallFunctorWithTeamThreads(const int threads_per_team, const int team_scratch_size_a, const int team_scratch_size_b, const int thread_scratch_size_a, const int thread_scratch_size_b) {
        // calls breakout over vector lanes with vector lane size of 1
        CallFunctorWithTeamThreadsAndVectors<Tag>(threads_per_team, 1, team_scratch_size_a, team_scratch_size_b, thread_scratch_size_a, thread_scratch_size_b);
    }

    /*! \brief Evaluates the polynomial basis under a particular sampling function. Generally used to fill a row of P.
        \param delta            [in/out] - scratch space that is allocated so that each thread has its own copy. Must be at least as large is the _basis_multipler*the dimension of the polynomial basis.
        \param target_index         [in] - target number
        \param neighbor_index       [in] - index of neighbor for this target with respect to local numbering [0,...,number of neighbors for target]
        \param alpha                [in] - double to determine convex combination of target and neighbor site at which to evaluate polynomials. (1-alpha)*neighbor + alpha*target
        \param dimension            [in] - spatial dimension of basis to evaluate. e.g. dimension two basis of order one is 1, x, y, whereas for dimension 3 it is 1, x, y, z
        \param poly_order           [in] - polynomial basis degree
        \param specific_order_only  [in] - boolean for only evaluating one degree of polynomial when true
        \param V                    [in] - orthonormal basis matrix size _dimensions * _dimensions whose first _dimensions-1 columns are an approximation of the tangent plane
        \param reconstruction_space [in] - space of polynomial that a sampling functional is to evaluate
        \param sampling_strategy    [in] - sampling functional specification
    */
    KOKKOS_INLINE_FUNCTION
    void calcPij(double* delta, const int target_index, int neighbor_index, const double alpha, const int dimension, const int poly_order, bool specific_order_only = false, scratch_matrix_type* V = NULL, const ReconstructionSpace reconstruction_space = ReconstructionSpace::ScalarTaylorPolynomial, const SamplingFunctional sampling_strategy = SamplingFunctional::PointSample, const int additional_evaluation_index = -1) const;

    /*! \brief Evaluates the gradient of a polynomial basis under the Dirac Delta (pointwise) sampling function.
        \param delta            [in/out] - scratch space that is allocated so that each thread has its own copy. Must be at least as large is the _basis_multipler*the dimension of the polynomial basis.
        \param target_index         [in] - target number
        \param neighbor_index       [in] - index of neighbor for this target with respect to local numbering [0,...,number of neighbors for target]
        \param alpha                [in] - double to determine convex combination of target and neighbor site at which to evaluate polynomials. (1-alpha)*neighbor + alpha*target
        \param partial_direction    [in] - direction that partial is taken with respect to, e.g. 0 is x direction, 1 is y direction
        \param dimension            [in] - spatial dimension of basis to evaluate. e.g. dimension two basis of order one is 1, x, y, whereas for dimension 3 it is 1, x, y, z
        \param poly_order           [in] - polynomial basis degree
        \param specific_order_only  [in] - boolean for only evaluating one degree of polynomial when true
        \param V                    [in] - orthonormal basis matrix size _dimensions * _dimensions whose first _dimensions-1 columns are an approximation of the tangent plane
        \param reconstruction_space [in] - space of polynomial that a sampling functional is to evaluate
        \param sampling_strategy    [in] - sampling functional specification
    */
    KOKKOS_INLINE_FUNCTION
    void calcGradientPij(double* delta, const int target_index, const int neighbor_index, const double alpha, const int partial_direction, const int dimension, const int poly_order, bool specific_order_only, scratch_matrix_type* V, const ReconstructionSpace reconstruction_space, const SamplingFunctional sampling_strategy) const;

    /*! \brief Evaluates the weighting kernel
        \param r                [in] - Euclidean distance of relative vector. Euclidean distance of (target - neighbor) in some basis.
        \param h                [in] - window size. Kernel is guaranteed to take on a value of zero if it exceeds h.
        \param weighting_type   [in] - weighting type to be evaluated as the kernel. e,g. power, Gaussian, etc..
        \param power            [in] - power parameter to be given to the kernel.
    */
    KOKKOS_INLINE_FUNCTION
    double Wab(const double r, const double h, const WeightingFunctionType& weighting_type, const int power) const; 
    
    /*! \brief Fills the _P matrix with either P or P*sqrt(w)
        \param teamMember           [in] - Kokkos::TeamPolicy member type (created by parallel_for)
        \param delta            [in/out] - scratch space that is allocated so that each thread has its own copy. Must be at least as large is the _basis_multipler*the dimension of the polynomial basis.
        \param P                   [out] - 2D Kokkos View which will contain evaluation of sampling functional on polynomial basis for each neighbor the target has (stored column major)
        \param w                   [out] - 1D Kokkos View which will contain weighting kernel values for the target with each neighbor if weight_p = true
        \param dimension            [in] - spatial dimension of basis to evaluate. e.g. dimension two basis of order one is 1, x, y, whereas for dimension 3 it is 1, x, y, z
        \param polynomial_order     [in] - polynomial basis degree
        \param weight_p             [in] - boolean whether to fill w with kernel weights
        \param V                    [in] - orthonormal basis matrix size _dimensions * _dimensions whose first _dimensions-1 columns are an approximation of the tangent plane
        \param reconstruction_space [in] - space of polynomial that a sampling functional is to evaluate
        \param sampling_strategy    [in] - sampling functional specification
    */
    KOKKOS_INLINE_FUNCTION
    void createWeightsAndP(const member_type& teamMember, scratch_vector_type delta, scratch_matrix_type P, scratch_vector_type w, const int dimension, int polynomial_order, bool weight_p = false, scratch_matrix_type* V = NULL, const ReconstructionSpace reconstruction_space = ReconstructionSpace::ScalarTaylorPolynomial, const SamplingFunctional sampling_strategy = SamplingFunctional::PointSample) const;

    /*! \brief Fills the _P matrix with P*sqrt(w) for use in solving for curvature

         Uses _curvature_poly_order as the polynomial order of the basis

        \param teamMember           [in] - Kokkos::TeamPolicy member type (created by parallel_for)
        \param delta            [in/out] - scratch space that is allocated so that each thread has its own copy. Must be at least as large is the _basis_multipler*the dimension of the polynomial basis.
        \param P                   [out] - 2D Kokkos View which will contain evaluation of sampling functional on polynomial basis for each neighbor the target has (stored column major)
        \param w                   [out] - 1D Kokkos View which will contain weighting kernel values for the target with each neighbor if weight_p = true
        \param dimension            [in] - spatial dimension of basis to evaluate. e.g. dimension two basis of order one is 1, x, y, whereas for dimension 3 it is 1, x, y, z
        \param only_specific_order  [in] - boolean for only evaluating one degree of polynomial when true
        \param V                    [in] - orthonormal basis matrix size _dimensions * _dimensions whose first _dimensions-1 columns are an approximation of the tangent plane
    */
    KOKKOS_INLINE_FUNCTION
    void createWeightsAndPForCurvature(const member_type& teamMember, scratch_vector_type delta, scratch_matrix_type P, scratch_vector_type w, const int dimension, bool only_specific_order, scratch_matrix_type* V = NULL) const;

    /*! \brief Evaluates a polynomial basis with a target functional applied to each member of the basis
        \param teamMember                   [in] - Kokkos::TeamPolicy member type (created by parallel_for)
        \param t1                       [in/out] - scratch space that is allocated so that each team has its own copy. Must be at least as large is the _basis_multipler*the dimension of the polynomial basis.
        \param t2                       [in/out] - scratch space that is allocated so that each team has its own copy. Must be at least as large is the _basis_multipler*the dimension of the polynomial basis.
        \param P_target_row                [out] - 1D Kokkos View where the evaluation of the polynomial basis is stored
    */
    KOKKOS_INLINE_FUNCTION
    void computeTargetFunctionals(const member_type& teamMember, scratch_vector_type t1, scratch_vector_type t2, scratch_vector_type P_target_row) const;

    /*! \brief Evaluates a polynomial basis for the curvature with a gradient target functional applied

        _operations is used by this function which is set through a modifier function

        \param teamMember                   [in] - Kokkos::TeamPolicy member type (created by parallel_for)
        \param t1                       [in/out] - scratch space that is allocated so that each team has its own copy. Must be at least as large is the _basis_multipler*the dimension of the polynomial basis.
        \param t2                       [in/out] - scratch space that is allocated so that each team has its own copy. Must be at least as large is the _basis_multipler*the dimension of the polynomial basis.
        \param P_target_row                [out] - 1D Kokkos View where the evaluation of the polynomial basis is stored
        \param V                            [in] - orthonormal basis matrix size _dimensions * _dimensions whose first _dimensions-1 columns are an approximation of the tangent plane
    */
    KOKKOS_INLINE_FUNCTION
    void computeCurvatureFunctionals(const member_type& teamMember, scratch_vector_type t1, scratch_vector_type t2, scratch_vector_type P_target_row, scratch_matrix_type* V) const;

    /*! \brief Evaluates a polynomial basis with a target functional applied, using information from the manifold curvature

         _operations is used by this function which is set through a modifier function

        \param teamMember                   [in] - Kokkos::TeamPolicy member type (created by parallel_for)
        \param t1                       [in/out] - scratch space that is allocated so that each team has its own copy. Must be at least as large is the _basis_multipler*the dimension of the polynomial basis.
        \param t2                       [in/out] - scratch space that is allocated so that each team has its own copy. Must be at least as large is the _basis_multipler*the dimension of the polynomial basis.
        \param P_target_row                [out] - 1D Kokkos View where the evaluation of the polynomial basis is stored
        \param V                            [in] - orthonormal basis matrix size _dimensions * _dimensions whose first _dimensions-1 columns are an approximation of the tangent plane
        \param G_inv                        [in] - (_dimensions-1)*(_dimensions-1) Kokkos View containing inverse of metric tensor
        \param curvature_coefficients       [in] - polynomial coefficients for curvature
        \param curvature_gradients          [in] - approximation of gradient of curvature, Kokkos View of size (_dimensions-1)
    */
    KOKKOS_INLINE_FUNCTION
    void computeTargetFunctionalsOnManifold(const member_type& teamMember, scratch_vector_type t1, scratch_vector_type t2, scratch_vector_type P_target_row, scratch_matrix_type V, scratch_matrix_type G_inv, scratch_vector_type curvature_coefficients, scratch_vector_type curvature_gradients) const;

    //! Helper function for applying the evaluations from a target functional to the polynomial coefficients
    KOKKOS_INLINE_FUNCTION
    void applyTargetsToCoefficients(const member_type& teamMember, scratch_vector_type t1, scratch_vector_type t2, scratch_matrix_type Q, scratch_matrix_type R, scratch_vector_type w, scratch_vector_type P_target_row, const int target_NP) const;

    //! Generates quadrature for staggered approach
    void generate1DQuadrature();

///@}



/** @name Private Accessors
 *  Private function because information lives on the device
 */
///@{

    //! Returns number of neighbors for a particular target
    KOKKOS_INLINE_FUNCTION
    int getNNeighbors(const int target_index) const {
        return _neighbor_lists(target_index, 0);
    }

    //! Mapping from [0,number of neighbors for a target] to the row that contains the source coordinates for
    //! that neighbor
    KOKKOS_INLINE_FUNCTION
    int getNeighborIndex(const int target_index, const int neighbor_list_num) const {
        return _neighbor_lists(target_index, neighbor_list_num+1);
    }

    //! (OPTIONAL)
    //! Returns number of additional evaluation sites for a particular target
    KOKKOS_INLINE_FUNCTION
    int getNAdditionalEvaluationCoordinates(const int target_index) const {
        return _additional_evaluation_indices(target_index,0);
    }

    //! (OPTIONAL)
    //! Mapping from [0,number of additional evaluation sites for a target] to the row that contains the coordinates for
    //! that evaluation
    KOKKOS_INLINE_FUNCTION
    int getAdditionalEvaluationIndex(const int target_index, const int additional_list_num) const {
        return _additional_evaluation_indices(target_index, additional_list_num+1);
    }

    //! Returns Euclidean norm of a vector
    KOKKOS_INLINE_FUNCTION
    double EuclideanVectorLength(const XYZ& delta_vector, const int dimension) const {

        double inside_val = delta_vector.x*delta_vector.x;
        switch (dimension) {
        case 3:
            inside_val += delta_vector.z*delta_vector.z;
            // no break is intentional
        case 2:
            inside_val += delta_vector.y*delta_vector.y;
            // no break is intentional
        default:
            break;
        }
        return std::sqrt(inside_val);

    }

    //! Returns one component of the target coordinate for a particular target. Whether global or local coordinates 
    //! depends upon V being specified
    KOKKOS_INLINE_FUNCTION
    double getTargetCoordinate(const int target_index, const int dim, const scratch_matrix_type* V = NULL) const {
        compadre_kernel_assert_debug((_target_coordinates.extent(0) >= target_index) && "Target index is out of range for _target_coordinates.");
        if (V==NULL) {
            return _target_coordinates(target_index, dim);
        } else {
            XYZ target_coord = XYZ( _target_coordinates(target_index, 0), 
                                    _target_coordinates(target_index, 1), 
                                    _target_coordinates(target_index, 2));
            return this->convertGlobalToLocalCoordinate(target_coord, dim, V);
        }
    }

    //! (OPTIONAL)
    //! Returns one component of the additional evaluation coordinates. Whether global or local coordinates 
    //! depends upon V being specified
    KOKKOS_INLINE_FUNCTION
    double getTargetAuxiliaryCoordinate(const int additional_evaluation_index, const int dim, const scratch_matrix_type* V = NULL) const {
        compadre_kernel_assert_debug((_additional_evaluation_coordinates.extent(0) >= additional_evaluation_index) && "Additional evaluation index is out of range for _additional_evaluation_coordinates.");
        if (V==NULL) {
            return _additional_evaluation_coordinates(additional_evaluation_index, dim);
        } else {
            XYZ additional_target_coord = XYZ( _additional_evaluation_coordinates(additional_evaluation_index, 0),
                                               _additional_evaluation_coordinates(additional_evaluation_index, 1),
                                               _additional_evaluation_coordinates(additional_evaluation_index, 2));
            return this->convertGlobalToLocalCoordinate(additional_target_coord, dim, V);
        }
    }

    //! Returns one component of the neighbor coordinate for a particular target. Whether global or local coordinates 
    //! depends upon V being specified
    KOKKOS_INLINE_FUNCTION
    double getNeighborCoordinate(const int target_index, const int neighbor_list_num, const int dim, const scratch_matrix_type* V = NULL) const {
        compadre_kernel_assert_debug((_source_coordinates.extent(0) >= this->getNeighborIndex(target_index, neighbor_list_num)) && "Source index is out of range for _source_coordinates.");
        if (V==NULL) {
            return _source_coordinates(this->getNeighborIndex(target_index, neighbor_list_num), dim);
        } else {
            XYZ neighbor_coord = XYZ(_source_coordinates(this->getNeighborIndex(target_index, neighbor_list_num), 0), _source_coordinates(this->getNeighborIndex(target_index, neighbor_list_num), 1), _source_coordinates(this->getNeighborIndex(target_index, neighbor_list_num), 2));
            return this->convertGlobalToLocalCoordinate(neighbor_coord, dim, V);
        }
    }

    //! Returns the relative coordinate as a vector between the target site and the neighbor site. 
    //! Whether global or local coordinates depends upon V being specified
    KOKKOS_INLINE_FUNCTION
    XYZ getRelativeCoord(const int target_index, const int neighbor_list_num, const int dimension, const scratch_matrix_type* V = NULL) const {
        XYZ coordinate_delta;

        coordinate_delta.x = this->getNeighborCoordinate(target_index, neighbor_list_num, 0, V) - this->getTargetCoordinate(target_index, 0, V);
        if (dimension>1) coordinate_delta.y = this->getNeighborCoordinate(target_index, neighbor_list_num, 1, V) - this->getTargetCoordinate(target_index, 1, V);
        if (dimension>2) coordinate_delta.z = this->getNeighborCoordinate(target_index, neighbor_list_num, 2, V) - this->getTargetCoordinate(target_index, 2, V);

        return coordinate_delta;
    }

    //! Returns a component of the local coordinate after transformation from global to local under the orthonormal basis V.
    KOKKOS_INLINE_FUNCTION
    double convertGlobalToLocalCoordinate(const XYZ global_coord, const int dim, const scratch_matrix_type* V) const {
        // only written for 2d manifold in 3d space
        double val = 0;
        val += global_coord.x * (*V)(dim, 0);
        val += global_coord.y * (*V)(dim, 1); // can't be called from dimension 1 problem
        if (_dimensions>2) val += global_coord.z * (*V)(dim, 2);
        return val;
    }

    //! Returns a component of the global coordinate after transformation from local to global under the orthonormal basis V^T.
    KOKKOS_INLINE_FUNCTION
    double convertLocalToGlobalCoordinate(const XYZ local_coord, const int dim, const scratch_matrix_type* V) const {
        // only written for 2d manifold in 3d space
        double val;
        if (dim == 0 && _dimensions==2) { // 2D problem with 1D manifold
            val = local_coord.x * (*V)(0, dim);
        } else if (dim == 0) { // 3D problem with 2D manifold
            val = local_coord.x * ((*V)(0, dim) + (*V)(1, dim));
        } else if (dim == 1) { // 3D problem with 2D manifold
            val = local_coord.y * ((*V)(0, dim) + (*V)(1, dim));
        }
        return val;
    }

    //! Get offset depending on 
    KOKKOS_INLINE_FUNCTION
    int getPTargetOffsetIndex(const int lro_num, const int input_component, const int output_component, const int additional_evaluation_local_index = -1) const {
        // the +1 is so we get a multiplier of 0 if additional_evaluation_local_index is not specified
        return (_total_alpha_values*(additional_evaluation_local_index+1) + _lro_total_offsets[lro_num] + input_component*_lro_output_tile_size[lro_num]+output_component)*_basis_multiplier;
    }

///@}

public:

/** @name Instantiation / Destruction
 *  
 */
///@{

    //! Minimal constructor providing no data (neighbor lists, source sites, target sites) 
    GMLS(const int poly_order,
            const std::string dense_solver_type = std::string("QR"),
            const int manifold_curvature_poly_order = 2,
            const int dimensions = 3) : _poly_order(poly_order), _curvature_poly_order(manifold_curvature_poly_order), _dimensions(dimensions) {

        // seed random number generator pool
        _random_number_pool = pool_type(1);

        _NP = this->getNP(_poly_order, dimensions);
        Kokkos::fence();

#ifdef COMPADRE_USE_CUDA
        _scratch_team_level_a = 0;
        _scratch_thread_level_a = 1;
        _scratch_team_level_b = 1;
        _scratch_thread_level_b = 1;
#else
        _scratch_team_level_a = 0;
        _scratch_thread_level_a = 0;
        _scratch_team_level_b = 0;
        _scratch_thread_level_b = 0;
#endif
        _threads_per_team = 0;

        // temporary, just to avoid warning
        _dense_solver_type = DenseSolverType::QR;
        // set based on input
        this->setSolverType(dense_solver_type);

        _lro_lookup = std::vector<int>(TargetOperation::COUNT,-1); // hard coded against number of operations defined
        _lro = std::vector<TargetOperation>();

        // various initializations
        _type = 1;
        _total_alpha_values = 0;

        _weighting_type = WeightingFunctionType::Power;
        _curvature_weighting_type = WeightingFunctionType::Power;
        _weighting_power = 2;
        _curvature_weighting_power = 2;

        _reconstruction_space = ReconstructionSpace::VectorOfScalarClonesTaylorPolynomial;
        _polynomial_sampling_functional = SamplingFunctional::VectorPointSample;
        _data_sampling_functional = SamplingFunctional::VectorPointSample;
        _reconstruction_space_rank = ActualReconstructionSpaceRank[_reconstruction_space];

        _basis_multiplier = 1;
        _sampling_multiplier = 1;
        _number_of_quadrature_points = 2;

        _nontrivial_nullspace = false;
        _orthonormal_tangent_space_provided = false; 

        _global_dimensions = dimensions;
        if (_dense_solver_type == DenseSolverType::MANIFOLD) {
            _local_dimensions = dimensions-1;
            // VectorPointSample is dealt with differently on a manifold since it includes a coordinate
            // transform to a manifold's local chart
            if (_polynomial_sampling_functional == SamplingFunctional::VectorPointSample)
                _polynomial_sampling_functional = SamplingFunctional::ManifoldVectorPointSample;
            if (_data_sampling_functional == SamplingFunctional::VectorPointSample)
                _data_sampling_functional = SamplingFunctional::ManifoldVectorPointSample;
        } else {
            _local_dimensions = dimensions;
        }
    }

    //! Constructor for the case when the data sampling functional does not match the polynomial
    //! sampling functional. Only case anticipated is staggered Laplacian.
    GMLS(ReconstructionSpace reconstruction_space,
        SamplingFunctional polynomial_sampling_strategy,
        SamplingFunctional data_sampling_strategy,
        const int poly_order,
        const std::string dense_solver_type = std::string("QR"),
        const int manifold_curvature_poly_order = 2,
        const int dimensions = 3)
            : GMLS(poly_order, dense_solver_type, manifold_curvature_poly_order, dimensions) {

        _reconstruction_space = reconstruction_space;
        _polynomial_sampling_functional = polynomial_sampling_strategy;
        _data_sampling_functional = data_sampling_strategy;
        _reconstruction_space_rank = ActualReconstructionSpaceRank[_reconstruction_space];
        if (_dense_solver_type == DenseSolverType::MANIFOLD) {
            // VectorPointSample is dealt with differently on a manifold since it includes a coordinate
            // transform to a manifold's local chart
            if (_polynomial_sampling_functional == SamplingFunctional::VectorPointSample)
                _polynomial_sampling_functional = SamplingFunctional::ManifoldVectorPointSample;
            if (_data_sampling_functional == SamplingFunctional::VectorPointSample)
                _data_sampling_functional = SamplingFunctional::ManifoldVectorPointSample;
        }
        compadre_assert_release((SamplingOutputTensorRank[(int)_polynomial_sampling_functional] 
                    == SamplingOutputTensorRank[(int)_polynomial_sampling_functional]) 
                && "Output rank of polynomial and data sampling functionals must match.");
    };

    //! Constructor for the case when nonstandard sampling functionals or reconstruction spaces
    //! are to be used. Reconstruction space and sampling strategy can only be set at instantiation.
    GMLS(ReconstructionSpace reconstruction_space,
            SamplingFunctional dual_sampling_strategy,
            const int poly_order,
            const std::string dense_solver_type = std::string("QR"),
            const int manifold_curvature_poly_order = 2,
            const int dimensions = 3)
                : GMLS(reconstruction_space, dual_sampling_strategy, dual_sampling_strategy, poly_order, dense_solver_type, manifold_curvature_poly_order, dimensions) {}

    //! Destructor
    ~GMLS(){
    };

///@}

/** @name Functors
 *  Member functions that perform operations on the entire batch
 */
///@{

    //! Tag for functor to assemble the P*sqrt(weights) matrix and construct sqrt(weights)*Identity
    struct AssembleStandardPsqrtW{};

    //! Tag for functor to evaluate targets, apply target evaluation to polynomial coefficients to
    //! store in _alphas
    struct ApplyStandardTargets{};

    //! Tag for functor to create a coarse tangent approximation from a given neighborhood of points
    struct ComputeCoarseTangentPlane{};

    //! Tag for functor to assemble the P*sqrt(weights) matrix and construct sqrt(weights)*Identity for curvature
    struct AssembleCurvaturePsqrtW{};

    //! Tag for functor to evaluate curvature targets and construct accurate tangent direction approximation for manifolds
    struct GetAccurateTangentDirections{};

    //! Tag for functor to evaluate curvature targets and apply to coefficients of curvature reconstruction
    struct ApplyCurvatureTargets{};

    //! Tag for functor to assemble the P*sqrt(weights) matrix and construct sqrt(weights)*Identity
    struct AssembleManifoldPsqrtW{};

    //! Tag for functor to evaluate targets, apply target evaluation to polynomial coefficients to store in _alphas
    struct ApplyManifoldTargets{};

    //! Tag for functor to calculate prestencil weights to apply to data to transform into a format expected by a GMLS stencil
    struct ComputePrestencilWeights{};



    //! Functor to assemble the P*sqrt(weights) matrix and construct sqrt(weights)*Identity
    KOKKOS_INLINE_FUNCTION
    void operator() (const AssembleStandardPsqrtW&, const member_type& teamMember) const;

    //! Functor to evaluate targets, apply target evaluation to polynomial coefficients to store in _alphas
    KOKKOS_INLINE_FUNCTION
    void operator() (const ApplyStandardTargets&, const member_type& teamMember) const;

    //! Functor to create a coarse tangent approximation from a given neighborhood of points
    KOKKOS_INLINE_FUNCTION
    void operator() (const ComputeCoarseTangentPlane&, const member_type& teamMember) const;

    //! Functor to assemble the P*sqrt(weights) matrix and construct sqrt(weights)*Identity for curvature
    KOKKOS_INLINE_FUNCTION
    void operator() (const AssembleCurvaturePsqrtW&, const member_type& teamMember) const;

    //! Functor to evaluate curvature targets and construct accurate tangent direction approximation for manifolds
    KOKKOS_INLINE_FUNCTION
    void operator() (const GetAccurateTangentDirections&, const member_type& teamMember) const;

    //! Functor to evaluate curvature targets and apply to coefficients of curvature reconstruction
    KOKKOS_INLINE_FUNCTION
    void operator() (const ApplyCurvatureTargets&, const member_type& teamMember) const;

    //! Functor to assemble the P*sqrt(weights) matrix and construct sqrt(weights)*Identity
    KOKKOS_INLINE_FUNCTION
    void operator() (const AssembleManifoldPsqrtW&, const member_type& teamMember) const;

    //! Functor to evaluate targets, apply target evaluation to polynomial coefficients to store in _alphas
    KOKKOS_INLINE_FUNCTION
    void operator() (const ApplyManifoldTargets&, const member_type& teamMember) const;

    //! Functor to calculate prestencil weights to apply to data to transform into a format expected by a GMLS stencil
    KOKKOS_INLINE_FUNCTION
    void operator() (const ComputePrestencilWeights&, const member_type& teamMember) const;

///@}


    //! Returns size of the basis for a given polynomial order and dimension
    KOKKOS_INLINE_FUNCTION
    static int getNP(const int m, const int dimension = 3) {
        if (dimension == 3) return (m+1)*(m+2)*(m+3)/6;
        else if (dimension == 2) return (m+1)*(m+2)/2;
        else return m+1;
    }

    //! Returns number of neighbors needed for unisolvency for a given basis order and dimension
    KOKKOS_INLINE_FUNCTION
    static int getNN(const int m, const int dimension = 3) {
        const int np = getNP(m, dimension);
        int nn = np;
        switch (dimension) {
            case 3:
                nn = np * (1.7 + m*0.1);
                break;
            case 2:
                nn = np * (1.4 + m*0.03);
                break;
            case 1:
                nn = np * 1.1;
        }
        return nn;
    }

/** @name Accessors
 *  Retrieve member variables through public member functions
 */
///@{

    //! Dimension of the GMLS problem, set only at class instantiation
    int getDimensions() const { return _dimensions; }

    //! Dimension of the GMLS problem's point data (spatial description of points in ambient space), set only at class instantiation
    int getGlobalDimensions() const { return _global_dimensions; }

    //! Local dimension of the GMLS problem (less than global dimension if on a manifold), set only at class instantiation
    int getLocalDimensions() const { return _local_dimensions; }

    //! Get type of problem for GMLS
    DenseSolverType getProblemType() { return _dense_solver_type; }

    //! Type for weighting kernel for GMLS problem
    WeightingFunctionType getWeightingType() const { return _weighting_type; }

    //! Type for weighting kernel for curvature 
    WeightingFunctionType getManifoldWeightingType() const { return _curvature_weighting_type; }

    //! Power for weighting kernel for GMLS problem
    int getWeightingPower() const { return _weighting_power; }

    //! Power for weighting kernel for curvature
    int getManifoldWeightingPower() const { return _curvature_weighting_power; }

    //! Number of 1D quadrature points to use for staggered approach
    int getNumberOfQuadraturePoints() const { return _number_of_quadrature_points; }

    //! Get a view (host) of the length of each neighbor list. 
    //! Each entry corresponds to a row of _neighbor_lists.
    decltype(_number_of_neighbors_list) getNeighborListsLengths() const { 
        return _number_of_neighbors_list; 
    }

    //! Get a view (device) of all neighbor lists. First column is the number of neighbors for that row's list.
    decltype(_neighbor_lists) getNeighborLists() const { return _neighbor_lists; }

    //! Get a view (device) of all tangent direction bundles.
    decltype(_T) getTangentDirections() const { return _T; }

    //! Get component of tangent or normal directions for manifold problems
    double getTangentBundle(const int target_index, const int direction, const int component) {
        // Component index 0.._dimensions-2 will return tangent direction
        // Component index _dimensions-1 will return the normal direction
        Kokkos::View<double**, layout_type, Kokkos::MemoryTraits<Kokkos::Unmanaged> >::HostMirror T(_host_T.data() + target_index*_dimensions*_dimensions, _dimensions, _dimensions);
        return T(direction, component);
    }

    //! Get a view (device) of all rank 2 preprocessing tensors
    //! This is a rank 5 tensor that is able to provide data transformation
    //! into a form that GMLS is able to operate on. The ranks are as follows:
    //!
    //! 1 - Either size 2 if it operates on the target site and neighbor site (staggered schemes)
    //!     or 1 if it operates only on the neighbor sites (almost every scheme)
    //!
    //! 2 - If the data transform varies with each target site (but could be the same for each neighbor of that target site), then this is the number of target sites
    //!
    //! 3 - If the data transform varies with each neighbor of each target site, then this is the number of neighbors for each respective target (max number of neighbors for all target sites is its uniform size)
    //!
    //! 4 - Data transform resulting in rank 1 data for the GMLS operator will have size _local_dimensions, otherwise 1
    //!
    //! 5 - Data transform taking in rank 1 data will have size _global_dimensions, otherwise 1
    decltype(_prestencil_weights) getPrestencilWeights() const { 
        return _prestencil_weights;
    }

    //! Retrieves the offset for an operator based on input and output component, generic to row
    //! (but still multiplied by the number of neighbors for each row and then needs a neighbor number added 
    //! to this returned value to be meaningful)
    int getAlphaColumnOffset(TargetOperation lro, const int output_component_axis_1, 
            const int output_component_axis_2, const int input_component_axis_1, const int input_component_axis_2) const {

        const int lro_number = _lro_lookup[(int)lro];
        compadre_assert_debug((lro_number >= 0) && "getAlphasColumnOffset called for a TargetOperation that was not registered.");

        // the target functional input indexing is sized based on the output rank of the sampling
        // functional used, which can not be inferred unless a specification of target functional,
        // reconstruction space, and sampling functional are all known (as was the case at the
        // construction of this class)
        const int input_index = getSamplingOutputIndex((int)_polynomial_sampling_functional, input_component_axis_1, input_component_axis_2);
        const int output_index = getTargetOutputIndex((int)lro, output_component_axis_1, output_component_axis_2);

        return  _host_lro_total_offsets[lro_number] + input_index*_host_lro_output_tile_size[lro_number] + output_index;
    }

    //! Get a view (device) of all alphas
    decltype(_alphas) getAlphas() const { return _alphas; }

    //! Helper function for getting alphas for scalar reconstruction from scalar data
    double getAlpha0TensorTo0Tensor(TargetOperation lro, const int target_index, const int neighbor_index) const {
        // e.g. Dirac Delta target of a scalar field
        return getAlpha(lro, target_index, 0, 0, neighbor_index, 0, 0);
    }

    //! Helper function for getting alphas for vector reconstruction from scalar data
    double getAlpha0TensorTo1Tensor(TargetOperation lro, const int target_index, const int output_component, const int neighbor_index) const {
        // e.g. gradient of a scalar field
        return getAlpha(lro, target_index, output_component, 0, neighbor_index, 0, 0);
    }

    //! Helper function for getting alphas for matrix reconstruction from scalar data
    double getAlpha0TensorTo2Tensor(TargetOperation lro, const int target_index, const int output_component_axis_1, const int output_component_axis_2, const int neighbor_index) const {
        return getAlpha(lro, target_index, output_component_axis_1, output_component_axis_2, neighbor_index, 0, 0);
    }

    //! Helper function for getting alphas for scalar reconstruction from vector data
    double getAlpha1TensorTo0Tensor(TargetOperation lro, const int target_index, const int neighbor_index, const int input_component) const {
        // e.g. divergence of a vector field
        return getAlpha(lro, target_index, 0, 0, neighbor_index, input_component, 0);
    }

    //! Helper function for getting alphas for vector reconstruction from vector data
    double getAlpha1TensorTo1Tensor(TargetOperation lro, const int target_index, const int output_component, const int neighbor_index, const int input_component) const {
        // e.g. curl of a vector field
        return getAlpha(lro, target_index, output_component, 0, neighbor_index, input_component, 0);
    }

    //! Helper function for getting alphas for matrix reconstruction from vector data
    double getAlpha1TensorTo2Tensor(TargetOperation lro, const int target_index, const int output_component_axis_1, const int output_component_axis_2, const int neighbor_index, const int input_component) const {
        // e.g. gradient of a vector field
        return getAlpha(lro, target_index, output_component_axis_1, output_component_axis_2, neighbor_index, input_component, 0);
    }

    //! Helper function for getting alphas for scalar reconstruction from matrix data
    double getAlpha2TensorTo0Tensor(TargetOperation lro, const int target_index, const int neighbor_index, const int input_component_axis_1, const int input_component_axis_2) const {
        return getAlpha(lro, target_index, 0, 0, neighbor_index, input_component_axis_1, input_component_axis_2);
    }

    //! Helper function for getting alphas for vector reconstruction from matrix data
    double getAlpha2TensorTo1Tensor(TargetOperation lro, const int target_index, const int output_component, const int neighbor_index, const int input_component_axis_1, const int input_component_axis_2) const {
        return getAlpha(lro, target_index, output_component, 0, neighbor_index, input_component_axis_1, input_component_axis_2);
    }

    //! Helper function for getting alphas for matrix reconstruction from matrix data
    double getAlpha2TensorTo2Tensor(TargetOperation lro, const int target_index, const int output_component_axis_1, const int output_component_axis_2, const int neighbor_index, const int input_component_axis_1, const int input_component_axis_2) const {
        return getAlpha(lro, target_index, output_component_axis_1, output_component_axis_2, neighbor_index, input_component_axis_1, input_component_axis_2);
    }

    //! Underlying function all interface helper functions call to retrieve alpha values
    double getAlpha(TargetOperation lro, const int target_index, const int output_component_axis_1, const int output_component_axis_2, const int neighbor_index, const int input_component_axis_1, const int input_component_axis_2) const {
        // lro - the operator from TargetOperations
        // target_index - the # for the target site where information is required
        // neighbor_index - the # for the neighbor of the target
        //
        // This code support up to rank 2 tensors for inputs and outputs
        //
        // scalar reconstruction from scalar data: rank 0 to rank 0
        //   provides 1 piece of information for each neighbor
        // scalar reconstruction from vector data (e.g. divergence): rank 1 to rank 0
        //   provides 'd' pieces of information for each neighbor
        // vector reconstruction from scalar data (e.g. gradient): rank 0 to rank 1
        //   provides 'd' piece of information for each neighbor
        // vector reconstruction from vector data (e.g. curl): rank 1 to rank 1
        //   provides 'd'x'd' pieces of information for each neighbor
        //
        // This function would more reasonably be called from one of the getAlphaNTensorFromNTensor
        // which is much easier to understand with respect to indexing and only requesting indices
        // that are relavent to the operator in question.
        //

        const int alpha_column_offset = this->getAlphaColumnOffset( lro, output_component_axis_1, 
                output_component_axis_2, input_component_axis_1, input_component_axis_2);

        return _host_alphas(ORDER_INDICES(target_index, alpha_column_offset*_number_of_neighbors_list(target_index) + neighbor_index));
    }

    //! Returns a stencil to transform data from its existing state into the input expected 
    //! for some sampling functionals.
    double getPreStencilWeight(SamplingFunctional sro, const int target_index, const int neighbor_index, bool for_target, const int output_component = 0, const int input_component = 0) const {
        // for certain sampling strategies, linear combinations of the neighbor and target value are needed
        // for the traditional PointSample, this value is 1 for the neighbor and 0 for the target
        if (sro == SamplingFunctional::PointSample ) {
            if (for_target) return 0; else return 1;
        }

        // these check conditions on the sampling operator and change indexing on target and neighbors
        // in order to reuse information, such as if the same data transformation is used, regardless
        // of target site or neighbor site
        const int target_index_in_weights = 
            (SamplingTensorStyle[(int)sro]==DifferentEachTarget 
                    || SamplingTensorStyle[(int)sro]==DifferentEachNeighbor) ?
                target_index : 0;
        const int neighbor_index_in_weights = 
            (SamplingTensorStyle[(int)sro]==DifferentEachNeighbor) ?
                neighbor_index : 0;

        return _host_prestencil_weights((int)for_target, target_index_in_weights, neighbor_index_in_weights, 
                    output_component, input_component);
    }

    //! Dimensions ^ output rank for target operation
    int getOutputDimensionOfOperation(TargetOperation lro, bool ambient = false) const {
        int return_val;
        if (ambient) return_val = std::pow(_global_dimensions, TargetOutputTensorRank[(int)lro]);
        else return_val = std::pow(_local_dimensions, TargetOutputTensorRank[(int)lro]);
        return return_val;
    }

    //! Dimensions ^ input rank for target operation (always in local chart if on a manifold, never ambient space)
    int getInputDimensionOfOperation(TargetOperation lro) const {
        return this->_host_lro_input_tile_size[_lro_lookup[(int)lro]];
        // this is the same return values as the OutputDimensionOfSampling for the GMLS class's SamplingFunctional
    }

    //! Dimensions ^ output rank for sampling operation 
    //! (always in local chart if on a manifold, never ambient space)
    int getOutputDimensionOfSampling(SamplingFunctional sro) const {
        return std::pow(_local_dimensions, SamplingOutputTensorRank[(int)sro]);
    }

    //! Dimensions ^ output rank for sampling operation 
    //! (always in ambient space, never local chart on a manifold)
    int getInputDimensionOfSampling(SamplingFunctional sro) const {
        return std::pow(_global_dimensions, SamplingInputTensorRank[(int)sro]);
    }

    //! Output rank for sampling operation
    int getOutputRankOfSampling(SamplingFunctional sro) const {
        return SamplingOutputTensorRank[(int)sro];
    }

    //! Input rank for sampling operation
    int getInputRankOfSampling(SamplingFunctional sro) const {
        return SamplingInputTensorRank[(int)sro];
    }

///@}


/** @name Modifiers
 *  Changed member variables through public member functions
 */
///@{

    //! Sets basic problem data (neighbor lists, source coordinates, and target coordinates)
    template<typename view_type_1, typename view_type_2, typename view_type_3, typename view_type_4>
    void setProblemData(
            view_type_1 neighbor_lists,
            view_type_2 source_coordinates,
            view_type_3 target_coordinates,
            view_type_4 epsilons) {
        this->setNeighborLists<view_type_1>(neighbor_lists);
        this->setSourceSites<view_type_2>(source_coordinates);
        this->setTargetSites<view_type_3>(target_coordinates);
        this->setWindowSizes<view_type_4>(epsilons);
    }

    //! Sets neighbor list information. Should be # targets x maximum number of neighbors for any target + 1.
    //! first entry in ever row should be the number of neighbors for the corresponding target.
    template <typename view_type>
    void setNeighborLists(view_type neighbor_lists) {
        // Catches Kokkos::View<int**, Kokkos::DefaultHostExecutionSpace
        // allocate memory on device
        _neighbor_lists = Kokkos::View<int**, layout_type>("device neighbor lists",
            neighbor_lists.dimension_0(), neighbor_lists.dimension_1());

        _host_neighbor_lists = Kokkos::create_mirror_view(_neighbor_lists);
        Kokkos::deep_copy(_host_neighbor_lists, neighbor_lists);
        // copy data from host to device
        Kokkos::deep_copy(_neighbor_lists, _host_neighbor_lists);

        _number_of_neighbors_list = Kokkos::View<int*, Kokkos::HostSpace>("number of neighbors", neighbor_lists.dimension_0());
        for (int i=0; i<_neighbor_lists.dimension_0(); ++i) {
            _number_of_neighbors_list(i) = neighbor_lists(i,0);
        }
    }

    //! Sets neighbor list information. 2D array should be # targets x maximum number of neighbors for any target + 1.
    //! first entry in ever row should be the number of neighbors for the corresponding target.
    template <typename view_type>
    void setNeighborLists(Kokkos::View<int**, Kokkos::DefaultExecutionSpace> neighbor_lists) {
        // allocate memory on device
        _neighbor_lists = neighbor_lists;

        _host_neighbor_lists = Kokkos::create_mirror_view(_neighbor_lists);
        // copy data from host to device
        Kokkos::deep_copy(_host_neighbor_lists, _neighbor_lists);

        _number_of_neighbors_list = Kokkos::View<int*, Kokkos::HostSpace>("number of neighbors", neighbor_lists.dimension_0());
        for (int i=0; i<_neighbor_lists.dimension_0(); ++i) {
            _number_of_neighbors_list(i) = _host_neighbor_lists(i,0);
        }
    }

    //! Sets source coordinate information. Rows of this 2D-array should correspond to neighbor IDs contained in the entries
    //! of the neighbor lists 2D array.
    template<typename view_type>
    void setSourceSites(view_type source_coordinates) {

        // allocate memory on device
        _source_coordinates = Kokkos::View<double**, layout_type>("device neighbor coordinates",
                source_coordinates.dimension_0(), source_coordinates.dimension_1());

        _host_source_coordinates = Kokkos::create_mirror_view(_source_coordinates);
        Kokkos::deep_copy(_host_source_coordinates, source_coordinates);
        // copy data from host to device
        Kokkos::deep_copy(_source_coordinates, _host_source_coordinates);

    }

    //! Sets source coordinate information. Rows of this 2D-array should correspond to neighbor IDs contained in the entries
    //! of the neighbor lists 2D array.
    template<typename view_type>
    void setSourceSites(Kokkos::View<double**, Kokkos::DefaultExecutionSpace> source_coordinates) {
        // allocate memory on device
        _source_coordinates = source_coordinates;
    }

    //! Sets target coordinate information. Rows of this 2D-array should correspond to rows of the neighbor lists.
    template<typename view_type>
    void setTargetSites(view_type target_coordinates) {
        // allocate memory on device
        _target_coordinates = Kokkos::View<double**, layout_type>("device target coordinates",
                target_coordinates.dimension_0(), target_coordinates.dimension_1());

        _host_target_coordinates = Kokkos::create_mirror_view(_target_coordinates);
        Kokkos::deep_copy(_host_target_coordinates, target_coordinates);
        // copy data from host to device
        Kokkos::deep_copy(_target_coordinates, _host_target_coordinates);
    }

    //! Sets target coordinate information. Rows of this 2D-array should correspond to rows of the neighbor lists.
    template<typename view_type>
    void setTargetSites(Kokkos::View<double**, Kokkos::DefaultExecutionSpace> target_coordinates) {
        // allocate memory on device
        _target_coordinates = target_coordinates;
    }

    //! Sets window sizes, also called the support of the kernel
    template<typename view_type>
    void setWindowSizes(view_type epsilons) {

        // allocate memory on device
        _epsilons = Kokkos::View<double*>("device epsilons",
                        epsilons.dimension_0(), epsilons.dimension_1());

        _host_epsilons = Kokkos::create_mirror_view(_epsilons);
        Kokkos::deep_copy(_host_epsilons, epsilons);
        // copy data from host to device
        Kokkos::deep_copy(_epsilons, _host_epsilons);
    }

    //! Sets window sizes, also called the support of the kernel (device)
    template<typename view_type>
    void setWindowSizes(Kokkos::View<double*, Kokkos::DefaultExecutionSpace> epsilons) {
        // allocate memory on device
        _epsilons = epsilons;
    }

    //! (OPTIONAL)
    //! Sets orthonormal tangent directions for reconstruction on a manifold. The first rank of this 2D array 
    //! corresponds to the target indices, i.e., rows of the neighbor lists 2D array. The second rank is the 
    //! ordinal of the tangent direction (spatial dimensions-1 are tangent, last one is normal), and the third 
    //! rank is indices into the spatial dimension.
    template<typename view_type>
    void setTangentBundle(view_type tangent_directions) {
        // accept input from user as a rank 3 tensor
        // but convert data to a rank 2 tensor with the last rank of dimension = _dimensions x _dimensions
        // this allows for nonstrided views on the device later
        
        // add assert for manifold
        // (_dense_solver_type == DenseSolverType::MANIFOLD) {

        // allocate memory on device
        _T = Kokkos::View<double*>("device tangent directions", _target_coordinates.dimension_0()*_dimensions*_dimensions);

        // rearrange data on device from data given on host
        Kokkos::parallel_for("copy tangent vectors", Kokkos::RangePolicy<Kokkos::DefaultExecutionSpace>(0, _target_coordinates.dimension_0()), KOKKOS_LAMBDA(const int i) {
            Kokkos::View<double**, layout_type, Kokkos::MemoryTraits<Kokkos::Unmanaged> > T(_T.data() + i*_dimensions*_dimensions, _dimensions, _dimensions);
            for (int j=0; j<_dimensions; ++j) {
                for (int k=0; k<_dimensions; ++k) {
                    T(j,k) = tangent_directions(i, j, k);
                }
            }
        });
        _orthonormal_tangent_space_provided = true;

        // copy data from device back to host in rearranged format
        _host_T = Kokkos::create_mirror_view(_T);
        Kokkos::deep_copy(_host_T, _T);
    }

    //! (OPTIONAL)
    //! Sets additional points for evaluation of target operation on polynomial reconstruction.
    //! If this is never called, then the target sites are the only locations where the target
    //! operations will be evaluated and applied to polynomial reconstructions.
    template <typename view_type>
    void setAuxiliaryEvaluationCoordinates(view_type evaluation_coordinates) {
        // Catches Kokkos::View<int**, Kokkos::DefaultHostExecutionSpace
        // allocate memory on device
        _additional_evaluation_coordinates = Kokkos::View<double**, layout_type>("device additional evaluation coordinates",
            evaluation_coordinates.dimension_0(), evaluation_coordinates.dimension_1());

        auto host_additional_evaluation_coordinates = Kokkos::create_mirror_view(_additional_evaluation_coordinates);
        Kokkos::deep_copy(host_additional_evaluation_coordinates, evaluation_coordinates);
        // copy data from host to device
        Kokkos::deep_copy(_additional_evaluation_coordinates, host_additional_evaluation_coordinates);
    }

    //! (OPTIONAL)
    //! Sets additional points for evaluation of target operation on polynomial reconstruction.
    //! If this is never called, then the target sites are the only locations where the target
    //! operations will be evaluated and applied to polynomial reconstructions. (device)
    template <typename view_type>
    void setAuxiliaryEvaluationCoordinates(Kokkos::View<double**, Kokkos::DefaultExecutionSpace> evaluation_coordinates) {
        _additional_evaluation_coordinates = evaluation_coordinates;
    }

    //! (OPTIONAL)
    //! Sets the additional target evaluation coordinate indices list information. Should be # targets x maximum number of indices
    //! evaluation indices for any target + 1. first entry in every row should be the number of indices for the corresponding target.
    template <typename view_type>
    void setAuxiliaryEvaluationIndicesLists(view_type indices_lists) {
        // Catches Kokkos::View<int**, Kokkos::DefaultHostExecutionSpace
        // allocate memory on device
        _additional_evaluation_indices = Kokkos::View<int**, layout_type>("device additional evaluation indices",
            indices_lists.dimension_0(), indices_lists.dimension_1());

        _host_additional_evaluation_indices = Kokkos::create_mirror_view(_additional_evaluation_indices);
        Kokkos::deep_copy(_host_additional_evaluation_indices, indices_lists);
        // copy data from host to device
        Kokkos::deep_copy(_additional_evaluation_indices, _host_additional_evaluation_indices);

        _number_of_additional_evaluation_indices 
            = Kokkos::View<int*, Kokkos::HostSpace>("number of additional evaluation indices", indices_lists.dimension_0());

        for (int i=0; i<_additional_evaluation_indices.dimension_0(); ++i) {
            _number_of_additional_evaluation_indices(i) = indices_lists(i,0);
        }
    }

    //! (OPTIONAL)
    //! Sets the additional target evaluation coordinate indices list information. Should be # targets x maximum number of indices
    //! evaluation indices for any target + 1. first entry in every row should be the number of indices for the corresponding target.
    template <typename view_type>
    void setAuxiliaryEvaluationIndicesLists(Kokkos::View<int**, Kokkos::DefaultExecutionSpace> indices_lists) {
        // allocate memory on device
        _additional_evaluation_indices = indices_lists;

        _host_additional_evaluation_indices = Kokkos::create_mirror_view(_additional_evaluation_indices);
        // copy data from host to device
        Kokkos::deep_copy(_host_additional_evaluation_indices, _additional_evaluation_indices);

        _number_of_additional_evaluation_indices 
            = Kokkos::View<int*, Kokkos::HostSpace>("number of additional evaluation indices", indices_lists.dimension_0());

        for (int i=0; i<_additional_evaluation_indices.dimension_0(); ++i) {
            _number_of_additional_evaluation_indices(i) = _host_additional_evaluation_indices(i,0);
        }
    }


    //! Type for weighting kernel for GMLS problem
    void setWeightingType( const std::string &wt) {
        if (wt == "power") {
            _weighting_type = WeightingFunctionType::Power;
        } else {
            _weighting_type = WeightingFunctionType::Gaussian;
        }
    }

    //! Type for weighting kernel for GMLS problem
    void setWeightingType( const WeightingFunctionType wt) {
        _weighting_type = wt;
    }

    //! Type for weighting kernel for curvature 
    void setCurvatureWeightingType( const std::string &wt) {
        if (wt == "power") {
            _curvature_weighting_type = WeightingFunctionType::Power;
        } else {
            _curvature_weighting_type = WeightingFunctionType::Gaussian;
        }
    }

    //! Type for weighting kernel for curvature
    void setCurvatureWeightingType( const WeightingFunctionType wt) {
        _curvature_weighting_type = wt;
    }

    //! Sets basis order to be used when reoncstructing any function
    void setPolynomialOrder(const int poly_order) {
        _poly_order = poly_order;
        _NP = this->getNP(_poly_order, _dimensions);
    }

    //! Sets basis order to be used when reoncstructing curvature
    void setCurvaturePolynomialOrder(const int manifold_poly_order) {
        _curvature_poly_order = manifold_poly_order;
    }

    //! Power for weighting kernel for GMLS problem
    void setWeightingPower(int wp) { _weighting_power = wp; }

    //! Power for weighting kernel for curvature
    void setCurvatureWeightingPower(int wp) { _curvature_weighting_power = wp; }

    //! Number of 1D quadrature points to use for staggered approach
    void setNumberOfQuadraturePoints(int np) { _number_of_quadrature_points = np; }

    //! Parses a string to determine solver type
    void setSolverType(const std::string& dense_solver_type) {
        std::string solver_type_to_lower = dense_solver_type;
        transform(solver_type_to_lower.begin(), solver_type_to_lower.end(), solver_type_to_lower.begin(), ::tolower);
        if (solver_type_to_lower == "svd") {
            _dense_solver_type = DenseSolverType::SVD;
        } else if (solver_type_to_lower == "manifold") {
            _dense_solver_type = DenseSolverType::MANIFOLD;
            _curvature_support_operations = Kokkos::View<TargetOperation*>
                ("operations needed for manifold gradient reconstruction", 1);
            auto curvature_support_operations_mirror = 
                Kokkos::create_mirror(_curvature_support_operations);
            curvature_support_operations_mirror(0) = 
                TargetOperation::GradientOfScalarPointEvaluation;
            Kokkos::deep_copy(_curvature_support_operations, curvature_support_operations_mirror);
        } else {
            _dense_solver_type = DenseSolverType::QR;
        }
    }

    //! Adds a target to the vector of target functional to be applied to the reconstruction
    void addTargets(TargetOperation lro) {
        std::vector<TargetOperation> temporary_lro_vector(1, lro);
        this->addTargets(temporary_lro_vector);
    }

    //! Adds a vector of target functionals to the vector of target functionals already to be applied to the reconstruction
    void addTargets(std::vector<TargetOperation> lro) {
        // if called multiple times with different dimensions, only the last
        // dimension called with is used for all

        // loop over requested targets
        for (int i=0; i<lro.size(); ++i) {

            bool operation_found = false;
            // loop over existing targets registered
            for (int j=0; j<_lro.size(); ++j) {

                // if found
                if (_lro[j]==lro[i]) {

                    operation_found = true;

                    // the operation should now point to where the operation is stored
                    _lro_lookup[(int)lro[i]] = j;

                    break;

                }
            }

            if (!operation_found) {
                _lro_lookup[(int)lro[i]] = _lro.size();
                _lro.push_back(lro[i]);
            }
        }

        _lro_total_offsets = Kokkos::View<int*, layout_type>("total offsets for alphas", _lro.size());
        _lro_output_tile_size = Kokkos::View<int*, layout_type>("output tile size for each operation", _lro.size());
        _lro_input_tile_size = Kokkos::View<int*, layout_type>("output tile size for each operation", _lro.size());
        _lro_output_tensor_rank = Kokkos::View<int*, layout_type>("output tensor rank", _lro.size());
        _lro_input_tensor_rank = Kokkos::View<int*, layout_type>("input tensor rank", _lro.size());

        _host_lro_total_offsets = create_mirror(_lro_total_offsets);
        _host_lro_output_tile_size = create_mirror(_lro_output_tile_size);
        _host_lro_input_tile_size = create_mirror(_lro_input_tile_size);
        _host_lro_output_tensor_rank = create_mirror(_lro_output_tensor_rank);
        _host_lro_input_tensor_rank = create_mirror(_lro_input_tensor_rank);

        int total_offset = 0; // need total offset
        int output_offset = 0;
        int input_offset = 0;
        for (int i=0; i<_lro.size(); ++i) {
            _host_lro_total_offsets(i) = total_offset;

            // allows for a tile of the product of dimension^input_tensor_rank * dimension^output_tensor_rank * the number of neighbors
            int output_tile_size = std::pow(_local_dimensions, TargetOutputTensorRank[(int)_lro[i]]);

            // the target functional input indexing is sized based on the output rank of the sampling
            // functional used
            int input_tile_size = getOutputDimensionOfSampling(_polynomial_sampling_functional);
            _host_lro_output_tile_size(i) = output_tile_size;
            _host_lro_input_tile_size(i) = input_tile_size;

            total_offset += input_tile_size * output_tile_size;
            output_offset += output_tile_size;
            input_offset += input_tile_size;

            // the target functional output rank is based on the output rank of the sampling
            // functional used
            _host_lro_input_tensor_rank(i) = SamplingOutputTensorRank[(int)_polynomial_sampling_functional];
            _host_lro_output_tensor_rank(i) = TargetOutputTensorRank[(int)_lro[i]];
        }

        _total_alpha_values = total_offset;

        Kokkos::deep_copy(_lro_total_offsets, _host_lro_total_offsets);
        Kokkos::deep_copy(_lro_output_tile_size, _host_lro_output_tile_size);
        Kokkos::deep_copy(_lro_input_tile_size, _host_lro_input_tile_size);
        Kokkos::deep_copy(_lro_output_tensor_rank, _host_lro_output_tensor_rank);
        Kokkos::deep_copy(_lro_input_tensor_rank, _host_lro_input_tensor_rank);

    }

    //! Empties the vector of target functionals to apply to the reconstruction
    void clearTargets() {
        _lro.clear();
        for (int i=0; i<TargetOperation::COUNT; ++i) {
            _lro_lookup[i] = -1;
        }
    }

    //! Sets up the batch of GMLS problems to be solved for. Provides alpha values
    //! that can later be contracted against data or degrees of freedom to form a
    //! global linear system.
    void generatePolynomialCoefficients();

    //! Calculates target operations and applies the evaluations to the previously 
    //! constructed polynomial coefficients. If polynomial coefficients were not
    //! already calculated, then generatePolynomialCoefficients() will also be
    //! called.
    void generateAlphas();

///@}




}; // GMLS Class
}; // Compadre

#endif


