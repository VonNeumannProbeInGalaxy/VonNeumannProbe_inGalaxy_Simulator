module;

#include "Engine/Core/Base.h"

export module Base.NpgsObject;

_NPGS_BEGIN

export class NpgsObject {
public:
    NpgsObject() = default;
    virtual ~NpgsObject() = default;
};

_NPGS_END
