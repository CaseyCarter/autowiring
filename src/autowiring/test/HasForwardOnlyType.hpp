// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include <autowiring/autowiring.h>

class MyForwardedType;

class HasForwardOnlyType {
public:
  HasForwardOnlyType(void);

  Autowired<MyForwardedType> forwarded;
};