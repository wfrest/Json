#include <benchmark/benchmark.h>
#include <string>
#include "Json.h"
#include <fstream>

static void AcceptValidJsonBenchmark(benchmark::State& state) {
    std::string valid_json = R"({ "name": "John", "age": 30 })";
    for (auto _ : state) {
        wfrest::Json::parse(valid_json);
    }
}
BENCHMARK(AcceptValidJsonBenchmark);

static void AcceptInvalidJsonBenchmark(benchmark::State& state) {
    std::string invalid_json = R"({ "name": "John", "age": })";
    for (auto _ : state) {
        wfrest::Json::parse(invalid_json);
    }
}
BENCHMARK(AcceptInvalidJsonBenchmark);

BENCHMARK_MAIN();
