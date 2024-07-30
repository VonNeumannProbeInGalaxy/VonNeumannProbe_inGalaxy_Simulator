#pragma once

#include <algorithm>
#include <concepts>
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

template <typename... Args>
concept CsvConcept = (std::copyable<Args> && ...);

template <typename... Args>
requires CsvConcept<Args...>
class Csv {
public:
    using RowArray = std::vector<std::string>;

public:
    Csv() = default;
    Csv(const std::string& Filename, Args&&... ColNames) : _Filename(Filename) {
        AssignColNames(ColNames...);
        InitHeaderMap();
        ReadData(io::ignore_extra_column, std::forward<Args>(ColNames)...);
    }

    ~Csv() = default;

public:
    RowArray GetDataArray(const std::string& DataHeader, const std::string& DataValue) const {
        std::size_t DataIndex = GetHeaderIndex(DataHeader);
        for (const auto& Row : _Data) {
            if (Row[DataIndex] == DataValue) {
                return Row;
            }
        }

        throw std::out_of_range("Data not found.");
    }

    std::string GetSingleValue(const std::string& DataHeader, const std::string& DataValue, const std::string& TargetHeader) const {
        std::size_t DataIndex = GetHeaderIndex(DataHeader);
        std::size_t TargetIndex = GetHeaderIndex(TargetHeader);
        for (const auto& Row : _Data) {
            if (Row[DataIndex] == DataValue) {
                return Row[TargetIndex];
            }
        }

        throw std::out_of_range("Data not found.");
    }

    template <typename Func = decltype(&Csv::StrLessThan)>
    std::pair<RowArray, RowArray> FindSurroundingValues(const std::string& DataHeader, const std::string& TargetValue, bool bSorted = true, Func Pred = &Csv::StrLessThan) const {
        std::size_t DataIndex = GetHeaderIndex(DataHeader);
        std::vector<std::pair<std::string, RowArray>> ColData;

        for (const auto& Row : _Data) {
            ColData.emplace_back(Row[DataIndex], Row);
        }

        if (!bSorted) {
            std::sort(ColData.begin(), ColData.end(),
                [&](const auto& Lhs, const auto& Rhs) -> bool {
                    return Pred(Lhs.first, Rhs.first);
                }
            );
        }

        auto it = std::lower_bound(ColData.begin(), ColData.end(), TargetValue,
            [&](const auto& Lhs, const std::string& Rhs) -> bool {
                return Pred(Lhs.first, Rhs);
            }
        );

        if (it == ColData.end()) {
            throw std::out_of_range("Target value is out of range of the data.");
        }

        RowArray LowerRow = it == ColData.begin() ? it->second : (it - 1)->second;
        RowArray UpperRow = it->second;
        return { LowerRow, UpperRow };
    }

private:
    template <typename ColType, typename... Args>
    void AssignColNames(const ColType& First, const Args&... RestCols) {
        _ColNames.push_back(First);
        if constexpr (sizeof...(Args) > 0) {
            AssignColNames(RestCols...);
        }
    }

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

    template <typename... Args>
    void ReadData(io::ignore_column IgnoreColumn, Args&&... ColNames) {
        io::CSVReader<sizeof...(Args)> Reader(_Filename);
        Reader.read_header(IgnoreColumn, std::forward<Args>(ColNames)...);
        std::tuple<Args...> Row;
        while (ReadRow(Reader, Row)) {
            std::vector<std::string> RowData = TupleToVector(Row);
            _Data.push_back(RowData);
        }
    }

    template <typename ReaderType, typename TupleType>
    bool ReadRow(ReaderType& Reader, TupleType& Row) {
        return ReadRowImpl(Reader, Row, std::make_index_sequence<std::tuple_size<TupleType>::value>{});
    }

    template <typename ReaderType, typename TupleType, std::size_t... Indices>
    bool ReadRowImpl(ReaderType& Reader, TupleType& Row, std::index_sequence<Indices...>) {
        return Reader.read_row(std::get<Indices>(Row)...);
    }

    template <typename TupleType>
    std::vector<std::string> TupleToVector(const TupleType& Row) {
        return TupleToVectorImpl(Row, std::make_index_sequence<std::tuple_size<TupleType>::value>{});
    }

    template <typename TupleType, std::size_t... Indices>
    std::vector<std::string> TupleToVectorImpl(const TupleType& Row, std::index_sequence<Indices...>) {
        return { ToString(std::get<Indices>(Row))... };
    }

    template <typename ValueType>
    std::string ToString(const ValueType& Value) {
        std::stringstream Stream;
        Stream << Value;
        return Stream.str();
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
    std::vector<std::vector<std::string>>        _Data;
};

_ASSETS_END
_NPGS_END
