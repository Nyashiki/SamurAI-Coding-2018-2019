CXX = g++
CXXFLAGS = -std=c++14 -Wall -pedantic -fpermissive
INCLUDES =
LIBRARIES =

ifeq ($(TARGET),release)
	CXXFLAGS += -fno-exceptions -fno-rtti -Ofast -DNDEBUG
endif
ifeq ($(TARGET),debug)
	CXXFLAGS += -g3 -DDEBUG_MODE -D_GLIBCXX_DEBUG
endif
ifeq ($(TARGET),test)
	INCLUDES += -Igoogletest/include
	LIBRARIES += -Lgoogletest -lgtest -lgtest_main -lpthread
	CXXFLAGS += -g3 -D_GLIBCXX_DEBUG
endif

output = ./samurai-coding
object_dir = ./
sources = $(filter-out main.cc, $(wildcard *.cc)) $(wildcard agent/*.cc)
objects = $(sources:%.cc=$(object_dir)/%.o)
dependencies = $(objects:%.o=%.d)

test_output = ./test/runtest
test_object_dir = ./test/
test_sources = $(wildcard ./test/*.cc)
test_objects = $(test_sources:%.cc=%.o)
test_dependencies = $(test_objects:%.o=%.d)

.PHONY: release debug test
release debug:
	$(MAKE) TARGET=$@ executable
test:
	$(MAKE) TARGET=$@ maketest

.PHONY: clean
clean:
	rm -rf $(objects) $(output)
	rm -rf $(test_objects) $(test_output)

.PHONY: executable
executable: $(objects) main.cc
	$(CXX) $(CXXFLAGS) -o $(output) main.cc $(objects) $(LIBRARIES)

maketest: $(test_objects) $(objects)
	$(CXX) $(CXXFLAGS) -o $(test_output) $(test_objects) $(objects) $(LIBRARIES)

runtest: test
	./test/runtest

$(object_dir)/%.o : %.cc
	$(CXX) -c $(CXXFLAGS) -o $@ $(INCLUDES) $<

$(test_object_dir)/%.o : %.cc
	$(CXX) -c $(CXXFLAGS) -o $@ $(INCLUDES) $<
