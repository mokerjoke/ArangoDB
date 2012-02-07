////////////////////////////////////////////////////////////////////////////////
/// @brief tasks used to establish connections
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2010-2011 triagens GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is triAGENS GmbH, Cologne, Germany
///
/// @author Dr. Frank Celler
/// @author Achim Brandt
/// @author Copyright 2009-2011, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef TRIAGENS_FYN_REST_LISTEN_TASK_H
#define TRIAGENS_FYN_REST_LISTEN_TASK_H 1

#include <netdb.h>

#include "Scheduler/Task.h"

#include <Basics/Mutex.h>
#include <Rest/ConnectionInfo.h>

namespace triagens {
  namespace rest {

    ////////////////////////////////////////////////////////////////////////////////
    /// @ingroup Scheduler
    /// @brief task used to establish connections
    ////////////////////////////////////////////////////////////////////////////////

    class ListenTask : virtual public Task {
      public:

        ////////////////////////////////////////////////////////////////////////////////
        /// @brief maximal number of failed connects
        ////////////////////////////////////////////////////////////////////////////////

        static size_t const MAX_ACCEPT_ERRORS = 1000;

      public:

        ////////////////////////////////////////////////////////////////////////////////
        /// @brief listen to given address and port (deprecated)
        ////////////////////////////////////////////////////////////////////////////////

        ListenTask (string const& address, int port, bool reuseAddress);

        ////////////////////////////////////////////////////////////////////////////////
        /// @brief listen to given port (deprecated)
        ////////////////////////////////////////////////////////////////////////////////

        ListenTask (int port, bool reuseAddress);

        ////////////////////////////////////////////////////////////////////////////////
        /// @brief listen to given adress info pointer
        ////////////////////////////////////////////////////////////////////////////////

        ListenTask (struct addrinfo *aip, bool reuseAddress);

      public:

        ////////////////////////////////////////////////////////////////////////////////
        /// @brief checks if listen socket is bound
        ////////////////////////////////////////////////////////////////////////////////

        bool isBound () const;

      protected:

        ////////////////////////////////////////////////////////////////////////////////
        /// @brief called by the task to indicate connection success
        ////////////////////////////////////////////////////////////////////////////////

        virtual bool handleConnected (socket_t fd, ConnectionInfo const& info) = 0;

      protected:

        ////////////////////////////////////////////////////////////////////////////////
        /// @brief destructs a listen task
        ///
        /// This method will close the underlying socket.
        ////////////////////////////////////////////////////////////////////////////////

        ~ListenTask ();

      protected:

        ////////////////////////////////////////////////////////////////////////////////
        /// {@inheritDoc}
        ///
        /// Note that registerTask must only be called when the socket is bound.
        ////////////////////////////////////////////////////////////////////////////////

        void setup (Scheduler*, EventLoop);

        ////////////////////////////////////////////////////////////////////////////////
        /// {@inheritDoc}
        ////////////////////////////////////////////////////////////////////////////////

        void cleanup ();

        ////////////////////////////////////////////////////////////////////////////////
        /// {@inheritDoc}
        ////////////////////////////////////////////////////////////////////////////////

        bool handleEvent (EventToken token, EventType);

      protected:

        ////////////////////////////////////////////////////////////////////////////////
        /// @brief event for read
        ////////////////////////////////////////////////////////////////////////////////

        EventToken readWatcher;

      private:
        bool bindSocket ();
        bool bindSocket (struct addrinfo *aip);

      private:
        bool reuseAddress;
        string address;
        int port;
        socket_t listenSocket;
        bool bound;

        size_t acceptFailures;

        mutable basics::Mutex changeLock;
    };
  }
}

#endif
