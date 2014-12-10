// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "Decompose.h"
#include <iostream>

template<class T>
struct has_validate_function {
};

/// <summary>
/// Detects whether the specified type T has a static method with the name Validate
/// </summary>
template<class T>
struct has_validate {
  template<class Fn, Fn>
  struct unnamed_constant;

  template<class U>
  static std::true_type select(decltype(&U::Validate));

  template<class U>
  static std::false_type select(...);

  // Conveninece typedef used externally:
  typedef decltype(select<T>(nullptr)) has_valid;

  // Evaluates to true only if T includes a unique AutoFilter method with at least one argument.
  static const bool value = has_valid::value;
};

template<class T, class Validator, bool validatable = has_validate<Validator>::value>
struct CallValidate {
  static bool Call(const T&) { return true; }
};

template<class T, class Validator>
struct CallValidate<T, Validator, true> {
  static bool Call(const T& obj) {
    return Validator::Validate(obj); 
  }
};
