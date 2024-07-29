#pragma once

#include <concepts>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>

#include <fast-cpp-csv-parser/csv.h>

#include "Engine/Core/Assert.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN

template <typename... Args>
concept CsvConcept = (std::copyable<Args> && ...);

template <typename... Args>
requires CsvConcept<Args...>
class Csv {
public:
    Csv() = delete;
    Csv(const std::string& Filename, Args&&... ColNames) : _Filename(Filename) {
        AssignColNames(ColNames...);
        InitHeaderMap();
        ReadData(io::ignore_extra_column, std::forward<Args>(ColNames)...);
    }

    ~Csv() = default;

public:
    std::vector<std::string> GetDataArray(const std::string& DataHeader, const std::string& DataValue) const {
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

private:
    std::string                                  _Filename;
    std::vector<std::string>                     _ColNames;
    std::unordered_map<std::string, std::size_t> _HeaderMap;
    std::vector<std::vector<std::string>>        _Data;
};

_NPGS_END
