// Copyright(c) 2015 kittikun
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sub license, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#pragma once

#include <unordered_map>
#include <boost/thread.hpp>

namespace Takoyaki
{
    // Not thread-safe by itself, just provide a convenience RW mutex
    template<   class Key,                                              // unordered_map::key_type
                class T,                                                // unordered_map::mapped_type
                class Hash = std::hash<Key>,                            // unordered_map::hasher
                class Pred = std::equal_to<Key>,                        // unordered_map::key_equal
                class Alloc = std::allocator<std::pair<const Key, T>>>  // unordered_map::allocator_type>
    class RWLockMap final : public std::unordered_map<Key, T, Hash, Pred, Alloc>
    {
    public:
        std::unique_lock<boost::shared_mutex> getWriteLock() { return std::unique_lock<boost::shared_mutex>{rwMutex_}; }
        boost::shared_lock<boost::shared_mutex> getReadLock() { return boost::shared_lock<boost::shared_mutex>{rwMutex_}; }

    private:
        mutable boost::shared_mutex rwMutex_;
    };
} // namespace Takoyaki