/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2017 jwellbelove

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef ETL_REFERENCE_COUNTED_MESSAGE_INCLUDED
#define ETL_REFERENCE_COUNTED_MESSAGE_INCLUDED

#include <stdint.h>

#include "platform.h"
#include "message.h"
#include "atomic.h"
#include "reference_counted_object.h"
#include "ireference_counted_message_pool.h"

namespace etl
{
  //***************************************************************************
  // Base class for all reference counted messages.
  //***************************************************************************
  class ireference_counted_message
  {
  public:

    virtual ~ireference_counted_message() {}
    ETL_NODISCARD virtual const etl::imessage& get_message() const = 0;                     ///< Get a const reference to the message.
    ETL_NODISCARD virtual etl::ireference_counter& get_reference_counter() = 0;             ///< Get a reference to the reference counter.
    ETL_NODISCARD virtual const etl::ireference_counter& get_reference_counter() const = 0; ///< Get a const reference to the reference counter.
    virtual void release() = 0;                                                             ///< Release back to the owner.
  };

  //***************************************************************************
  // Reference counted message type.
  //***************************************************************************
  class ipool_message : public etl::ireference_counted_message
  {
  };

  template <typename TMessage, typename TCounter>
  class pool_message : public etl::ipool_message
  {
  public:

    typedef TMessage message_type;
    typedef TCounter counter_type;

    //***************************************************************************
    /// Constructor
    /// \param msg The message to count.
    //***************************************************************************
    pool_message(const TMessage& msg_, etl::ireference_counted_message_pool& owner_)
      : rc_object(msg_)
      , owner(owner_)
    {
    }

    //***************************************************************************
    /// Get a const reference to the message.
    /// \return A const reference to the message.
    //***************************************************************************
    ETL_NODISCARD virtual const TMessage& get_message() const ETL_OVERRIDE
    {
      return rc_object.get_object();
    }

    //***************************************************************************
    /// Get a reference to the reference counter.
    /// \return A reference to the reference counter.
    //***************************************************************************
    ETL_NODISCARD virtual etl::ireference_counter& get_reference_counter() ETL_OVERRIDE
    {
      return rc_object.get_reference_counter();
    }

    //***************************************************************************
    /// Get a const reference to the reference counter.
    /// \return A const reference to the reference counter.
    //***************************************************************************
    ETL_NODISCARD virtual const etl::ireference_counter& get_reference_counter() const ETL_OVERRIDE
    {
      return rc_object.get_reference_counter();
    }

    //***************************************************************************
    /// Release back to the owner pool.
    /// \return A reference to the owner pool.
    //***************************************************************************
    virtual void release() ETL_OVERRIDE
    {
      owner.release(*this);
    }

  private:

    etl::reference_counted_object<TMessage, TCounter> rc_object; ///< The reference counted object.
    etl::ireference_counted_message_pool& owner;                 ///< The pool that owns this object.
  };

  //***************************************************************************
  /// Persistent message type.
  /// The message type will always have a reference count of 1. 
  /// \tparam TMessage  The message type stored.
  //***************************************************************************
  class inon_pool_message : public etl::ireference_counted_message
  {
  };
  
  template <typename TMessage>
  class non_pool_message : public etl::inon_pool_message
  {
  public:

    typedef TMessage message_type;

    //***************************************************************************
    /// Constructor
    /// \param msg The message to count.
    //***************************************************************************
    explicit non_pool_message(const TMessage& msg_)
      : rc_object(msg_)
    {
    }

    //***************************************************************************
    /// Get a const reference to the message.
    /// \return A const reference to the message.
    //***************************************************************************
    ETL_NODISCARD virtual const TMessage& get_message() const ETL_OVERRIDE
    {
      return rc_object.get_object();
    }

    //***************************************************************************
    /// Get a reference to the reference counter.
    /// \return A reference to the reference counter.
    //***************************************************************************
    ETL_NODISCARD virtual etl::ireference_counter& get_reference_counter() ETL_OVERRIDE
    {
      return rc_object.get_reference_counter();
    }

    //***************************************************************************
    /// Get a const reference to the reference counter.
    /// \return A const reference to the reference counter.
    //***************************************************************************
    ETL_NODISCARD virtual const etl::ireference_counter& get_reference_counter() const ETL_OVERRIDE
    {
      return rc_object.get_reference_counter();
    }

    //***************************************************************************
    /// Release back to the owner pool.
    /// \return A reference to the owner pool.
    //***************************************************************************
    virtual void release() ETL_OVERRIDE
    {
      // Do nothing.
    }

  private:

    // This class must not be default contructed, copy constructed or assigned.
    non_pool_message() ETL_DELETE;
    non_pool_message(const non_pool_message&) ETL_DELETE;
    non_pool_message& operator =(const non_pool_message&) ETL_DELETE;

    etl::persistent_object<TMessage> rc_object; ///< The reference counted object.
  };

#if ETL_CPP11_SUPPORTED && ETL_HAS_ATOMIC
  //***************************************************************************
  /// Class for creating reference counted objects using an atomic counter.
  /// \tparam TObject  The type to be reference counted.
  //***************************************************************************
  template <typename TMessage>
  using atomic_counted_message = etl::pool_message<TMessage, etl::atomic_int32_t>;
#endif
}

#endif