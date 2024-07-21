#include <thread>

#include "Engine/Core/Modules/Stellar/StellarGenerator.h"
#include "Engine/Core/Logger.h"
#include "Engine/Core/ThreadPool.h"

using namespace Npgs;
using namespace Npgs::Modules;

void func(StellarGenerator& generator) {
    for (int i = 0; i < 1000000; ++i) {
        // 生成恒星的基础属性
        auto properties = generator.GenBasicProperties();

        // 使用Logger记录生成的恒星属性
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
    // 初始化Logger
    Logger::Init();

    // 创建StellarGenerator实例，可以指定一个种子以保证结果的可重复性
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

