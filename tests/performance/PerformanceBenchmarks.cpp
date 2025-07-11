#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include "Performance/Profiler.h"
#include "Memory/MemoryManager.h"
#include "Memory/ObjectPool.h"
#include <vector>
#include <chrono>
#include <thread>
#include <random>
#include <algorithm>
#include <cmath>

// Test class for performance testing
class TestPerformanceObject {
public:
    explicit TestPerformanceObject(size_t dataSize = 1000) {
        // Allocate some data
        m_data.resize(dataSize);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0, 1000);
        
        for (auto& val : m_data) {
            val = dis(gen);
        }
    }
    
    // Perform a computationally expensive operation
    double processData() const {
        double sum = 0.0;
        
        for (auto& val : m_data) {
            sum += std::sqrt(std::sin(val) * std::cos(val) + std::tan(val / 10.0));
        }
        
        return sum;
    }
    
private:
    std::vector<double> m_data;
};

// Test fixture for performance tests
class PerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up profiler
        profiler = &Performance::ProfilingManager::getInstance();
        profiler->setEnabled(true);
        profiler->resetMetrics();
    }
    
    void TearDown() override {
        profiler->setEnabled(false);
    }
    
    Performance::ProfilingManager* profiler = nullptr;
    
    // Helper for performance testing
    void profileFunction(const std::string& name, std::function<void()> func) {
        profiler->beginScope(name);
        func();
        profiler->endScope();
        
        // Print metrics for debugging
        const auto& metrics = profiler->getMetrics();
        auto it = metrics.find(name);
        if (it != metrics.end()) {
            std::cout << name << ": " 
                      << it->second.totalTime << "ms total, "
                      << it->second.averageTimePerCall << "ms avg ("
                      << it->second.callCount << " calls)" << std::endl;
        }
    }
};

// Google Test for basic profiling functionality
TEST_F(PerformanceTest, BasicProfiling) {
    // Profile a simple function
    const int iterations = 100;
    
    profileFunction("TestFunction", [iterations]() {
        for (int i = 0; i < iterations; ++i) {
            // Some work
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });
    
    // Check profiling results
    const auto& metrics = profiler->getMetrics();
    auto it = metrics.find("TestFunction");
    ASSERT_NE(metrics.end(), it);
    
    // Should have 1 call
    EXPECT_EQ(1, it->second.callCount);
    
    // Time should be at least iterations ms
    EXPECT_GE(it->second.totalTime, iterations * 0.5);
}

// Test nested profiling
TEST_F(PerformanceTest, NestedProfiling) {
    profileFunction("OuterFunction", [this]() {
        // Do some work
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        // Call inner function
        profileFunction("InnerFunction", []() {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        });
        
        // More work
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    });
    
    // Check profiling results
    const auto& metrics = profiler->getMetrics();
    
    // Check outer function
    auto outerIt = metrics.find("OuterFunction");
    ASSERT_NE(metrics.end(), outerIt);
    EXPECT_EQ(1, outerIt->second.callCount);
    EXPECT_GE(outerIt->second.totalTime, 25.0);
    
    // Check inner function
    auto innerIt = metrics.find("InnerFunction");
    ASSERT_NE(metrics.end(), innerIt);
    EXPECT_EQ(1, innerIt->second.callCount);
    EXPECT_GE(innerIt->second.totalTime, 5.0);
    
    // Inner time should be less than outer time
    EXPECT_LT(innerIt->second.totalTime, outerIt->second.totalTime);
}

// Test performance budget
TEST_F(PerformanceTest, PerformanceBudget) {
    // Set a budget
    const double budget = 5.0;
    profiler->setPerformanceBudget("BudgetTest", budget);
    
    // Count budget exceeded events
    int budgetExceededCount = 0;
    
    // Set up callback
    profiler->setBudgetExceededCallback(
        [&budgetExceededCount](const std::string& name, double timeMs, double budgetMs) {
            budgetExceededCount++;
        }
    );
    
    // Run function that stays within budget
    profileFunction("BudgetTest", []() {
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    });
    
    // Budget should not be exceeded
    EXPECT_EQ(0, budgetExceededCount);
    
    // Run function that exceeds budget
    profileFunction("BudgetTest", []() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    });
    
    // Budget should be exceeded
    EXPECT_GE(budgetExceededCount, 1);
}

