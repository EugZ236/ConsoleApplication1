#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <execution> 
#include <thread>

bool isEven(int num) {
    return num % 2 == 0;
}

template <typename Iterator, typename Predicate>
size_t parallel_count_if(Iterator begin, Iterator end, Predicate predicate, size_t num_threads) {
    const size_t size = std::distance(begin, end);
    const size_t chunk_size = (size + num_threads - 1) / num_threads;

    std::vector<std::thread> threads;
    std::vector<size_t> counts(num_threads, 0);

    for (size_t i = 0; i < num_threads; ++i) {
        threads.emplace_back([i, &counts, chunk_size, &predicate, begin, end]() {
            Iterator local_begin = std::next(begin, i * chunk_size);
            Iterator local_end = std::next(local_begin, std::min(chunk_size, static_cast<size_t>(std::distance(local_begin, end))));

            counts[i] = std::count_if(local_begin, local_end, predicate);
            });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return std::accumulate(counts.begin(), counts.end(), 0);
}

int main() {
    const size_t vector_size = 1000000;
    std::vector<int> data(vector_size, 1);

    auto start_time_seq = std::chrono::high_resolution_clock::now();
    size_t result_seq = std::count_if(data.begin(), data.end(), isEven);
    auto end_time_seq = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time_seq = end_time_seq - start_time_seq;

    auto start_time_par = std::chrono::high_resolution_clock::now();
    size_t result_par = std::count_if(std::execution::par, data.begin(), data.end(), isEven);
    auto end_time_par = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time_par = end_time_par - start_time_par;

    std::cout << "Sequential algorithm result: " << result_seq << "\n";
    std::cout << "Sequential algorithm time: " << elapsed_time_seq.count() << " seconds\n";
    std::cout << "Parallel algorithm result: " << result_par << "\n";
    std::cout << "Parallel algorithm time: " << elapsed_time_par.count() << " seconds\n";

    return 0;
}