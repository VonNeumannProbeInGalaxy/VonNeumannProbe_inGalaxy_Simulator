#include "Npgs.h"
#include <chrono>
#include <iostream>
#include "Npgs.h"

int main() {
    Npgs::Logger::Init();

    Npgs::Modules::StellarGenerator sg(42);

    sg.GenStar();

    //// ��ʼ���̳߳أ�16�߳�
    //Npgs::ThreadPool::Init(16);
    //auto pool = Npgs::ThreadPool::GetInstance(16);

    //auto start = std::chrono::high_resolution_clock::now();

    //// �ύ�����̳߳�
    //for (int i = 0; i < 1; ++i) {
    //    pool->Commit([&sg]() {
    //        sg.GenStar();
    //    });
    //}

    //// �����̳߳�
    //pool->Terminate();
    //Npgs::ThreadPool::Destroy();

    //auto end = std::chrono::high_resolution_clock::now();
    //std::chrono::duration<double> duration = end - start;

    //std::cout << "Benchmark completed in " << duration.count() << " seconds." << std::endl;

    return 0;
}
