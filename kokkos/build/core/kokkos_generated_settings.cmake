#Global Settings used to generate this library
set(KOKKOS_PATH CACHE FILEPATH "Kokkos installation path" FORCE)
set(KOKKOS_GMAKE_DEVICES "Pthread" CACHE STRING "Kokkos devices list" FORCE)
set(KOKKOS_GMAKE_ARCH "" CACHE STRING "Kokkos architecture flags" FORCE)
set(KOKKOS_DEBUG_CMAKE OFF CACHE BOOL "Kokkos debug enabled ?" FORCE)
set(KOKKOS_GMAKE_USE_TPLS "" CACHE STRING "Kokkos templates list" FORCE)
set(KOKKOS_CXX_STANDARD c++11 CACHE STRING "Kokkos C++ standard" FORCE)
set(KOKKOS_GMAKE_OPTIONS "" CACHE STRING "Kokkos options" FORCE)
set(KOKKOS_GMAKE_CUDA_OPTIONS "" CACHE STRING "Kokkos Cuda options" FORCE)
set(KOKKOS_GMAKE_TPL_INCLUDE_DIRS "" CACHE STRING "Kokkos TPL include directories" FORCE)
set(KOKKOS_GMAKE_TPL_LIBRARY_DIRS "" CACHE STRING "Kokkos TPL library directories" FORCE)
set(KOKKOS_GMAKE_TPL_LIBRARY_NAMES " dl pthread" CACHE STRING "Kokkos TPL library names" FORCE)
if(NOT DEFINED ENV{NVCC_WRAPPER})
set(NVCC_WRAPPER /home/zishengy/Projects/compadre/kokkos/bin/nvcc_wrapper CACHE FILEPATH "Path to command nvcc_wrapper" FORCE)
else()
  set(NVCC_WRAPPER $ENV{NVCC_WRAPPER} CACHE FILEPATH "Path to command nvcc_wrapper")
endif()

