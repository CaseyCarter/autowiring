// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "JunctionBoxManager.h"
#include "AutoPacketFactory.h"
#include "AutowiringEvents.h"
#include "JunctionBox.h"
#include "EventRegistry.h"

template class RegEvent<AutowiringEvents>;
template class TypeUnifierComplex<AutowiringEvents>;

JunctionBoxManager::JunctionBoxManager(void) {
  // Enumerate all event types to initialize a new JunctionBox for each
  for (auto p = g_pFirstEventEntry; p; p = p->pFlink)
    m_junctionBoxes[p->ti] = p->NewJunctionBox();

  // Make sure AutowiringEvents is in EventRegistry
  assert(m_junctionBoxes.find(typeid(AutowiringEvents)) != m_junctionBoxes.end()
         && "AutowiringEvents wasn't added to the event registry");
  
  // Always allow internal events
  m_junctionBoxes[typeid(AutowiringEvents)]->Initiate();
}

JunctionBoxManager::~JunctionBoxManager(void) {}

std::shared_ptr<JunctionBoxBase> JunctionBoxManager::Get(const std::type_index& pTypeIndex) {
  auto box = m_junctionBoxes.find(pTypeIndex);
  assert(box != m_junctionBoxes.end() && "If JunctionBox isn't found, EventRegistry isn't working");
  return box->second;
}

void JunctionBoxManager::Initiate(void) {
  for (const auto& q : m_junctionBoxes)
    q.second->Initiate();
}

void JunctionBoxManager::AddEventReceiver(JunctionBoxEntry<Object> receiver) {
  // Notify all junctionboxes that there is a new event
  for(const auto& q: m_junctionBoxes)
    q.second->Add(receiver);
}

void JunctionBoxManager::RemoveEventReceiver(JunctionBoxEntry<Object> receiver) {
  // Notify all compatible senders that we're going away:
  for(const auto& q : m_junctionBoxes)
    q.second->Remove(receiver);
}
