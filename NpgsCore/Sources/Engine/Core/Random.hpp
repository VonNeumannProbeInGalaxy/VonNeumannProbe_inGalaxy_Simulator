#pragma once

#include <random>
#include "Engine/Core/Base.h"

_NPGS_BEGIN

class NPGS_API Distribution {
public:
    virtual ~Distribution() = default;
    virtual double Generate(std::mt19937& Engine) = 0;
};

class NPGS_API UniformRealDistribution : public Distribution {
public:
    UniformRealDistribution(double Min, double Max) : _Distribution(Min, Max) {}

    double Generate(std::mt19937& Engine) override {
        return _Distribution(Engine);
    }

private:
    std::uniform_real_distribution<double> _Distribution;
};

class NPGS_API NormalDistribution : public Distribution {
public:
    NormalDistribution(double Mean, double Sigma) : _Distribution(Mean, Sigma) {}

    double Generate(std::mt19937& Engine) override {
        return _Distribution(Engine);
    }

private:
    std::normal_distribution<double> _Distribution;
};

class NPGS_API LogNormalDistribution : public Distribution {
public:
    LogNormalDistribution(double Mean, double Sigma) : _Distribution(Mean, Sigma) {}

    double Generate(std::mt19937& Engine) override {
        return _Distribution(Engine);
    }

private:
    std::lognormal_distribution<double> _Distribution;
};

class NPGS_API BernoulliDistribution : public Distribution {
public:
    BernoulliDistribution(double Probability) : _Distribution(Probability) {}

    double Generate(std::mt19937& Engine) override {
        return _Distribution(Engine);
    }

private:
    std::bernoulli_distribution _Distribution;
};

_NPGS_END
