// Copyright (c) 2011-2015 Zeex
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef AMXPROF_DURATION_H
#define AMXPROF_DURATION_H

#include "stdint.h"

namespace amxprof {

template<int64_t x, int64_t y>
struct Ratio {
  static const int64_t X = x;
  static const int64_t Y = y;
};

template<typename D1, typename D2>
D1 duration_cast(D2 d2) {
  // (d2 / d1) * count

  static const int64_t x1 = D1::RatioType::X;
  static const int64_t y1 = D1::RatioType::Y;
  static const int64_t x2 = D2::RatioType::X;
  static const int64_t y2 = D2::RatioType::Y;

  return static_cast<typename D1::ValueType>((d2.count() * (x2 * y1)) / (y2 * x1));
}

template<typename T, typename R>
class Duration {
 public:
  typedef T ValueType;
  typedef R RatioType;
  
  typedef Duration<ValueType, RatioType> ThisType;

  Duration() : count_(ValueType()) {}
  Duration(ValueType count) : count_(count) {}

  template<typename T2, typename R2>
  Duration(const Duration<T2, R2> &d) {
    count_ = duration_cast<ThisType>(d).count_;
  }

  ValueType count() const { return count_; }

  ThisType operator+() const {
    return ThisType(count_);
  }

  ThisType operator-() const {
    return ThisType(-count_);
  }

  template<typename D>
  ThisType operator+(const D &other) const {
    return ThisType(count_ + duration_cast<ThisType>(other).count_);
  }

  template<typename D>
  ThisType operator-(const D &other) const {
    return ThisType(count_ - duration_cast<ThisType>(other).count_);
  }

  template<typename D>
  ThisType &operator-=(const D &other) {
    count_ -= duration_cast<ThisType>(other).count_;
    return *this;
  }

  template<typename D>
  ThisType &operator+=(const D &other) {
    count_ += duration_cast<ThisType>(other).count_;
    return *this;
  }

  template<typename D>
  bool operator==(const D &other) const {
    return count_ == duration_cast<ThisType>(other).count_;
  }

  template<typename D>
  bool operator!=(const D &other) const {
    return count_ != duration_cast<ThisType>(other).count_;
  }

  template<typename D>
  bool operator<(const D &other) const {
    return count_ < duration_cast<ThisType>(other).count_;
  }

  template<typename D>
  bool operator>(const D &other) const {
    return count_ > duration_cast<ThisType>(other).count_;
  }

 private:
  ValueType count_;
};

typedef Duration<double, Ratio<1, 1000000000> > Nanoseconds;
typedef Duration<double, Ratio<1, 1000000> >    Microseconds;
typedef Duration<double, Ratio<1, 1000> >       Milliseconds;
typedef Duration<double, Ratio<1, 1> >          Seconds;
typedef Duration<double, Ratio<60, 1> >         Minutes;
typedef Duration<double, Ratio<3600, 1> >       Hours;
typedef Duration<double, Ratio<86400, 1> >      Days;
typedef Duration<double, Ratio<604800, 1> >     Weeks;

} // namespace amxprof

#endif // !AMXPROF_DURATION_H
