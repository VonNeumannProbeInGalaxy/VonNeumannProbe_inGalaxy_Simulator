#pragma once

#include <algorithm>
#include <concepts>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include <fast-cpp-csv-parser/csv.h>

#include "Engine/Core/Assert.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN
_ASSETS_BEGIN

template <std::size_t Size>
concept CsvConcept = Size > 0;

template <typename BasicType, std::size_t Size>
requires CsvConcept<Size>
class Csv {
public:
    using RowArray = std::vector<BasicType>;

public:
    Csv() = default;
    Csv(const std::string& Filename, std::vector<std::string>& ColNames) : _Filename(Filename), _ColNames(ColNames) {
        InitHeaderMap();
        ReadData(io::ignore_extra_column);
    }

    ~Csv() = default;

public:
    RowArray FindFirstDataArray(const std::string& DataHeader, const BasicType& DataValue) const {
        std::size_t DataIndex = GetHeaderIndex(DataHeader);
        for (const auto& Row : _Data) {
            if (Row[DataIndex] == DataValue) {
                return Row;
            }
        }

        throw std::out_of_range("Data not found.");
    }

    BasicType FindSingleValue(const std::string& DataHeader, const BasicType& DataValue, const std::string& TargetHeader) const {
        std::size_t DataIndex   = GetHeaderIndex(DataHeader);
        std::size_t TargetIndex = GetHeaderIndex(TargetHeader);
        for (const auto& Row : _Data) {
            if (Row[DataIndex] == DataValue) {
                return Row[TargetIndex];
            }
        }

        throw std::out_of_range("Data not found.");
    }

    template <typename Func = std::less<>>
    std::pair<RowArray, RowArray> FindSurroundingValues(const std::string& DataHeader, const BasicType& TargetValue, bool bSorted = true, Func Pred = Func()) const {
        std::size_t DataIndex = GetHeaderIndex(DataHeader);
        std::vector<std::pair<BasicType, RowArray>> ColData;

        std::function<bool(const BasicType&, const BasicType&)> Comparator = Pred;
        if constexpr (std::is_same_v<Func, std::less<>> && std::is_same_v<BasicType, std::string>) {
            Comparator = &Csv::StrLessThan;
        }

        for (const auto& Row : _Data) {
            ColData.emplace_back(Row[DataIndex], Row);
        }

        if (!bSorted) {
            std::sort(ColData.begin(), ColData.end(), [&](const auto& Lhs, const auto& Rhs) -> bool {
                return Comparator(Lhs.first, Rhs.first);
            });
        }

        auto it = std::lower_bound(ColData.begin(), ColData.end(), TargetValue, [&](const auto& Lhs, const BasicType& Rhs) -> bool {
            return Comparator(Lhs.first, Rhs);
        });

        if (it == ColData.end()) {
            throw std::out_of_range("Target value is out of range of the data.");
        }

        RowArray LowerRow;
        RowArray UpperRow;

        if (it->first == TargetValue) {
            LowerRow = UpperRow = it->second;
        } else {
            LowerRow = it == ColData.begin() ? it->second : (it - 1)->second;
            UpperRow = it->second;
        }
        return { LowerRow, UpperRow };
    }

    const std::vector<RowArray>* const Data() const {
        return &_Data;
    }

private:
    void InitHeaderMap() {
        for (std::size_t i = 0; i < _ColNames.size(); ++i) {
            _HeaderMap[_ColNames[i]] = i;
        }
    }

    std::size_t GetHeaderIndex(const std::string& Header) const {
        auto it = _HeaderMap.find(Header);
        if (it != _HeaderMap.end()) {
            return it->second;
        }

        throw std::out_of_range("Header not found.");
    }

    template <typename ReaderType>
    void ReadHeader(ReaderType& Reader, io::ignore_column IgnoreColumn) {
        std::apply([&](auto&&... Args) -> void {
            Reader.read_header(IgnoreColumn, std::forward<decltype(Args)>(Args)...);
        }, VectorToTuple(_ColNames));
    }

    void ReadData(io::ignore_column IgnoreColumn) {
        io::CSVReader<Size> Reader(_Filename);
        ReadHeader(Reader, IgnoreColumn);
        std::vector<BasicType> Row(_ColNames.size());
        while (ReadRow(Reader, Row)) {
            _Data.emplace_back(Row);
        }
    }

    template <typename ReaderType>
    bool ReadRow(ReaderType& Reader, std::vector<BasicType>& Row) {
        return ReadRowImpl(Reader, Row, std::make_index_sequence<Size>{});
    }

    template <typename ReaderType, std::size_t... Indices>
    bool ReadRowImpl(ReaderType& Reader, std::vector<BasicType>& Row, std::index_sequence<Indices...>) {
        return Reader.read_row(Row[Indices]...);
    }

    template <typename ElemType>
    auto VectorToTuple(const std::vector<ElemType>& Vector) {
        return VectorToTupleImpl(Vector, std::make_index_sequence<Size>{});
    }

    template <typename ElemType, std::size_t... Indices>
    auto VectorToTupleImpl(const std::vector<ElemType>& Vector, std::index_sequence<Indices...>) {
        return std::make_tuple(Vector[Indices]...);
    }

    static bool StrLessThan(const std::string& Str1, const std::string& Str2) {
        double StrValue1 = std::stod(Str1);
        double StrValue2 = std::stod(Str2);
        return StrValue1 < StrValue2;
    }

private:
    std::string                                  _Filename;
    std::vector<std::string>                     _ColNames;
    std::unordered_map<std::string, std::size_t> _HeaderMap;
    std::vector<RowArray>                        _Data;
};

_ASSETS_END
_NPGS_END