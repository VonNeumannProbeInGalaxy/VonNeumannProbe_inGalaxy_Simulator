#pragma once

#include <random>
#include "Engine/Core/Base.h"

_NPGS_BEGIN

template <typename Ty = float, typename RandomEngine = std::mt19937>
class Distribution {
public:
    virtual ~Distribution() = default;
    virtual Ty operator()(RandomEngine& Engine) = 0;
    virtual Ty Generate(RandomEngine& Engine) = 0;
};

template <typename Ty = int, typename RandomEngine = std::mt19937>
class UniformIntDistribution : public Distribution<Ty> {
public:
    UniformIntDistribution() = default;
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
class UniformRealDistribution : public Distribution<Ty, RandomEngine> {
public:
    UniformRealDistribution() = default;
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
class NormalDistribution : public Distribution<Ty, RandomEngine> {
public:
    NormalDistribution() = default;
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
class LogNormalDistribution : public Distribution<Ty, RandomEngine> {
public:
    LogNormalDistribution() = default;
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
class BernoulliDistribution : public Distribution<double, RandomEngine> {
public:
    BernoulliDistribution() = default;
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
