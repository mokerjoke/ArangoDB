////////////////////////////////////////////////////////////////////////////////
/// @brief V8 enigne configuration
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2004-2012 triagens GmbH, Cologne, Germany
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
/// @author Copyright 2011-2012, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef TRIAGENS_REST_SERVER_APPLICATION_V8_H
#define TRIAGENS_REST_SERVER_APPLICATION_V8_H 1

#include "ApplicationServer/ApplicationFeature.h"

#include <v8.h>

#include "Basics/ConditionVariable.h"
#include "V8/JSLoader.h"

// -----------------------------------------------------------------------------
// --SECTION--                                              forward declarations
// -----------------------------------------------------------------------------

extern "C" {
  struct TRI_vocbase_s;
}

namespace triagens {
  namespace basics {
    class Thread;
  }

// -----------------------------------------------------------------------------
// --SECTION--                                               class ApplicationV8
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup ArangoDB
/// @{
////////////////////////////////////////////////////////////////////////////////

  namespace arango {

////////////////////////////////////////////////////////////////////////////////
/// @brief application simple user and session management feature
////////////////////////////////////////////////////////////////////////////////

    class ApplicationV8 : public rest::ApplicationFeature {
      private:
        ApplicationV8 (ApplicationV8 const&);
        ApplicationV8& operator= (ApplicationV8 const&);

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                      public types
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup ArangoDB
/// @{
////////////////////////////////////////////////////////////////////////////////

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief V8 isolate and context
////////////////////////////////////////////////////////////////////////////////

        struct V8Context {
          v8::Persistent<v8::Context> _context;
          v8::Isolate* _isolate;
          v8::Locker* _locker;
          size_t _dirt;
        };

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup ArangoDB
/// @{
////////////////////////////////////////////////////////////////////////////////

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief constructor
////////////////////////////////////////////////////////////////////////////////

        ApplicationV8 (string const& binaryPath);

////////////////////////////////////////////////////////////////////////////////
/// @brief destructor
////////////////////////////////////////////////////////////////////////////////

        ~ApplicationV8 ();

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup ArangoDB
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief sets the concurrency
////////////////////////////////////////////////////////////////////////////////

        void setConcurrency (size_t);

////////////////////////////////////////////////////////////////////////////////
/// @brief sets the database
////////////////////////////////////////////////////////////////////////////////

        void setVocbase (struct TRI_vocbase_s*);

////////////////////////////////////////////////////////////////////////////////
/// @brief enters an context
////////////////////////////////////////////////////////////////////////////////

        V8Context* enterContext ();

////////////////////////////////////////////////////////////////////////////////
/// @brief exists an context
////////////////////////////////////////////////////////////////////////////////

        void exitContext (V8Context*);

////////////////////////////////////////////////////////////////////////////////
/// @brief runs the garbage collection
////////////////////////////////////////////////////////////////////////////////

        void collectGarbage ();

////////////////////////////////////////////////////////////////////////////////
/// @brief disables actions
////////////////////////////////////////////////////////////////////////////////

        void disableActions ();

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                        ApplicationFeature methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup ApplicationServer
/// @{
////////////////////////////////////////////////////////////////////////////////

      public:

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

        void setupOptions (map<string, basics::ProgramOptionsDescription>&);

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

        bool prepare ();

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

        bool isStartable ();

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

        bool start ();

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

        bool isStarted ();

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

        void beginShutdown ();

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

        void shutdown ();

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                   private methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup ArangoDB
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief prepares a V8 instance
////////////////////////////////////////////////////////////////////////////////

        bool prepareV8Instance (size_t i);

////////////////////////////////////////////////////////////////////////////////
/// @brief shut downs a V8 instances
////////////////////////////////////////////////////////////////////////////////

        void shutdownV8Instance (size_t i);

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                 private variables
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup ArangoDB
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief path to the directory containing alternate startup scripts
///
/// @CMDOPT{--javascript.directory @CA{directory}}
///
/// Specifies the @CA{directory} path to alternate startup JavaScript files.
/// Normally, the server will start using built-in JavaScript core
/// functionality. To override the core functionality with a different
/// implementation, this option can be used.
////////////////////////////////////////////////////////////////////////////////

        string _startupPath;

////////////////////////////////////////////////////////////////////////////////
/// @brief semicolon separated list of module directories
///
/// @CMDOPT{--javascript.modules-path @CA{directory}}
///
/// Specifies the @CA{directory} path with user defined JavaScript modules.
/// Multiple paths can be specified separated with commas.
////////////////////////////////////////////////////////////////////////////////

        string _startupModules;

////////////////////////////////////////////////////////////////////////////////
/// @brief path to the system action directory
///
/// @CMDOPT{--javascript.action-directory @CA{directory}}
///
/// Specifies the @CA{directory} containg the system defined JavaScript files
/// that can be invoked as actions.
////////////////////////////////////////////////////////////////////////////////

        string _actionPath;

////////////////////////////////////////////////////////////////////////////////
/// @brief JavaScript garbage collection interval (each x requests)
///
/// @CMDOPT{--javascript.gc-interval @CA{interval}}
///
/// Specifies the interval (approximately in number of requests) that the
/// garbage collection for JavaScript objects will be run in each thread.
////////////////////////////////////////////////////////////////////////////////

        uint64_t _gcInterval;

////////////////////////////////////////////////////////////////////////////////
/// @brief V8 startup loader
////////////////////////////////////////////////////////////////////////////////

        JSLoader _startupLoader;

////////////////////////////////////////////////////////////////////////////////
/// @brief V8 action loader
////////////////////////////////////////////////////////////////////////////////

        JSLoader _actionLoader;

////////////////////////////////////////////////////////////////////////////////
/// @brief database
////////////////////////////////////////////////////////////////////////////////

        struct TRI_vocbase_s* _vocbase;

////////////////////////////////////////////////////////////////////////////////
/// @brief number of instances to create
////////////////////////////////////////////////////////////////////////////////

        size_t _nrInstances;

////////////////////////////////////////////////////////////////////////////////
/// @brief V8 contexts
////////////////////////////////////////////////////////////////////////////////

        V8Context** _contexts;

////////////////////////////////////////////////////////////////////////////////
/// @brief V8 contexts queue lock
////////////////////////////////////////////////////////////////////////////////

        basics::ConditionVariable _contextCondition;

////////////////////////////////////////////////////////////////////////////////
/// @brief V8 free contexts
////////////////////////////////////////////////////////////////////////////////

        std::vector<V8Context*> _freeContexts;

////////////////////////////////////////////////////////////////////////////////
/// @brief V8 free contexts
////////////////////////////////////////////////////////////////////////////////

        std::vector<V8Context*> _dirtyContexts;

////////////////////////////////////////////////////////////////////////////////
/// @brief shutdown in progress
////////////////////////////////////////////////////////////////////////////////

        volatile sig_atomic_t _stopping;

////////////////////////////////////////////////////////////////////////////////
/// @brief garbage collection thread
////////////////////////////////////////////////////////////////////////////////

        basics::Thread* _gcThread;
    };
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

#endif

// Local Variables:
// mode: outline-minor
// outline-regexp: "^\\(/// @brief\\|/// {@inheritDoc}\\|/// @addtogroup\\|// --SECTION--\\|/// @\\}\\)"
// End:
