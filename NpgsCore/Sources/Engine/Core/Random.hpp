#pragma once

#include <random>
#include "Engine/Core/Base.h"

_NPGS_BEGIN

template <typename Ty = float, typename RandomEngine = std::mt19937>
class NPGS_API Distribution {
public:
    virtual ~Distribution() = default;
    virtual Ty operator()(RandomEngine& Engine) = 0;
    virtual Ty Generate(RandomEngine& Engine) = 0;
};

template <typename Ty = int, typename RandomEngine = std::mt19937>
class NPGS_API UniformIntDistribution : public Distribution<Ty> {
public:
    UniformIntDistribution(Ty Min, Ty Max) : _Distribution(Min, Max) {}

    Ty operator()(RandomEngine& Engine) override {
        return _Distribution(Engine);
    }

    Ty Generate(RandomEngine& Engine) override {
        return operator()(Engine);
    }

private:
    std::uniform_int_distribution<Ty> _Distribution;
};

template <typename Ty = float, typename RandomEngine = std::mt19937>
class NPGS_API UniformRealDistribution : public Distribution<Ty, RandomEngine> {
public:
    UniformRealDistribution(Ty Min, Ty Max) : _Distribution(Min, Max) {}

    Ty operator()(RandomEngine& Engine) override {
        return _Distribution(Engine);
    }

    Ty Generate(RandomEngine& Engine) override {
        return operator()(Engine);
    }

private:
    std::uniform_real_distribution<Ty> _Distribution;
};

template <typename Ty = float, typename RandomEngine = std::mt19937>
class NPGS_API NormalDistribution : public Distribution<Ty, RandomEngine> {
public:
    NormalDistribution(Ty Mean, Ty Sigma) : _Distribution(Mean, Sigma) {}

    Ty operator()(RandomEngine& Engine) override {
        return _Distribution(Engine);
    }

    Ty Generate(RandomEngine& Engine) override {
        return operator()(Engine);
    }

private:
    std::normal_distribution<Ty> _Distribution;
};

template <typename Ty = float, typename RandomEngine = std::mt19937>
class NPGS_API LogNormalDistribution : public Distribution<Ty, RandomEngine> {
public:
    LogNormalDistribution(Ty Mean, Ty Sigma) : _Distribution(Mean, Sigma) {}

    Ty operator()(RandomEngine& Engine) override {
        return _Distribution(Engine);
    }

    Ty Generate(RandomEngine& Engine) override {
        return operator()(Engine);
    }

private:
    std::lognormal_distribution<Ty> _Distribution;
};

template <typename RandomEngine = std::mt19937>
class NPGS_API BernoulliDistribution : public Distribution<double, RandomEngine> {
public:
    BernoulliDistribution(double Probability) : _Distribution(Probability) {}

    double operator()(RandomEngine& Engine) override {
        return _Distribution(Engine);
    }

    double Generate(RandomEngine& Engine) override {
        return operator()(Engine);
    }

private:
    std::bernoulli_distribution _Distribution;
};

_NPGS_END
