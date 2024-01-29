// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once

#include <ctime>
#include <vector>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <functional>
#include <sstream>
#include "Logger.h"
#include <cassert>
/*
  
*/

namespace Engine
{
  using namespace Logger;

  /* Messages are encoded as strings, parameters are seperated by newlines */
  struct Packet
  {
  public:
    virtual ~Packet() = 0 {}

    template<typename T>
    void setData(const T & data);

    template<typename T>
    const T & getData() const;

    virtual std::string type() const = 0;

    template<typename TYPE>
    bool isType() const
    {
      return typeid(TYPE).name() == type();
    }
  };

  template<typename T>
  struct Message : public Packet
  {
  public:
    Message() {}
    Message(const T & payload) : data(payload) {}

    Message & operator=(const T & rhs)
    {
      data = rhs;
      return *this;
    }
    
    std::string type() const override
    {
      return typeid(T).name();
    }

    T data;
  };


  // Dynamic casted because type safety is fairly imporntant here and you can't always
  // be sure you're getting the right data type
  template<typename T>
  void Packet::setData(const T & data)
  {
    // Type checking in debug mode
#ifdef _DEBUG
    if (!isType<T>())
    {
      Log<Error>("Requested data is not of type %s", type().c_str());
      assert(isType<T>());
    }
#endif // _DEBUG

    static_cast<Message<T> &>(*this).data = data;
  }

  template<typename T>
  const T & Packet::getData() const
  {
    // Type checking in debug mode
#ifdef _DEBUG
    if (!isType<T>())
    {
      Log<Error>("Requested data is not of type %s", type().c_str());
      assert(isType<T>());
    }
#endif // _DEBUG

    return static_cast<const Message<T> &>(*this).data;
  }

  typedef std::function<void(const Packet &)> SUBSCRIBER_ACTION;
  typedef std::function<void(Packet &)> REQUEST_ACTION;

  class Messenger
  {
  public:

    unsigned long Subscribe( Messenger & sub, 
                    const std::string & subType, 
                    SUBSCRIBER_ACTION function) const;

    virtual void Unsubscribe( Messenger & sub,
                              const std::string & subType, unsigned long sid = 0);

    virtual void SetupRequest(const std::string & reqType, 
                              REQUEST_ACTION function);

    virtual void DisconnectRequest(const std::string & reqType);

    virtual void Clear();

    template<typename T>
    T Request(const std::string & type)
    {
      try
      {
        Message<T> req;

        requestList_.at(type)(req);

        return req.data;
      }
      catch (const std::bad_cast&)
      {
        Log<Error>("Requested data is of a different type than requested! Type: %s", type.c_str());
        throw std::runtime_error("Requested data is of a different type than requested!");
      }
      catch (const std::out_of_range&)
      {
        Log<Error>("Requested messenger does not have a request for the desired type! Type: %s", type.c_str());
        throw std::runtime_error("Requested messenger does not have a request for the desired type!");
      }
    }

    template<typename T>
    void Post(const std::string & eventType, const Message<T> & payload)
    {
      PostMsg(eventType, payload);
    }

    template<typename T>
    void Post(const std::string & eventType, const T & payload)
    {
      PostMsg(eventType, Message<T>(payload));
    }

    static unsigned long getSID();

  protected:
    void AddRequest(const std::string & reqType, REQUEST_ACTION function);
    void RemoveRequest(const std::string & reqType);

    void PostMsg(const std::string & eventType, const Packet & payload);

     unsigned long AddSub(const Messenger * subscriber, 
                const std::string & subType,
                SUBSCRIBER_ACTION function); 

    void RemoveSub( Messenger * subscriber,
                    const std::string & subType, 
                    unsigned long sid);
  private:
    std::unordered_map< std::string, 
                        std::unordered_map< const Messenger *, 
                                           std::vector<std::pair<unsigned long, SUBSCRIBER_ACTION>> > >
      subscriberList_;
    std::unordered_map<std::string, REQUEST_ACTION> requestList_;
  };

}

//#include "../source/Messages_Encoder.cpp"
