// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "../include/Messages.h"
#include <algorithm>

namespace Engine
{
  /****************************************************************************/
  /*!
    \brief
      Gets the next subscriber ID for message subscriptions
      
  */
  /****************************************************************************/
  unsigned long Messenger::getSID()
  {
    static unsigned long sid = 0;
    return ++sid;
  }

  /****************************************************************************/
  /*!
    \brief
      Subscribes one messenger to another with the given subscription name
      and subscriber action

    \param sub
      Messenger to subscribe to

    \param subType
      Name of the event to listen for

    \param function
      Function to call when the event is fired

    \return
      Subscriber ID of the new event
  */
  /****************************************************************************/
  unsigned long Messenger::Subscribe(Messenger & sub,
                            const std::string & subType,
                            SUBSCRIBER_ACTION function) const
  {
    return sub.AddSub(this, subType, function);
  }

  /****************************************************************************/
  /*!
    \brief
      unsubscribes a messenger from an event

    \param sub
      Messenger to unsubscribe from

    \param subType
      Event to unsubscribe from

    \param sid
      ID of the event to unsubscribe from. If 0, unsubscribes all listeners
      from that messenger
  */
  /****************************************************************************/
  void Messenger::Unsubscribe(Messenger & sub,
                              const std::string & subType, unsigned long sid)
  {
    sub.RemoveSub(this, subType, sid);
  }

  /****************************************************************************/
  /*!
    \brief
      Clears all subscriptions and requests from a messenger
  */
  /****************************************************************************/
  void Messenger::Clear() 
  { 
    subscriberList_.clear(); 
    requestList_.clear(); 
  }

  /****************************************************************************/
  /*!
    \brief
      Adds a subscriber to a messenger with the given event name and function

    \param subscriber
      Pointer to the subscriber that is being set up

     \param subType
      Name of the event to listen for

    \param function
      Subscriber action fired when the event is retrieved

    \return
      Subscriber ID of the new event
  */
  /****************************************************************************/
  unsigned long Messenger::AddSub( const Messenger * subscriber,
                          const std::string & subType,
                          SUBSCRIBER_ACTION function)
  {
    unsigned long sid = getSID();
    subscriberList_[subType][subscriber].push_back(std::make_pair(sid, function));
    return sid;
  }

  /****************************************************************************/
  /*!
    \brief
      Removes a listener from an event

    \param subscriber
      Messenger to remove

    \param subType
      Event to remove from

    \param sid
      ID of the event to remove. If 0, removes all listeners
      from that messenger
  */
  /****************************************************************************/
  void Messenger::RemoveSub(Messenger * subscriber,
                            const std::string & subType, 
                            unsigned long sid)
  {
    // Remove all subscribers
    if(sid == 0)
      subscriberList_.at(subType).erase(subscriber);  
   
    // Remove specific subscriber
    else
    {
      std::vector<std::pair<unsigned long, SUBSCRIBER_ACTION>> list = subscriberList_.at(subType).at(subscriber);
      auto iter = std::find_if(list.begin(), list.end(),
        // Search lambda
        [&id = sid](const std::pair<unsigned long, SUBSCRIBER_ACTION> & element)
      { 
        return element.first == id; 
      });

      if (iter == list.end())
        throw(std::out_of_range("Attempt to erase invalid subscriber"));

      // Remove list if no more actions exsit on it
      list.erase(iter);

      if (list.empty())
        subscriberList_.at(subType).erase(subscriber);
    }

    // Remove the messaging list if it's empty
    if (subscriberList_.at(subType).empty())
      subscriberList_.erase(subType);
  }


  /****************************************************************************/
  /*!
    \brief
      Sets up a request on a messenger with a given name and function

    \param reqType
      Name of the request event

    \param function
      Request action to use to retrieve data
  */
  /****************************************************************************/
  void Messenger::SetupRequest( const std::string & reqType,
                                REQUEST_ACTION function)
  {
    AddRequest(reqType, function);
  }

  /****************************************************************************/
  /*!
    \brief
      Adds a request on a messenger with a given name and function

    \param reqType
      Name of the request event

    \param function
      Request action to use to retrieve data
  */
  /****************************************************************************/
  void Messenger::AddRequest(const std::string & reqType,
    REQUEST_ACTION function)
  {
    requestList_.insert(std::make_pair(reqType, function));
  }
  
  /****************************************************************************/
  /*!
    \brief
      Removes a request from a messenger

    \param reqType
      Type of request to remove
  */
  /****************************************************************************/
  void Messenger::RemoveRequest(const std::string & reqType)
  {
    requestList_.erase(reqType);
  }

  void Messenger::DisconnectRequest(const std::string & reqType)
  {
    RemoveRequest(reqType);
  }

  void Messenger::PostMsg(const std::string & eventType, const Packet & payload)
  {
    try
    {
      //auto subscribers = subscriberList_.at(eventType);
      // Get each subscriber listening for the given event type
      for (auto & subscriber_entry : subscriberList_.at(eventType))
      {
        for(auto & sub : subscriber_entry.second)
          sub.second(payload); // Call the event function with the given payload
      }
    }
    // No event listeners exist for that type, no need to do anything
    catch (std::out_of_range &)
    {}
  }
}
