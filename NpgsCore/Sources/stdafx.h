// C++ includes used for precompiling -*- C++ -*-

// Copyright (C) 2003-2024 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file stdafx.h
 *  This is an modified implementation for Visual Studio.
 */

// 17.4.1.2 Headers

// C

#pragma once

#include <cassert>
#include <cctype>
#include <cerrno>
#include <cfloat>
#include <ciso646>
#include <climits>
#include <clocale>
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cwchar>
#include <cwctype>

#if _MSVC_LANG >= 201103L
#include <ccomplex>
#include <cfenv>
#include <cinttypes>
#include <cstdalign>
#include <cstdbool>
#include <cstdint>
#include <ctgmath>
#include <cuchar>
#endif

// C++
#include <algorithm>
#include <bitset>
#include <complex>
#include <deque>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <iterator>
#include <limits>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <new>
#include <numeric>
#include <ostream>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <typeinfo>
#include <utility>
#include <valarray>
#include <vector>

#if _MSVC_LANG >= 201103L
#include <array>
#include <atomic>
#include <chrono>
#include <codecvt>
#include <condition_variable>
#include <forward_list>
#include <future>
#include <initializer_list>
#include <mutex>
#include <random>
#include <ratio>
#include <regex>
#include <scoped_allocator>
#include <system_error>
#include <thread>
#include <tuple>
#include <typeindex>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#endif

#if _MSVC_LANG >= 201402L
#include <shared_mutex>
#endif

#if _MSVC_LANG >= 201703L
#include <any>
#include <charconv>
#include <execution>
#include <filesystem>
#include <memory_resource>
#include <optional>
#include <string_view>
#include <variant>
#endif

#if _MSVC_LANG >= 202002L
#include <barrier>
#include <bit>
#include <compare>
#include <concepts>
#include <format>
#include <latch>
#include <numbers>
#include <ranges>
#include <semaphore>
#include <source_location>
#include <span>
#include <stop_token>
#include <syncstream>
#include <version>
#endif

#if _MSVC_LANG > 202002L
#if __cpp_impl_coroutine
# include <coroutine>
#endif
#include <expected>
#if __has_include(<generator>)
# include <generator>
#endif
#include <print>
#include <spanstream>
#if __has_include(<stacktrace>)
# include <stacktrace>
#endif
#include <stdatomic.h>
#include <stdfloat>
#endif

#if _MSVC_LANG > 202302L
#if __has_include(<text_encoding>)
# include <text_encoding>
#endif
#endif

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <fast-cpp-csv-parser/csv.h>
#include <glad/glad.h>

#ifdef VULKAN
#define GLFW_INCLUDE_VULKAN
#define VK_USE_PLATFORM_WIN32_KHR
#endif // VULKAN
#include <GLFW/glfw3.h>

#ifdef VULKAN
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif // VULKAN
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

#ifndef _RELEASE
#define NPGS_ENABLE_CONSOLE_LOGGER
#endif // _RELEASE
#include "Engine/Utils/Logger.h"
