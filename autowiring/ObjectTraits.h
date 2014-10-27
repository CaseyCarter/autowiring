// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include <typeinfo>
#include MEMORY_HEADER

#include "fast_pointer_cast.h"
#include "AnySharedPointer.h"
#include "AutoFilterDescriptor.h"

#include "AutowiringEvents.h"
#include "Object.h"
#include "ContextMember.h"
#include "CoreRunnable.h"
#include "BasicThread.h"
#include "ExceptionFilter.h"
#include "BoltBase.h"

#include "EventRegistry.h"

/// <summary>
/// Mapping and extraction structure used to provide a runtime version of an Object-implementing shared pointer
/// </summary>
struct ObjectTraits {
  template<class T>
  ObjectTraits(const std::shared_ptr<typename SelectTypeUnifier<T>::type>& value, T*) :
    type(typeid(T)),
    stump(SlotInformationStump<T>::s_stump),
    value(value),
    cast_offset(this->value->cast_offset()),
    subscriber(MakeAutoFilterDescriptor(value)),
    pObject(autowiring::fast_pointer_cast<Object>(value)),
    pContextMember(autowiring::fast_pointer_cast<ContextMember>(value)),
    pCoreRunnable(autowiring::fast_pointer_cast<CoreRunnable>(value)),
    pBasicThread(autowiring::fast_pointer_cast<BasicThread>(value)),
    pFilter(autowiring::fast_pointer_cast<ExceptionFilter>(value)),
    pBoltBase(autowiring::fast_pointer_cast<BoltBase>(value)),
    receivesEvents(
      [this]{
        for (auto evt = g_pFirstEventEntry; evt; evt = evt->pFlink) {
          auto identifier = evt->NewTypeIdentifier();
          if (identifier->IsSameAs(pObject.get()))
            return true;
        }
        // HACK: Manually check if type implements AutowiringEvents
        return !!dynamic_cast<const AutowiringEvents*>(pObject.get());
      }()
    )
  {
    if(!pObject)
      throw autowiring_error("Cannot add a type which does not implement Object");
  }

  // The declared original type:
  const std::type_info& type;

  /// <summary>
  /// Used to obtain a list of slots defined on this type, for reflection purposes
  /// </summary>
  /// <returns>
  /// A pointer to the head of a linked list of slots on this context member
  /// </returns>
  /// <remarks>
  /// A slot is an Autowired field defined within a specific type.  Slots are of particular
  /// interest because they denote a compile-time relationship between two types, and generally
  /// are one way to understand class relationships in a system.  Furthermore, because of their
  /// compile-time nature, they are declarative and therefore denote a relationship between
  /// types, rather than states, which makes it easier to understand how slots are linked.
  ///
  /// The returned value is cached, and should not be freed or modified as it may be in use
  /// in other parts of the program.  The behavior of this method is undefined if it's called
  /// on an object before the object is fully constructed (for instance, if the method is
  /// invoked from a constructor).  This method will return correct results even if the
  /// ContextMember type was not the first inherited type.
  ///
  /// If this method returns a correct result at any point, then all subsequent calls to this
  /// method are guaranteed to return correct results, even in the aforementioned case where
  /// the method is called during construction.  This method is guaranteed to return correct
  /// results after the first instance of a concrete type is constructed.
  ///
  /// This list is guaranteed not to contain any AutowiredFast fields defined in the class.
  ///
  /// The linked list is guaranteed to be in reverse-sorted order
  /// </remarks>
  const SlotInformationStumpBase& stump;

  // A holder to store the original shared pointer, to ensure that type information propagates
  // correctly on the right-hand side of our map
  const AnySharedPointer value;

  // Offset of the void pointer to the embedded Object
  const size_t cast_offset;

  // The packet subscriber introduction method, if appropriate:
  const AutoFilterDescriptor subscriber;

  // There are a lot of interfaces we support, here they all are:
  const std::shared_ptr<Object> pObject;
  const std::shared_ptr<ContextMember> pContextMember;
  const std::shared_ptr<CoreRunnable> pCoreRunnable;
  const std::shared_ptr<BasicThread> pBasicThread;
  const std::shared_ptr<ExceptionFilter> pFilter;
  const std::shared_ptr<BoltBase> pBoltBase;

  // Does this type receive events?
  const bool receivesEvents;
};