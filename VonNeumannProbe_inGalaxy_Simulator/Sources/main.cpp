#include "Npgs.h"
#include <chrono>
#include <iostream>
#include "Npgs.h"

int main() {
    Npgs::Logger::Init();

    Npgs::Modules::StellarGenerator sg(42);

    sg.GenStar();

    //// 初始化线程池，16线程
    //Npgs::ThreadPool::Init(16);
    //auto pool = Npgs::ThreadPool::GetInstance(16);

    //auto start = std::chrono::high_resolution_clock::now();

    //// 提交任务到线程池
    //for (int i = 0; i < 1; ++i) {
    //    pool->Commit([&sg]() {
    //        sg.GenStar();
    //    });
    //}

    //// 销毁线程池
    //pool->Terminate();
    //Npgs::ThreadPool::Destroy();

    //auto end = std::chrono::high_resolution_clock::now();
    //std::chrono::duration<double> duration = end - start;

    //std::cout << "Benchmark completed in " << duration.count() << " seconds." << std::endl;

    return 0;
}
