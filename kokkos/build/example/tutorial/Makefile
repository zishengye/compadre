KOKKOS_SETTINGS=KOKKOS_SRC_PATH=/home/zishengy/Projects/compadre/kokkos CXXFLAGS="-fPIC" LDFLAGS="-ldl" GTEST_PATH=/home/zishengy/Projects/compadre/kokkos/tpls/gtest KOKKOS_PATH=/home/zishengy/Projects/compadre/kokkos/build/install

build:
	$(MAKE) -f /home/zishengy/Projects/compadre/kokkos/example/tutorial/Makefile KOKKOS_SETTINGS='KOKKOS_SRC_PATH=/home/zishengy/Projects/compadre/kokkos CXXFLAGS="-fPIC" LDFLAGS="-ldl" GTEST_PATH=/home/zishengy/Projects/compadre/kokkos/tpls/gtest KOKKOS_PATH=/home/zishengy/Projects/compadre/kokkos/build/install' KOKKOS_PATH=/home/zishengy/Projects/compadre/kokkos build

test: build
	$(MAKE) -f /home/zishengy/Projects/compadre/kokkos/example/tutorial/Makefile KOKKOS_SETTINGS='KOKKOS_SRC_PATH=/home/zishengy/Projects/compadre/kokkos CXXFLAGS="-fPIC" LDFLAGS="-ldl" GTEST_PATH=/home/zishengy/Projects/compadre/kokkos/tpls/gtest KOKKOS_PATH=/home/zishengy/Projects/compadre/kokkos/build/install' KOKKOS_PATH=/home/zishengy/Projects/compadre/kokkos test

clean:
	$(MAKE) -f /home/zishengy/Projects/compadre/kokkos/example/tutorial/Makefile KOKKOS_SETTINGS='KOKKOS_SRC_PATH=/home/zishengy/Projects/compadre/kokkos CXXFLAGS="-fPIC" LDFLAGS="-ldl" GTEST_PATH=/home/zishengy/Projects/compadre/kokkos/tpls/gtest KOKKOS_PATH=/home/zishengy/Projects/compadre/kokkos/build/install' KOKKOS_PATH=/home/zishengy/Projects/compadre/kokkos clean
