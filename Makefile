# Small makefile to build and run examples
.PHONY: build

ex1:
	./build/qmsInfer -hamDist TestBenchmark/example

ex2:
	./build/qmsInfer -hamDist TestBenchmark/example2

build:
	$(MAKE) -C build/ -j