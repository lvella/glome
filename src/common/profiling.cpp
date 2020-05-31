#include "profiling.hpp"

using namespace std::chrono_literals;

ProfillingAggregator globalProfiler(5000ms);
