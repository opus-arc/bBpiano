CXX ?= clang++
CXXFLAGS ?= -std=c++20 -O3 -DNDEBUG -Wall -Wextra -Wpedantic -Werror
CPPFLAGS ?= -Iinclude

BUILD_DIR := build
LIB_SOURCE := src/Experiment.cpp
CLI := $(BUILD_DIR)/hammerlab2
TEST := $(BUILD_DIR)/test_models

.PHONY: all clean test smoke

all: $(CLI)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(CLI): $(LIB_SOURCE) apps/hammerlab2_cli.cpp include/hammerlab2/Experiment.hpp include/hammerlab2/FeltModels.hpp | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LIB_SOURCE) apps/hammerlab2_cli.cpp -o $@

$(TEST): $(LIB_SOURCE) tests/test_models.cpp include/hammerlab2/Experiment.hpp include/hammerlab2/FeltModels.hpp | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LIB_SOURCE) tests/test_models.cpp -o $@

test: $(TEST)
	$(TEST)

smoke: $(CLI)
	./scripts/run_smoke.sh

clean:
	rm -rf $(BUILD_DIR)
