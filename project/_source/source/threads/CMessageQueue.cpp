/*
(w) 2016 - 2021 by Christian Keimel / https://www.dustbin-online.de
This file is licensed under the terms of the ZLib license - https://www.zlib.net/zlib_license.html
*/
#include <threads/CMessageQueue.h>

namespace dustbin {
  namespace threads {

    CInputQueue::CInputQueue() {
    }

    CInputQueue::~CInputQueue() {
      for (std::vector<COutputQueue *>::iterator it = m_cSenders.begin(); it != m_cSenders.end(); it++) {
        (*it)->removeListener(this);
      }

      for (std::vector<dustbin::messages::IMessage *>::iterator it = m_cMessages.begin(); it != m_cMessages.end(); it++) {
        delete *it;
      }
    }

    void CInputQueue::postMessage(dustbin::messages::IMessage *a_pMsg) {
      if (a_pMsg == nullptr) return;

      dustbin::messages::IMessage *l_pClone = a_pMsg->clone();

      std::lock_guard<std::mutex> l_Guard(m_cMutex);
      m_cMessages.push_back(l_pClone);
      m_cCv.notify_one();
    }

    dustbin::messages::IMessage *CInputQueue::popMessage() {
      std::lock_guard<std::mutex> l_Guard(m_cMutex);
      if (m_cMessages.size() == 0)
        return NULL;
      else {
        dustbin::messages::IMessage *l_Result = m_cMessages.at(0);
        m_cMessages.erase(m_cMessages.begin());
        return l_Result;
      }
    }

    dustbin::messages::IMessage *CInputQueue::waitMesssage() {
      if (m_cMessages.size() == 0) {
        std::unique_lock<std::mutex> l_Lock(m_cMutex);
        m_cCv.wait(l_Lock);
        l_Lock.unlock();
      }

      return popMessage();
    }

    void CInputQueue::addSender(COutputQueue *a_pSender) {
      for (std::vector<COutputQueue *>::iterator it = m_cSenders.begin(); it != m_cSenders.end(); it++) {
        if (*it == a_pSender) return;
      }

      m_cSenders.push_back(a_pSender);
    }

    void CInputQueue::removeSender(COutputQueue *a_pSender) {
      for (std::vector<COutputQueue*>::iterator it = m_cSenders.begin(); it != m_cSenders.end(); it++) {
        if (*it == a_pSender) {
          m_cSenders.erase(it);
          break;
        }
      }
    }

    int CInputQueue::getSenderCount() {
      return (int)m_cSenders.size();
    }

    void CInputQueue::notify() {
      m_cCv.notify_one();
    }

    COutputQueue::COutputQueue() {

    }

    COutputQueue::~COutputQueue() {
      std::lock_guard<std::mutex> l_cLock(m_cMutex);
      for (std::vector<CInputQueue *>::iterator it = m_cListeners.begin(); it != m_cListeners.end(); it++) {
        (*it)->removeSender(this);
      }
    }

    void COutputQueue::postMessage(dustbin::messages::IMessage *a_pMsg) {
      std::lock_guard<std::mutex> l_cLock(m_cMutex);

      for (std::vector<CInputQueue *>::iterator it = m_cListeners.begin(); it != m_cListeners.end(); it++) {
        (*it)->postMessage(a_pMsg);
      }
    }

    void COutputQueue::addListener(CInputQueue *a_pListener) {
      std::lock_guard<std::mutex> l_cLock(m_cMutex);

      for (std::vector<CInputQueue *>::iterator it = m_cListeners.begin(); it != m_cListeners.end(); it++) {
        if (*it == a_pListener) return;
      }

      m_cListeners.push_back(a_pListener);
      a_pListener->addSender(this);
    }

    void COutputQueue::removeListener(CInputQueue *a_pListener) {
      std::lock_guard<std::mutex> l_cLock(m_cMutex);

      for (std::vector<CInputQueue *>::iterator it = m_cListeners.begin(); it != m_cListeners.end(); it++) {
        if (*it == a_pListener) {
          m_cListeners.erase(it);
          break;
        }
      }
    }

    int COutputQueue::getListenerCount() {
      return (int)m_cListeners.size();
    }

  }
}