#Source and Header files of Kokkos relative to KOKKOS_PATH
set(KOKKOS_HEADERS /opt/kokkos/include/Kokkos_UniqueToken.hpp /opt/kokkos/include/Kokkos_CopyViews.hpp /opt/kokkos/include/Kokkos_NumericTraits.hpp /opt/kokkos/include/Kokkos_Timer.hpp /opt/kokkos/include/Kokkos_Pair.hpp /opt/kokkos/include/Kokkos_ExecPolicy.hpp /opt/kokkos/include/Kokkos_Macros.hpp /opt/kokkos/include/Kokkos_ScratchSpace.hpp /opt/kokkos/include/Kokkos_HBWSpace.hpp /opt/kokkos/include/Kokkos_ROCmSpace.hpp /opt/kokkos/include/Kokkos_ROCm.hpp /opt/kokkos/include/KokkosExp_MDRangePolicy.hpp /opt/kokkos/include/Kokkos_Atomic.hpp /opt/kokkos/include/Kokkos_MemoryPool.hpp /opt/kokkos/include/Kokkos_MasterLock.hpp /opt/kokkos/include/Kokkos_Threads.hpp /opt/kokkos/include/Kokkos_Layout.hpp /opt/kokkos/include/Kokkos_Serial.hpp /opt/kokkos/include/Kokkos_Qthreads.hpp /opt/kokkos/include/Kokkos_Crs.hpp /opt/kokkos/include/Kokkos_Parallel_Reduce.hpp /opt/kokkos/include/Kokkos_Parallel.hpp /opt/kokkos/include/Kokkos_Cuda.hpp /opt/kokkos/include/Kokkos_View.hpp /opt/kokkos/include/Kokkos_TaskPolicy.hpp /opt/kokkos/include/Kokkos_CudaSpace.hpp /opt/kokkos/include/Kokkos_Core.hpp /opt/kokkos/include/Kokkos_WorkGraphPolicy.hpp /opt/kokkos/include/Kokkos_Vectorization.hpp /opt/kokkos/include/Kokkos_OpenMP.hpp /opt/kokkos/include/Kokkos_hwloc.hpp /opt/kokkos/include/Kokkos_Array.hpp /opt/kokkos/include/Kokkos_OpenMPTargetSpace.hpp /opt/kokkos/include/Kokkos_Profiling_ProfileSection.hpp /opt/kokkos/include/Kokkos_OpenMPTarget.hpp /opt/kokkos/include/Kokkos_Core_fwd.hpp /opt/kokkos/include/Kokkos_Complex.hpp /opt/kokkos/include/Kokkos_HostSpace.hpp /opt/kokkos/include/Kokkos_Concepts.hpp /opt/kokkos/include/Kokkos_TaskScheduler.hpp /opt/kokkos/include/Kokkos_AnonymousSpace.hpp /opt/kokkos/include/Kokkos_MemoryTraits.hpp /opt/kokkos/include/impl/Kokkos_HostThreadTeam.hpp /opt/kokkos/include/impl/Kokkos_Timer.hpp /opt/kokkos/include/impl/Kokkos_ViewCtor.hpp /opt/kokkos/include/impl/Kokkos_Error.hpp /opt/kokkos/include/impl/Kokkos_Atomic_Fetch_Or.hpp /opt/kokkos/include/impl/Kokkos_ViewFillCopyETIDecl.hpp /opt/kokkos/include/impl/Kokkos_Spinwait.hpp /opt/kokkos/include/impl/Kokkos_Traits.hpp /opt/kokkos/include/impl/Kokkos_ViewMapping.hpp /opt/kokkos/include/impl/Kokkos_Atomic_Compare_Exchange_Strong.hpp /opt/kokkos/include/impl/Kokkos_FunctorAdapter.hpp /opt/kokkos/include/impl/Kokkos_Volatile_Load.hpp /opt/kokkos/include/impl/Kokkos_FunctorAnalysis.hpp /opt/kokkos/include/impl/Kokkos_Atomic_Fetch_Add.hpp /opt/kokkos/include/impl/Kokkos_Tags.hpp /opt/kokkos/include/impl/Kokkos_Profiling_Interface.hpp /opt/kokkos/include/impl/Kokkos_ViewArray.hpp /opt/kokkos/include/impl/Kokkos_HostBarrier.hpp /opt/kokkos/include/impl/Kokkos_Atomic_Windows.hpp /opt/kokkos/include/impl/Kokkos_CPUDiscovery.hpp /opt/kokkos/include/impl/KokkosExp_Host_IterateTile.hpp /opt/kokkos/include/impl/Kokkos_Profiling_DeviceInfo.hpp /opt/kokkos/include/impl/Kokkos_SharedAlloc.hpp /opt/kokkos/include/impl/Kokkos_Atomic_Generic.hpp /opt/kokkos/include/impl/Kokkos_Atomic_Assembly.hpp /opt/kokkos/include/impl/Kokkos_ClockTic.hpp /opt/kokkos/include/impl/Kokkos_ViewTile.hpp /opt/kokkos/include/impl/Kokkos_ConcurrentBitset.hpp /opt/kokkos/include/impl/Kokkos_ViewFillCopyETIAvail.hpp /opt/kokkos/include/impl/Kokkos_Utilities.hpp /opt/kokkos/include/impl/Kokkos_Atomic_Increment.hpp /opt/kokkos/include/impl/Kokkos_Atomic_Decrement.hpp /opt/kokkos/include/impl/Kokkos_StaticAssert.hpp /opt/kokkos/include/impl/Kokkos_BitOps.hpp /opt/kokkos/include/impl/Kokkos_Memory_Fence.hpp /opt/kokkos/include/impl/KokkosExp_ViewMapping.hpp /opt/kokkos/include/impl/Kokkos_ViewUniformType.hpp /opt/kokkos/include/impl/Kokkos_OldMacros.hpp /opt/kokkos/include/impl/Kokkos_Atomic_Fetch_And.hpp /opt/kokkos/include/impl/Kokkos_TaskQueue_impl.hpp /opt/kokkos/include/impl/Kokkos_Atomic_Fetch_Sub.hpp /opt/kokkos/include/impl/Kokkos_Serial_WorkGraphPolicy.hpp /opt/kokkos/include/impl/Kokkos_AnalyzePolicy.hpp /opt/kokkos/include/impl/Kokkos_TaskQueue.hpp /opt/kokkos/include/impl/Kokkos_Atomic_Exchange.hpp /opt/kokkos/include/impl/Kokkos_ViewLayoutTiled.hpp /opt/kokkos/include/impl/Kokkos_PhysicalLayout.hpp /opt/kokkos/include/impl/Kokkos_Atomic_View.hpp /opt/kokkos/include/impl/Kokkos_Serial_Task.hpp /opt/kokkos/include/Kokkos_Functional.hpp /opt/kokkos/include/Kokkos_StaticCrsGraph.hpp /opt/kokkos/include/Kokkos_ErrorReporter.hpp /opt/kokkos/include/Kokkos_OffsetView.hpp /opt/kokkos/include/Kokkos_DualView.hpp /opt/kokkos/include/Kokkos_Vector.hpp /opt/kokkos/include/Kokkos_ScatterView.hpp /opt/kokkos/include/Kokkos_Bitset.hpp /opt/kokkos/include/Kokkos_UnorderedMap.hpp /opt/kokkos/include/Kokkos_DynamicView.hpp /opt/kokkos/include/Kokkos_DynRankView.hpp /opt/kokkos/include/impl/Kokkos_StaticCrsGraph_factory.hpp /opt/kokkos/include/impl/Kokkos_Functional_impl.hpp /opt/kokkos/include/impl/Kokkos_Bitset_impl.hpp /opt/kokkos/include/impl/Kokkos_UnorderedMap_impl.hpp /opt/kokkos/include/Kokkos_Random.hpp /opt/kokkos/include/Kokkos_Sort.hpp /opt/kokkos/include/Threads/Kokkos_Threads_ViewCopyETIDecl.hpp /opt/kokkos/include/Threads/Kokkos_Threads_Parallel.hpp /opt/kokkos/include/Threads/Kokkos_Threads_WorkGraphPolicy.hpp /opt/kokkos/include/Threads/Kokkos_Threads_ViewCopyETIAvail.hpp /opt/kokkos/include/Threads/Kokkos_ThreadsExec.hpp /opt/kokkos/include/Threads/Kokkos_ThreadsTeam.hpp CACHE STRING "Kokkos headers list" FORCE)
set(KOKKOS_HEADERS_IMPL CACHE STRING "Kokkos headers impl list" FORCE)
set(KOKKOS_HEADERS_CUDA CACHE STRING "Kokkos headers Cuda list" FORCE)
set(KOKKOS_HEADERS_OPENMP CACHE STRING "Kokkos headers OpenMP list" FORCE)
set(KOKKOS_HEADERS_ROCM CACHE STRING "Kokkos headers ROCm list" FORCE)
set(KOKKOS_HEADERS_THREADS /opt/kokkos/include/Threads/Kokkos_Threads_ViewCopyETIDecl.hpp /opt/kokkos/include/Threads/Kokkos_Threads_Parallel.hpp /opt/kokkos/include/Threads/Kokkos_Threads_WorkGraphPolicy.hpp /opt/kokkos/include/Threads/Kokkos_Threads_ViewCopyETIAvail.hpp /opt/kokkos/include/Threads/Kokkos_ThreadsExec.hpp /opt/kokkos/include/Threads/Kokkos_ThreadsTeam.hpp CACHE STRING "Kokkos headers Threads list" FORCE)
set(KOKKOS_HEADERS_QTHREADS CACHE STRING "Kokkos headers QThreads list" FORCE)

