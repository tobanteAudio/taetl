CXXFLAGS ?= -std=c++2a -g -O1 -fsanitize=fuzzer,address -ftest-coverage -fcoverage-mapping -fprofile-arcs -fprofile-instr-generate
CXXWARNINGS = -Wall -Wextra -Wpedantic

INCLUDES = -I../

.PHONY: build
build:
	mkdir -p build
	$(CXX) -c $(CXXFLAGS) $(CXXWARNINGS) $(INCLUDES) -o build/fuzzer.o fuzzer.cpp
	$(CXX) $(CXXFLAGS) $(CXXWARNINGS) $(INCLUDES) -o build/fuzzer build/fuzzer.o

ifneq (,$(findstring clang,$(CXX)))
    LCOV = lcov --gcov-tool llvm-gcov.sh
else
    LCOV = lcov
endif
.PHONY: fuzz
fuzz: build
	cd build && $(LCOV) -c -i -d . --base-directory . -o base_cov.info
	cd build && ./fuzzer -max_total_time=30
	cd build && $(LCOV) -c -d . --base-directory . -o fuzz_cov.info
	cd build && $(LCOV) -a base_cov.info -a fuzz_cov.info -o cov.info
	cd build && $(LCOV) --remove cov.info "*3rd_party/*" -o cov.info
	cd build && $(LCOV) --remove cov.info "*c++*" -o cov.info
	cd build && $(LCOV) --remove cov.info "*v1*" -o cov.info
	cd build && $(LCOV) --remove cov.info "*Xcode.app*" -o cov.info
	cd build && $(LCOV) --remove cov.info "*test_*" -o cov.info
	cd build && $(LCOV) --remove cov.info "*clang*" -o cov.info

.PHONY: report
report:
	cd build && genhtml cov.info --output-directory lcov

.PHONY: clean
clean:
	rm -rf build