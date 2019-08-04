export CPLUS_INCLUDE_PATH=/opt/intel/opencl/include
export LIBRARY_PATH=/opt/intel/opencl

HEADERS=cmdoptions.hpp basic.hpp cmdparser.hpp oclobject.hpp
SOURCES=cmdoptions.cpp gemm.cpp basic.cpp cmdparser.cpp oclobject.cpp

ifeq ($(CONFIG),debug)
	OPT =-O0 -g
else
	OPT =
endif

all: gemm

gemm: $(HEADERS) $(SOURCES) Makefile
	g++ $(SOURCES) -lOpenCL -oGEMM -std=gnu++0x $(OPT)

clean:
	rm -f GEMM

