/*  This file is part of the Pinocchio automatic rigging library.
	Copyright (C) 2007 Ilya Baran (ibaran@mit.edu)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#pragma once
#ifndef HASHUTILS_H
#define HASHUTILS_H

#include "math/mathutils.h"

#ifndef _WIN32
// #include <unordered_map>
#include <ext/hash_map>
// #include <unordered_set>
#include <ext/hash_set>

#define _HASH_NAMESPACE __gnu_cxx

using namespace _HASH_NAMESPACE;

namespace _HASH_NAMESPACE
{
	template <class T1, class T2>
	struct hash<pair<T1, T2>>
	{
		size_t operator()(const pair<T1, T2> &p) const { return hash<T1>()(p.first) + 37 * hash<T2>()(p.second); }
	};

	template <class T>
	struct hash<T *>
	{
		size_t operator()(T *p) const { return (size_t)p; }
	};
}

#define MAKE_HASH(type, code)                               \
	namespace _HASH_NAMESPACE                               \
	{                                                       \
		template <>                                         \
		struct hash<type>                                   \
		{                                                   \
			size_t operator()(const type &p) const { code } \
		};                                                  \
	}
#else // MICROSOFT VC 2022
#include <unordered_map>
#include <unordered_set>

// NOTE
//#define _HASH_NAMESPACE stdext
//using namespace _HASH_NAMESPACE;

namespace _HASH_NAMESPACE
{
	template <class T>
	struct hash
	{
		size_t operator()(const T& p) { return hash_compare<T>()(p); }
	};

	// TODO: add to compile
	template <class T>
	struct hash_compare
	{
		bool operator()(const T& a, const T& b) const { return a < b; }
	};

	template <class T1, class T2>
	struct hash_compare<std::pair<T1, T2>>
	{
		static const size_t bucket_size = 4;
		static const size_t min_buckets = 8;
		size_t operator()(const std::pair<T1, T2> &p) const { return hash_compare<T1>()(p.first) + 37 * hash_compare<T2>()(p.second); }
		bool operator()(const std::pair<T1, T2> &p1, const std::pair<T1, T2> &p2) const { return p1 < p2; }
	};

	template <class T>
	struct hash_compare<T *>
	{
		static const size_t bucket_size = 4;
		static const size_t min_buckets = 8;
		size_t operator()(T *p) const { return (size_t)p; }
		bool operator()(T *p1, T *p2) const { return p1 < p2; }
	};
}

#define MAKE_HASH(type, code)                                                         \
	namespace _HASH_NAMESPACE                                                         \
	{                                                                                 \
		template <>                                                                   \
		struct hash_compare<type>                                                     \
		{                                                                             \
			static const size_t bucket_size = 4;                                      \
			static const size_t min_buckets = 8;                                      \
			size_t operator()(const type &p) const { code }                           \
			bool operator()(const type &p1, const type &p2) const { return p1 < p2; } \
		};                                                                            \
	}
#endif

#endif // HASHUTILS_H
