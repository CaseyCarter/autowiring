// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "TypeRegistry.h"
#include "JunctionBoxManager.h"
#include "AutoPacketFactory.h"
#include "AutoPacketListener.h"

JunctionBoxManager::JunctionBoxManager(void) {
  // Enumerate all Autofired types to initialize a new JunctionBox for each
  for(auto p = g_pFirstEntry; p; p = p->pFlink)
    m_junctionBoxes[p->ti] = p->m_NewJunctionBox();
}

JunctionBoxManager::~JunctionBoxManager(void) {}

std::shared_ptr<JunctionBoxBase> JunctionBoxManager::Get(std::type_index pTypeIndex) {
  auto box = m_junctionBoxes.find(pTypeIndex);
  assert(box != m_junctionBoxes.end());
  return box->second;
}

void JunctionBoxManager::AddEventReceiver(std::shared_ptr<EventReceiver> pRecvr){
  
  //Notify all junctionboxes that there is a new event
  for(auto q = m_junctionBoxes.begin(); q != m_junctionBoxes.end(); q++)
    *(q->second) += pRecvr;
}

void JunctionBoxManager::RemoveEventReceiver(std::shared_ptr<EventReceiver> pRecvr){
  
  // Notify all compatible senders that we're going away:
  for(auto q = m_junctionBoxes.begin(); q != m_junctionBoxes.end(); q++)
    *(q->second) -= pRecvr;
}

void JunctionBoxManager::RemoveEventReceivers(t_rcvrSet::iterator first, t_rcvrSet::iterator last){
  
  for(auto r = m_junctionBoxes.begin(); r != m_junctionBoxes.end(); r++) {
    auto box = r->second;
    for(auto q = first; q != last; q++) {
      *box -= *q;
    }
  }
}