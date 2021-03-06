////////////////////////////////////////////////////////////////////////////////
/// @brief general server with dispatcher
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

#ifndef TRIAGENS_FYN_GENERAL_SERVER_GENERAL_SERVER_DISPATCHER_H
#define TRIAGENS_FYN_GENERAL_SERVER_GENERAL_SERVER_DISPATCHER_H 1

#include "GeneralServer/GeneralServer.h"

#include "Dispatcher/Dispatcher.h"
#include "Dispatcher/Job.h"
#include "GeneralServer/GeneralCommTask.h"
#include "GeneralServer/GeneralAsyncCommTask.h"

namespace triagens {
  namespace rest {
    class Dispatcher;

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief general server with dispatcher
    ////////////////////////////////////////////////////////////////////////////////

    template<typename S, typename HF, typename CT>
    class GeneralServerDispatcher : public GeneralServer<S, HF, CT> {
      GeneralServerDispatcher (GeneralServerDispatcher const&);
      GeneralServerDispatcher& operator= (GeneralServerDispatcher const&);

      public:

        ////////////////////////////////////////////////////////////////////////////////
        /// @brief constructs a new general server
        ////////////////////////////////////////////////////////////////////////////////

        explicit
        GeneralServerDispatcher (Scheduler* scheduler)
          : GeneralServer<S, HF, CT>(scheduler), _dispatcher(0) {
        }

        ////////////////////////////////////////////////////////////////////////////////
        /// @brief constructs a new general server
        ////////////////////////////////////////////////////////////////////////////////

        GeneralServerDispatcher (Scheduler* scheduler, Dispatcher* dispatcher)
          : GeneralServer<S, HF, CT>(scheduler), _dispatcher(dispatcher) {
        }

      public:

        ////////////////////////////////////////////////////////////////////////////////
        /// {@inheritDoc}
        ////////////////////////////////////////////////////////////////////////////////

        void handleConnected (socket_t socket, ConnectionInfo& info) {
          SocketTask* task = new GeneralAsyncCommTask<S, HF, CT>(dynamic_cast<S*>(this), socket, info);

          this->_scheduler->registerTask(task);
        }

        ////////////////////////////////////////////////////////////////////////////////
        /// {@inheritDoc}
        ////////////////////////////////////////////////////////////////////////////////

        bool handleRequest (CT * task, typename HF::GeneralHandler *& handler) {

          // execute handler and (possibly) requeue
          bool done = false;
          
          assert(handler);

          while (! done) {

            // directly execute the handler within the scheduler thread
            if (handler->isDirect()) {
              Handler::status_e status = this->handleRequestDirectly(task, handler);

              if (status != Handler::HANDLER_REQUEUE) {
                done = true;
                this->destroyHandler(handler);
                handler = 0;
              }
              else {
                continue;
              }
            }

            // execute the handler using the dispatcher
            else if (_dispatcher != 0) {
              GeneralAsyncCommTask<S, HF, CT>* atask = dynamic_cast<GeneralAsyncCommTask<S, HF, CT>*>(task);

              if (atask == 0) {
                LOGGER_WARNING << "task is indirect, but not asynchronous - this cannot work!";
                this->destroyHandler(handler);
                handler = 0;

                return false;
              }
              else {
                atask->setHandler(handler);
                Job* job = handler->createJob(this->_scheduler, _dispatcher, atask);
                _dispatcher->addJob(job);
              }

              return true;
            }

            // without a dispatcher, simply give up
            else {
              LOGGER_WARNING << "no dispatcher is known";

              this->destroyHandler(handler);
              handler = 0;

              return false;
            }
          }

          return true;
        }

      protected:

        ////////////////////////////////////////////////////////////////////////////////
        /// @brief the dispatcher
        ////////////////////////////////////////////////////////////////////////////////

        Dispatcher* _dispatcher;
    };
  }
}

#endif




