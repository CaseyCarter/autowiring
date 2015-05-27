// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AnySharedPointer.h"

struct CoreObjectDescriptor;
class DeferrableAutowiring;

/// \internal
/// <summary>
/// Represents a single entry, together with any deferred elements waiting on the satisfaction of this entry
/// </summary>
struct MemoEntry {
  MemoEntry(void);

  // The first deferrable autowiring which requires this type, if one exists:
  DeferrableAutowiring* pFirst = nullptr;

  // A back reference to the concrete type from which this memo was generated.  This field may be null
  // if there is no corresponding concrete type.
  const CoreObjectDescriptor* pObjTraits;

  // Once this memo entry is satisfied, this will contain the AnySharedPointer instance that performs
  // the satisfaction
  AnySharedPointer m_value;

  // A flag to determine if this memo entry was set from the current context.
  bool m_local = true;
};