#Variables used in application Makefiles
set(KOKKOS_OS Linux CACHE STRING "" FORCE)
set(KOKKOS_CPP_DEPENDS KokkosCore_config.h /opt/kokkos/include/Kokkos_Serial.hpp /opt/kokkos/include/Kokkos_hwloc.hpp /opt/kokkos/include/Kokkos_ExecPolicy.hpp /opt/kokkos/include/Kokkos_OpenMPTargetSpace.hpp /opt/kokkos/include/Kokkos_Qthreads.hpp /opt/kokkos/include/Kokkos_Macros.hpp /opt/kokkos/include/Kokkos_Timer.hpp /opt/kokkos/include/Kokkos_Pair.hpp /opt/kokkos/include/Kokkos_UniqueToken.hpp /opt/kokkos/include/Kokkos_Profiling_ProfileSection.hpp /opt/kokkos/include/Kokkos_OpenMP.hpp /opt/kokkos/include/Kokkos_Vectorization.hpp /opt/kokkos/include/Kokkos_Atomic.hpp /opt/kokkos/include/Kokkos_Crs.hpp /opt/kokkos/include/Kokkos_CudaSpace.hpp /opt/kokkos/include/Kokkos_ScratchSpace.hpp /opt/kokkos/include/Kokkos_Core_fwd.hpp /opt/kokkos/include/Kokkos_CopyViews.hpp /opt/kokkos/include/Kokkos_Parallel_Reduce.hpp /opt/kokkos/include/Kokkos_OpenMPTarget.hpp /opt/kokkos/include/Kokkos_ROCmSpace.hpp /opt/kokkos/include/Kokkos_ROCm.hpp /opt/kokkos/include/Kokkos_HBWSpace.hpp /opt/kokkos/include/Kokkos_TaskScheduler.hpp /opt/kokkos/include/Kokkos_Cuda.hpp /opt/kokkos/include/Kokkos_Array.hpp /opt/kokkos/include/Kokkos_AnonymousSpace.hpp /opt/kokkos/include/KokkosExp_MDRangePolicy.hpp /opt/kokkos/include/Kokkos_Parallel.hpp /opt/kokkos/include/Kokkos_Complex.hpp /opt/kokkos/include/Kokkos_HostSpace.hpp /opt/kokkos/include/Kokkos_View.hpp /opt/kokkos/include/Kokkos_MasterLock.hpp /opt/kokkos/include/Kokkos_Core.hpp /opt/kokkos/include/Kokkos_WorkGraphPolicy.hpp /opt/kokkos/include/Kokkos_MemoryPool.hpp /opt/kokkos/include/Kokkos_TaskPolicy.hpp /opt/kokkos/include/Kokkos_NumericTraits.hpp /opt/kokkos/include/Kokkos_Concepts.hpp /opt/kokkos/include/Kokkos_Threads.hpp /opt/kokkos/include/Kokkos_Layout.hpp /opt/kokkos/include/Kokkos_MemoryTraits.hpp /opt/kokkos/include/impl/Kokkos_OldMacros.hpp /opt/kokkos/include/impl/Kokkos_ViewLayoutTiled.hpp /opt/kokkos/include/impl/Kokkos_Profiling_DeviceInfo.hpp /opt/kokkos/include/impl/Kokkos_ViewTile.hpp /opt/kokkos/include/impl/Kokkos_ConcurrentBitset.hpp /opt/kokkos/include/impl/Kokkos_Atomic_Fetch_Sub.hpp /opt/kokkos/include/impl/Kokkos_Serial_Task.hpp /opt/kokkos/include/impl/Kokkos_Atomic_Windows.hpp /opt/kokkos/include/impl/Kokkos_Memory_Fence.hpp /opt/kokkos/include/impl/Kokkos_FunctorAnalysis.hpp /opt/kokkos/include/impl/Kokkos_Atomic_Fetch_And.hpp /opt/kokkos/include/impl/KokkosExp_ViewMapping.hpp /opt/kokkos/include/impl/Kokkos_HostThreadTeam.hpp /opt/kokkos/include/impl/Kokkos_ViewFillCopyETIAvail.hpp /opt/kokkos/include/impl/Kokkos_SharedAlloc.hpp /opt/kokkos/include/impl/Kokkos_Spinwait.hpp /opt/kokkos/include/impl/Kokkos_ViewFillCopyETIDecl.hpp /opt/kokkos/include/impl/Kokkos_Volatile_Load.hpp /opt/kokkos/include/impl/Kokkos_Atomic_Fetch_Add.hpp /opt/kokkos/include/impl/Kokkos_Atomic_Generic.hpp /opt/kokkos/include/impl/Kokkos_Atomic_Compare_Exchange_Strong.hpp /opt/kokkos/include/impl/Kokkos_FunctorAdapter.hpp /opt/kokkos/include/impl/Kokkos_Serial_WorkGraphPolicy.hpp /opt/kokkos/include/impl/Kokkos_Atomic_Assembly.hpp /opt/kokkos/include/impl/Kokkos_Profiling_Interface.hpp /opt/kokkos/include/impl/Kokkos_ViewUniformType.hpp /opt/kokkos/include/impl/Kokkos_ViewCtor.hpp /opt/kokkos/include/impl/Kokkos_TaskQueue.hpp /opt/kokkos/include/impl/Kokkos_CPUDiscovery.hpp /opt/kokkos/include/impl/Kokkos_ViewArray.hpp /opt/kokkos/include/impl/Kokkos_PhysicalLayout.hpp /opt/kokkos/include/impl/Kokkos_HostBarrier.hpp /opt/kokkos/include/impl/Kokkos_Atomic_Decrement.hpp /opt/kokkos/include/impl/Kokkos_Error.hpp /opt/kokkos/include/impl/Kokkos_Atomic_View.hpp /opt/kokkos/include/impl/KokkosExp_Host_IterateTile.hpp /opt/kokkos/include/impl/Kokkos_ClockTic.hpp /opt/kokkos/include/impl/Kokkos_Atomic_Exchange.hpp /opt/kokkos/include/impl/Kokkos_TaskQueue_impl.hpp /opt/kokkos/include/impl/Kokkos_StaticAssert.hpp /opt/kokkos/include/impl/Kokkos_BitOps.hpp /opt/kokkos/include/impl/Kokkos_Traits.hpp /opt/kokkos/include/impl/Kokkos_Atomic_Increment.hpp /opt/kokkos/include/impl/Kokkos_Tags.hpp /opt/kokkos/include/impl/Kokkos_Timer.hpp /opt/kokkos/include/impl/Kokkos_ViewMapping.hpp /opt/kokkos/include/impl/Kokkos_Utilities.hpp /opt/kokkos/include/impl/Kokkos_AnalyzePolicy.hpp /opt/kokkos/include/impl/Kokkos_Atomic_Fetch_Or.hpp /opt/kokkos/include/Kokkos_DynRankView.hpp /opt/kokkos/include/Kokkos_Vector.hpp /opt/kokkos/include/Kokkos_StaticCrsGraph.hpp /opt/kokkos/include/Kokkos_Functional.hpp /opt/kokkos/include/Kokkos_DualView.hpp /opt/kokkos/include/Kokkos_DynamicView.hpp /opt/kokkos/include/Kokkos_Bitset.hpp /opt/kokkos/include/Kokkos_UnorderedMap.hpp /opt/kokkos/include/Kokkos_ScatterView.hpp /opt/kokkos/include/Kokkos_ErrorReporter.hpp /opt/kokkos/include/Kokkos_OffsetView.hpp /opt/kokkos/include/impl/Kokkos_StaticCrsGraph_factory.hpp /opt/kokkos/include/impl/Kokkos_Functional_impl.hpp /opt/kokkos/include/impl/Kokkos_Bitset_impl.hpp /opt/kokkos/include/impl/Kokkos_UnorderedMap_impl.hpp /opt/kokkos/include/Kokkos_Random.hpp /opt/kokkos/include/Kokkos_Sort.hpp /opt/kokkos/include/Threads/Kokkos_Threads_Parallel.hpp /opt/kokkos/include/Threads/Kokkos_Threads_WorkGraphPolicy.hpp /opt/kokkos/include/Threads/Kokkos_Threads_ViewCopyETIAvail.hpp /opt/kokkos/include/Threads/Kokkos_Threads_ViewCopyETIDecl.hpp /opt/kokkos/include/Threads/Kokkos_ThreadsExec.hpp /opt/kokkos/include/Threads/Kokkos_ThreadsTeam.hpp CACHE STRING "" FORCE)
set(KOKKOS_LINK_DEPENDS libkokkos.a CACHE STRING "" FORCE)
set(KOKKOS_CXXFLAGS -I./ -I/opt/kokkos/include -I/opt/kokkos/include -I/opt/kokkos/include -I/opt/kokkos/include/eti --std=c++11 CACHE STRING "" FORCE)
set(KOKKOS_CPPFLAGS CACHE STRING "" FORCE)
set(KOKKOS_LDFLAGS -L/opt/kokkos/lib CACHE STRING "" FORCE)
set(KOKKOS_CXXLDFLAGS -L/opt/kokkos/lib CACHE STRING "" FORCE)
set(KOKKOS_LIBS -lkokkos -ldl -lpthread CACHE STRING "" FORCE)
set(KOKKOS_EXTRA_LIBS -ldl -lpthread CACHE STRING "" FORCE)
set(KOKKOS_LINK_FLAGS CACHE STRING "extra flags to the link step (e.g. OpenMP)" FORCE)

