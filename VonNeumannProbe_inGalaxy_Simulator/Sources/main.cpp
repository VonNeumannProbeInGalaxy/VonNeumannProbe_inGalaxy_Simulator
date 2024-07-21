#include <thread>

#include "Engine/Core/Modules/Stellar/StellarGenerator.h"
#include "Engine/Core/Logger.h"
#include "Engine/Core/ThreadPool.h"

using namespace Npgs;
using namespace Npgs::Modules;

void func(StellarGenerator& generator) {
    for (int i = 0; i < 1000000; ++i) {
        // ���ɺ��ǵĻ�������
        auto properties = generator.GenBasicProperties();

        // ʹ��Logger��¼���ɵĺ�������
        if (properties.Mass >= 20) {
            Logger::GetClientLogger()->trace("Star #{}: Name = {}, Mass = {:.2f} Msun, Age = {:.2f} years, Fe/H = {:.2f}",
                i + 1,
                properties.StarSys.Name,
                properties.Mass,
                properties.Age,
                properties.FeH);
        }
    }
}

int main() {
    // ��ʼ��Logger
    Logger::Init();

    // ����StellarGeneratorʵ��������ָ��һ�������Ա�֤����Ŀ��ظ���
    std::vector<int> seeds{ 42, 34, 55, 24, 114514, 1919810, 114, 514, 1919, 810, 123, 456, 789, 1415, 9265, 3589 };

    ThreadPool* p = ThreadPool::GetInstance(16);
    for (int i = 0; i != 16; ++i) {
        StellarGenerator generator(seeds[i]);
        p->Commit(func, generator);
    }
    p->Terminate();
    p->Destroy();

    return 0;
}

