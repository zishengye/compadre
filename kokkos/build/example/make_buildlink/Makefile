KOKKOS_SETTINGS=KOKKOS_SRC_PATH=/home/zishengy/Projects/compadre/kokkos CXXFLAGS="-fPIC" LDFLAGS="-ldl" GTEST_PATH=/home/zishengy/Projects/compadre/kokkos/tpls/gtest KOKKOS_PATH=/home/zishengy/Projects/compadre/kokkos/build/install

build:
	$(MAKE) -f /home/zishengy/Projects/compadre/kokkos/example/make_buildlink/Makefile KOKKOS_SRC_PATH=/home/zishengy/Projects/compadre/kokkos CXXFLAGS="-fPIC" LDFLAGS="-ldl" GTEST_PATH=/home/zishengy/Projects/compadre/kokkos/tpls/gtest KOKKOS_PATH=/home/zishengy/Projects/compadre/kokkos/build/install build

test: build
	$(MAKE) -f /home/zishengy/Projects/compadre/kokkos/example/make_buildlink/Makefile KOKKOS_SRC_PATH=/home/zishengy/Projects/compadre/kokkos CXXFLAGS="-fPIC" LDFLAGS="-ldl" GTEST_PATH=/home/zishengy/Projects/compadre/kokkos/tpls/gtest KOKKOS_PATH=/home/zishengy/Projects/compadre/kokkos/build/install test

clean:
	$(MAKE) -f /home/zishengy/Projects/compadre/kokkos/example/make_buildlink/Makefile KOKKOS_SRC_PATH=/home/zishengy/Projects/compadre/kokkos CXXFLAGS="-fPIC" LDFLAGS="-ldl" GTEST_PATH=/home/zishengy/Projects/compadre/kokkos/tpls/gtest KOKKOS_PATH=/home/zishengy/Projects/compadre/kokkos/build/install clean