#Internal settings which need to propagated for Kokkos examples
set(KOKKOS_INTERNAL_USE_CUDA 0 CACHE STRING "" FORCE)
set(KOKKOS_INTERNAL_USE_OPENMP 0 CACHE STRING "" FORCE)
set(KOKKOS_INTERNAL_USE_PTHREADS 1 CACHE STRING "" FORCE)
set(KOKKOS_INTERNAL_USE_SERIAL 0 CACHE STRING "" FORCE)
set(KOKKOS_INTERNAL_USE_ROCM 0 CACHE STRING "" FORCE)
set(KOKKOS_INTERNAL_USE_QTHREADS 0 CACHE STRING "" FORCE)
set(KOKKOS_SRC /home/zishengy/Projects/compadre/kokkos/core/src/impl/Kokkos_ExecPolicy.cpp /home/zishengy/Projects/compadre/kokkos/core/src/impl/Kokkos_Spinwait.cpp /home/zishengy/Projects/compadre/kokkos/core/src/impl/Kokkos_SharedAlloc.cpp /home/zishengy/Projects/compadre/kokkos/core/src/impl/Kokkos_Profiling_Interface.cpp /home/zishengy/Projects/compadre/kokkos/core/src/impl/Kokkos_hwloc.cpp /home/zishengy/Projects/compadre/kokkos/core/src/impl/Kokkos_CPUDiscovery.cpp /home/zishengy/Projects/compadre/kokkos/core/src/impl/Kokkos_HostBarrier.cpp /home/zishengy/Projects/compadre/kokkos/core/src/impl/Kokkos_Core.cpp /home/zishengy/Projects/compadre/kokkos/core/src/impl/Kokkos_Error.cpp /home/zishengy/Projects/compadre/kokkos/core/src/impl/Kokkos_HostSpace.cpp /home/zishengy/Projects/compadre/kokkos/core/src/impl/Kokkos_HostThreadTeam.cpp /home/zishengy/Projects/compadre/kokkos/core/src/impl/Kokkos_MemoryPool.cpp /home/zishengy/Projects/compadre/kokkos/containers/src/impl/Kokkos_UnorderedMap_impl.cpp /home/zishengy/Projects/compadre/kokkos/core/src/Threads/Kokkos_ThreadsExec.cpp /home/zishengy/Projects/compadre/kokkos/core/src/Threads/Kokkos_ThreadsExec_base.cpp CACHE STRING "Kokkos source list" FORCE)
set(KOKKOS_CXX_FLAGS -I./ -I/home/zishengy/Projects/compadre/kokkos/core/src -I/home/zishengy/Projects/compadre/kokkos/containers/src -I/home/zishengy/Projects/compadre/kokkos/algorithms/src -I/home/zishengy/Projects/compadre/kokkos/core/src/eti --std=c++11)
set(KOKKOS_CPP_FLAGS )
set(KOKKOS_LD_FLAGS -L/home/zishengy/Projects/compadre/kokkos/build/core)
set(KOKKOS_LIBS_LIST "-lkokkos -ldl -lpthread")
set(KOKKOS_EXTRA_LIBS_LIST "-ldl -lpthread")
set(KOKKOS_LINK_FLAGS )
