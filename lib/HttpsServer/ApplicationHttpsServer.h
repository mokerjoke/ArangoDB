////////////////////////////////////////////////////////////////////////////////
/// @brief application https server feature
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2004-2012 triAGENS GmbH, Cologne, Germany
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
/// @author Copyright 2010-2012, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef TRIAGENS_HTTPS_SERVER_APPLICATION_HTTPS_SERVER_H
#define TRIAGENS_HTTPS_SERVER_APPLICATION_HTTPS_SERVER_H 1

#include "ApplicationServer/ApplicationFeature.h"

#include <openssl/ssl.h>

#include "HttpServer/HttpHandlerFactory.h"
#include "Rest/EndpointList.h"

// -----------------------------------------------------------------------------
// --SECTION--                                              forward declarations
// -----------------------------------------------------------------------------

namespace triagens {
  namespace rest {
    class ApplicationScheduler;
    class ApplicationDispatcher;
    class HttpsServer;

// -----------------------------------------------------------------------------
// --SECTION--                                      class ApplicationsHttpServer
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup HttpServer
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief application https server feature
////////////////////////////////////////////////////////////////////////////////

    class ApplicationHttpsServer : public ApplicationFeature {
      private:
        ApplicationHttpsServer (ApplicationHttpsServer const&);
        ApplicationHttpsServer& operator= (ApplicationHttpsServer const&);

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup HttpServer
/// @{
////////////////////////////////////////////////////////////////////////////////

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief constructor
////////////////////////////////////////////////////////////////////////////////

        ApplicationHttpsServer (ApplicationServer*,
                                ApplicationScheduler*, 
                                ApplicationDispatcher*,
                                std::string const& authenticationRealm,
                                HttpHandlerFactory::auth_fptr checkAuthentication);

////////////////////////////////////////////////////////////////////////////////
/// @brief destructor
////////////////////////////////////////////////////////////////////////////////

        ~ApplicationHttpsServer ();

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup HttpServer
/// @{
////////////////////////////////////////////////////////////////////////////////

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief builds the https server
///
/// Note that the server claims ownership of the factory.
////////////////////////////////////////////////////////////////////////////////

        HttpsServer* buildServer (const EndpointList* endpointList); 

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

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

        void setupOptions (map<string, basics::ProgramOptionsDescription>&);

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

        bool parsePhase2 (basics::ProgramOptions&);

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

        bool open ();

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

        void  close ();

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

        void stop ();

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                 protected methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup HttpServer
/// @{
////////////////////////////////////////////////////////////////////////////////

      protected:

////////////////////////////////////////////////////////////////////////////////
/// @brief build an http server
////////////////////////////////////////////////////////////////////////////////

        HttpsServer* buildHttpsServer (const EndpointList*);

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                               protected variables
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup HttpServer
/// @{
////////////////////////////////////////////////////////////////////////////////

      protected:

////////////////////////////////////////////////////////////////////////////////
/// @brief application server
////////////////////////////////////////////////////////////////////////////////

        ApplicationServer* _applicationServer;

////////////////////////////////////////////////////////////////////////////////
/// @brief application scheduler
////////////////////////////////////////////////////////////////////////////////

        ApplicationScheduler* _applicationScheduler;

////////////////////////////////////////////////////////////////////////////////
/// @brief application dispatcher or null
////////////////////////////////////////////////////////////////////////////////

        ApplicationDispatcher* _applicationDispatcher;

////////////////////////////////////////////////////////////////////////////////
/// @brief authentication realm
////////////////////////////////////////////////////////////////////////////////

        string _authenticationRealm;

////////////////////////////////////////////////////////////////////////////////
/// @brief authentication callback
////////////////////////////////////////////////////////////////////////////////

        HttpHandlerFactory::auth_fptr _checkAuthentication;

////////////////////////////////////////////////////////////////////////////////
/// @brief all constructed http servers
////////////////////////////////////////////////////////////////////////////////

        vector<HttpsServer*> _httpsServers;

////////////////////////////////////////////////////////////////////////////////
/// @brief keyfile
////////////////////////////////////////////////////////////////////////////////

        string _httpsKeyfile;

////////////////////////////////////////////////////////////////////////////////
/// @brief CA file
////////////////////////////////////////////////////////////////////////////////

        string _cafile;

////////////////////////////////////////////////////////////////////////////////
/// @brief ssl protocol to use
////////////////////////////////////////////////////////////////////////////////

        uint32_t _sslProtocol;

////////////////////////////////////////////////////////////////////////////////
/// @brief ssl cache mode to use
////////////////////////////////////////////////////////////////////////////////

        uint64_t _sslCacheMode;

////////////////////////////////////////////////////////////////////////////////
/// @brief ssl options to use
////////////////////////////////////////////////////////////////////////////////

        uint64_t _sslOptions;

////////////////////////////////////////////////////////////////////////////////
/// @brief ssl cipher list to use
////////////////////////////////////////////////////////////////////////////////

        string _sslCipherList;

////////////////////////////////////////////////////////////////////////////////
/// @brief ssl context
////////////////////////////////////////////////////////////////////////////////

        SSL_CTX* _sslContext;

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                   private methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup HttpServer
/// @{
////////////////////////////////////////////////////////////////////////////////

      private:

////////////////////////////////////////////////////////////////////////////////
/// @brief creates an ssl context
////////////////////////////////////////////////////////////////////////////////

        bool createSslContext ();
    };
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

#endif

// -----------------------------------------------------------------------------
// --SECTION--                                                       END-OF-FILE
// -----------------------------------------------------------------------------

// Local Variables:
// mode: outline-minor
// outline-regexp: "^\\(/// @brief\\|/// {@inheritDoc}\\|/// @addtogroup\\|// --SECTION--\\|/// @\\}\\)"
// End:
