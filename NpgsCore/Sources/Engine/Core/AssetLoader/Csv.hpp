#pragma once

#include <algorithm>
#include <concepts>
#include <functional>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include <fast-cpp-csv-parser/csv.h>

#include "Engine/Core/Base.h"

_NPGS_BEGIN
_ASSET_BEGIN

template <std::size_t Size>
concept CsvConcept = Size > 0;

template <typename BasicType, std::size_t Size>
requires CsvConcept<Size>
class Csv {
public:
    using RowArray = std::vector<BasicType>;

public:
    Csv() = default;
    Csv(const std::string& Filename, const std::vector<std::string>& ColNames)
        : _Filename(Filename), _ColNames(ColNames)
    {
        InitHeaderMap();
        ReadData(io::ignore_extra_column);
    }

    ~Csv() = default;

    RowArray FindFirstDataArray(const std::string& DataHeader, const BasicType& DataValue) const {
        std::size_t DataIndex = GetHeaderIndex(DataHeader);
        for (const auto& Row : _Data) {
            if (Row[DataIndex] == DataValue) {
                return Row;
            }
        }

        throw std::out_of_range("Data not found.");
    }

    BasicType FindMatchingValue(const std::string& DataHeader, const BasicType& DataValue, const std::string& TargetHeader) const {
        std::size_t DataIndex = GetHeaderIndex(DataHeader);
        std::size_t TargetIndex = GetHeaderIndex(TargetHeader);
        for (const auto& Row : _Data) {
            if (Row[DataIndex] == DataValue) {
                return Row[TargetIndex];
            }
        }

        throw std::out_of_range("Data not found.");
    }

    template <typename Func = std::less<>>
    std::pair<RowArray, RowArray> FindSurroundingValues(const std::string& DataHeader, const BasicType& TargetValue, bool bSorted = true, Func&& Pred = Func()) {
        std::size_t DataIndex = GetHeaderIndex(DataHeader);

        std::function<bool(const BasicType&, const BasicType&)> Comparator = Pred;
        if constexpr (std::is_same_v<Func, std::less<>> && std::is_same_v<BasicType, std::string>) {
            Comparator = &Csv::StrLessThan;
        }

        if (!bSorted) {
            std::sort(_Data.begin(), _Data.end(), [&](const RowArray& Lhs, const RowArray& Rhs) -> bool {
                return Comparator(Lhs[DataIndex], Rhs[DataIndex]);
            });
        }

        auto it = std::lower_bound(_Data.begin(), _Data.end(), TargetValue, [&](const RowArray& Row, const BasicType& Value) -> bool {
            return Comparator(Row[DataIndex], Value);
        });

        if (it == _Data.end()) {
            throw std::out_of_range("Target value is out of range of the data.");
        }

        typename std::vector<RowArray>::iterator LowerRow;
        typename std::vector<RowArray>::iterator UpperRow;

        if ((*it)[DataIndex] == TargetValue) {
            LowerRow = UpperRow = it;
        } else {
            LowerRow = it == _Data.begin() ? it : it - 1;
            UpperRow = it;
        }

        return { *LowerRow, *UpperRow };
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

_ASSET_END
_NPGS_END
