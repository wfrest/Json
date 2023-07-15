#include <benchmark/benchmark.h>
#include <string>
#include "nlohmann/json.hpp"

static void AcceptValidJsonBenchmark(benchmark::State& state) {
    std::string valid_json = R"({ "name": "John", "age": 30 })";
    for (auto _ : state) {
        nlohmann::json::accept(valid_json);
    }
}
BENCHMARK(AcceptValidJsonBenchmark);

static void AcceptInvalidJsonBenchmark(benchmark::State& state) {
    std::string invalid_json = R"({ "name": "John", "age": })";
    for (auto _ : state) {
        nlohmann::json::accept(invalid_json);
    }
}
BENCHMARK(AcceptInvalidJsonBenchmark);

BENCHMARK_MAIN();