// Google Benchmark for memory allocation performance
static void BM_MemoryPoolAllocation(benchmark::State& state) {
    const size_t POOL_SIZE = state.range(0);
    
    // Create a pool
    ObjectPool<TestPerformanceObject> pool(POOL_SIZE);
    
    // Benchmark allocation/deallocation loop
    for (auto _ : state) {
        std::vector<TestPerformanceObject*> objects;
        objects.reserve(POOL_SIZE / 2);
        
        // Allocate half the pool
        for (size_t i = 0; i < POOL_SIZE / 2; ++i) {
            objects.push_back(pool.allocate(10));
        }
        
        // Process objects
        for (auto* obj : objects) {
            benchmark::DoNotOptimize(obj->processData());
        }
        
        // Deallocate
        for (auto* obj : objects) {
            pool.deallocate(obj);
        }
    }
}

// Google Benchmark for standard allocation
static void BM_StandardAllocation(benchmark::State& state) {
    const size_t COUNT = state.range(0);
    
    // Benchmark allocation/deallocation loop
    for (auto _ : state) {
        std::vector<TestPerformanceObject*> objects;
        objects.reserve(COUNT / 2);
        
        // Allocate
        for (size_t i = 0; i < COUNT / 2; ++i) {
            objects.push_back(new TestPerformanceObject(10));
        }
        
        // Process objects
        for (auto* obj : objects) {
            benchmark::DoNotOptimize(obj->processData());
        }
        
        // Deallocate
        for (auto* obj : objects) {
            delete obj;
        }
    }
}

// Register benchmarks with different sizes
BENCHMARK(BM_MemoryPoolAllocation)->Arg(64)->Arg(256)->Arg(1024);
BENCHMARK(BM_StandardAllocation)->Arg(64)->Arg(256)->Arg(1024);

// Benchmark for profiling overhead
static void BM_ProfilerOverhead(benchmark::State& state) {
    auto& profiler = Performance::ProfilingManager::getInstance();
    profiler.setEnabled(true);
    profiler.resetMetrics();
    
    // Benchmark profiling overhead
    for (auto _ : state) {
        profiler.beginScope("OverheadTest");
        // Minimal operation
        benchmark::DoNotOptimize(1 + 1);
        profiler.endScope();
    }
    
    profiler.setEnabled(false);
}

BENCHMARK(BM_ProfilerOverhead);

// Create a performance regression test
class PerformanceRegressionTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create baseline data
        createBaselineData();
    }
    
    void createBaselineData() {
        std::ofstream baselineFile("performance_baseline.csv");
        baselineFile << "Scope,Time(ms)\n";
        baselineFile << "TestFunction,50.0\n";
        baselineFile << "CriticalFunction,20.0\n";
        baselineFile << "RenderUpdate,16.0\n";
        baselineFile.close();
    }
    
    Performance::ProfilingManager profiler;
};

// Test performance regression detection
TEST_F(PerformanceRegressionTest, DetectRegressions) {
    // Set up profiler with test metrics
    profiler.setEnabled(true);
    profiler.resetMetrics();
    
    // Simulate functions with different performance characteristics
    
    // 1. Simulate a function that stays within expected time
    profiler.beginScope("TestFunction");
    std::this_thread::sleep_for(std::chrono::milliseconds(45));
    profiler.endScope();
    
    // 2. Simulate a function that exceeds expected time (regression)
    profiler.beginScope("CriticalFunction");
    std::this_thread::sleep_for(std::chrono::milliseconds(30)); // 50% slower
    profiler.endScope();
    
    // 3. Another function within bounds
    profiler.beginScope("RenderUpdate");
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
    profiler.endScope();
    
    // Check for regressions with 20% threshold
    int regressionCount = profiler.detectRegressions("performance_baseline.csv", 20.0);
    
    // Should detect one regression (CriticalFunction)
    EXPECT_EQ(1, regressionCount);
}

// Main for running Google Benchmark in addition to Google Test
int main(int argc, char** argv) {
    // Initialize Google Test
    ::testing::InitGoogleTest(&argc, argv);
    
    // Run tests first
    int testResult = RUN_ALL_TESTS();
    
    // Then run benchmarks
    ::benchmark::Initialize(&argc, argv);
    ::benchmark::RunSpecifiedBenchmarks();
    
    return testResult;
}