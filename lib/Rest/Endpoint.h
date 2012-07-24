////////////////////////////////////////////////////////////////////////////////
/// @brief connection endpoints
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
/// @author Jan Steemann
/// @author Copyright 2012, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef TRIAGENS_FYN_REST_ENDPOINT_H
#define TRIAGENS_FYN_REST_ENDPOINT_H 1

#include <Basics/Common.h>
#include <Basics/StringUtils.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#ifdef TRI_HAVE_LINUX_SOCKETS
#include <sys/un.h>
#endif 

#include <sys/socket.h>
#include <netdb.h>

// -----------------------------------------------------------------------------
// --SECTION--                                                          Endpoint
// -----------------------------------------------------------------------------

namespace triagens {
  namespace rest {

////////////////////////////////////////////////////////////////////////////////
/// @brief endpoint specification
////////////////////////////////////////////////////////////////////////////////

    class Endpoint {

// -----------------------------------------------------------------------------
// --SECTION--                                                          typedefs
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////
      
      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief endpoint types
////////////////////////////////////////////////////////////////////////////////

        enum Type {
          ENDPOINT_SERVER,
          ENDPOINT_CLIENT 
        };

////////////////////////////////////////////////////////////////////////////////
/// @brief endpoint types
////////////////////////////////////////////////////////////////////////////////

        enum DomainType {
          ENDPOINT_UNKNOWN = 0,
#ifdef TRI_HAVE_LINUX_SOCKETS
          ENDPOINT_UNIX,
#endif
          ENDPOINT_IPV4,
          ENDPOINT_IPV6
        };

////////////////////////////////////////////////////////////////////////////////
/// @brief protocols used for endpoints
////////////////////////////////////////////////////////////////////////////////

        enum Protocol {
          PROTOCOL_UNKNOWN,
          PROTOCOL_BINARY,
#ifdef TRI_OPENSSL_VERSION
          PROTOCOL_HTTPS,
#endif
          PROTOCOL_HTTP
        };

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                        constructors / destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////

      protected:

////////////////////////////////////////////////////////////////////////////////
/// @brief creates an endpoint
////////////////////////////////////////////////////////////////////////////////

        Endpoint (const Type, 
                  const DomainType, 
                  const Protocol, 
                  const string&);

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief destroys an endpoint
////////////////////////////////////////////////////////////////////////////////
      
        virtual ~Endpoint ();

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////
    
      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief creates a server endpoint from a string value
////////////////////////////////////////////////////////////////////////////////

        static Endpoint* serverFactory (const string&);

////////////////////////////////////////////////////////////////////////////////
/// @brief creates a client endpoint from a string value
////////////////////////////////////////////////////////////////////////////////

        static Endpoint* clientFactory (const string&);

////////////////////////////////////////////////////////////////////////////////
/// @brief creates an endpoint from a string value
////////////////////////////////////////////////////////////////////////////////

        static Endpoint* factory (const Type type, 
                                  const string&);

////////////////////////////////////////////////////////////////////////////////
/// @brief compare two endpoints
////////////////////////////////////////////////////////////////////////////////

        bool operator== (Endpoint const &) const;

////////////////////////////////////////////////////////////////////////////////
/// @brief return the default endpoint
////////////////////////////////////////////////////////////////////////////////

        static const std::string getDefaultEndpoint ();

////////////////////////////////////////////////////////////////////////////////
/// @brief connect the endpoint
////////////////////////////////////////////////////////////////////////////////
        
        virtual int connect () = 0; 

////////////////////////////////////////////////////////////////////////////////
/// @brief disconnect the endpoint
////////////////////////////////////////////////////////////////////////////////
        
        virtual void disconnect () = 0; 

////////////////////////////////////////////////////////////////////////////////
/// @brief init an incoming connection
////////////////////////////////////////////////////////////////////////////////

        virtual bool initIncoming (socket_t) = 0;

////////////////////////////////////////////////////////////////////////////////
/// @brief initialise socket flags
////////////////////////////////////////////////////////////////////////////////
        
        virtual bool setSocketFlags (socket_t); 

////////////////////////////////////////////////////////////////////////////////
/// @brief return whether the endpoint is connected
////////////////////////////////////////////////////////////////////////////////

        bool isConnected () const {
          return _connected;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief get the type of an endpoint
////////////////////////////////////////////////////////////////////////////////

        Type getType () const {
          return _type;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief get the protocol of an endpoint
////////////////////////////////////////////////////////////////////////////////

        Protocol getProtocol () const {
          return _protocol;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief get the original endpoint specification
////////////////////////////////////////////////////////////////////////////////

        string getSpecification () const {
          return _specification;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief get endpoint domain
////////////////////////////////////////////////////////////////////////////////

        virtual int getDomain () const = 0;

////////////////////////////////////////////////////////////////////////////////
/// @brief get port number
////////////////////////////////////////////////////////////////////////////////

        virtual int getPort () const = 0;

////////////////////////////////////////////////////////////////////////////////
/// @brief get host name
////////////////////////////////////////////////////////////////////////////////

        virtual string getHost () const = 0;

////////////////////////////////////////////////////////////////////////////////
/// @brief get address
////////////////////////////////////////////////////////////////////////////////

        virtual string getHostString () const = 0;

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                               protected variables
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////

      protected:

////////////////////////////////////////////////////////////////////////////////
/// @brief whether or not the endpoint is connected
////////////////////////////////////////////////////////////////////////////////

        bool _connected;

////////////////////////////////////////////////////////////////////////////////
/// @brief the actual socket
////////////////////////////////////////////////////////////////////////////////

        socket_t _socket;

////////////////////////////////////////////////////////////////////////////////
/// @brief endpoint type
////////////////////////////////////////////////////////////////////////////////
      
        Type _type;

////////////////////////////////////////////////////////////////////////////////
/// @brief endpoint domain type
////////////////////////////////////////////////////////////////////////////////
      
        DomainType _domainType;

////////////////////////////////////////////////////////////////////////////////
/// @brief protocol used
////////////////////////////////////////////////////////////////////////////////

        Protocol _protocol;

////////////////////////////////////////////////////////////////////////////////
/// @brief original endpoint specification
////////////////////////////////////////////////////////////////////////////////

        string _specification;

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

    };

#ifdef TRI_HAVE_LINUX_SOCKETS

// -----------------------------------------------------------------------------
// --SECTION--                                                      EndpointUnix
// -----------------------------------------------------------------------------

    class EndpointUnix : public Endpoint {

// -----------------------------------------------------------------------------
// --SECTION--                                        constructors / destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief creates an endpoint
////////////////////////////////////////////////////////////////////////////////

        EndpointUnix (const Type, 
                      const Protocol,
                      string const&, 
                      string const&);

////////////////////////////////////////////////////////////////////////////////
/// @brief destroys an endpoint
////////////////////////////////////////////////////////////////////////////////
      
        virtual ~EndpointUnix ();

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////
    
      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief connect the endpoint
////////////////////////////////////////////////////////////////////////////////

        socket_t connect ();

////////////////////////////////////////////////////////////////////////////////
/// @brief disconnect the endpoint
////////////////////////////////////////////////////////////////////////////////
        
        virtual void disconnect ();

////////////////////////////////////////////////////////////////////////////////
/// @brief init an incoming connection
////////////////////////////////////////////////////////////////////////////////

        virtual bool initIncoming (socket_t);

////////////////////////////////////////////////////////////////////////////////
/// @brief get endpoint domain
////////////////////////////////////////////////////////////////////////////////
        
        int getDomain () const {
          return AF_UNIX;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief get port
////////////////////////////////////////////////////////////////////////////////

        int getPort () const {
          return 0;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief get host name
////////////////////////////////////////////////////////////////////////////////

        string getHost () const {
          return "localhost";
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief get host string for HTTP requests
////////////////////////////////////////////////////////////////////////////////

        string getHostString () const {
          return "localhost";
        }

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                 private variables
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////
    
      private:

////////////////////////////////////////////////////////////////////////////////
/// @brief socket file
////////////////////////////////////////////////////////////////////////////////

        string _path;

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

    };

#endif

// -----------------------------------------------------------------------------
// --SECTION--                                                        EndpointIp
// -----------------------------------------------------------------------------

    class EndpointIp : public Endpoint {

// -----------------------------------------------------------------------------
// --SECTION--                                        constructors / destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////

      protected:

////////////////////////////////////////////////////////////////////////////////
/// @brief creates an endpoint
////////////////////////////////////////////////////////////////////////////////

        EndpointIp (const Type, 
                    const DomainType, 
                    const Protocol,
                    string const&, 
                    string const&, 
                    const uint16_t);

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief destroys an endpoint
////////////////////////////////////////////////////////////////////////////////
      
        ~EndpointIp ();

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                  public variables
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief default port number if none specified
////////////////////////////////////////////////////////////////////////////////

        static const uint16_t _defaultPort;

////////////////////////////////////////////////////////////////////////////////
/// @brief default host if none specified
////////////////////////////////////////////////////////////////////////////////

        static const string _defaultHost;

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////


// -----------------------------------------------------------------------------
// --SECTION--                                                   private methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////
      
      private:

////////////////////////////////////////////////////////////////////////////////
/// @brief connect the socket
////////////////////////////////////////////////////////////////////////////////
        
        socket_t connectSocket (const struct addrinfo*);

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////
    
      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief connect the endpoint
////////////////////////////////////////////////////////////////////////////////

        socket_t connect ();

////////////////////////////////////////////////////////////////////////////////
/// @brief disconnect the endpoint
////////////////////////////////////////////////////////////////////////////////
        
        virtual void disconnect ();

////////////////////////////////////////////////////////////////////////////////
/// @brief init an incoming connection
////////////////////////////////////////////////////////////////////////////////

        virtual bool initIncoming (socket_t);

////////////////////////////////////////////////////////////////////////////////
/// @brief get port
////////////////////////////////////////////////////////////////////////////////

        int getPort () const {
          return _port;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief get host 
////////////////////////////////////////////////////////////////////////////////

        string getHost () const {
          return _host;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief get host strin for HTTP requests
////////////////////////////////////////////////////////////////////////////////

        string getHostString  () const {
          return _host + ':' + triagens::basics::StringUtils::itoa(_port);
        }

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                 private variables
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////
    
      private:

////////////////////////////////////////////////////////////////////////////////
/// @brief host name / address (IPv4 or IPv6)
////////////////////////////////////////////////////////////////////////////////

        string _host;

////////////////////////////////////////////////////////////////////////////////
/// @brief port number
////////////////////////////////////////////////////////////////////////////////

        uint16_t _port;

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

    };

// -----------------------------------------------------------------------------
// --SECTION--                                                      EndpointIpV4
// -----------------------------------------------------------------------------

    class EndpointIpV4 : public EndpointIp {

// -----------------------------------------------------------------------------
// --SECTION--                                        constructors / destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief creates an endpoint
////////////////////////////////////////////////////////////////////////////////

        EndpointIpV4 (const Type, 
                      const Protocol,
                      string const&, 
                      string const&, 
                      const uint16_t);

////////////////////////////////////////////////////////////////////////////////
/// @brief destroys an endpoint
////////////////////////////////////////////////////////////////////////////////
      
        ~EndpointIpV4 ();

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////
    
      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief get endpoint domain
////////////////////////////////////////////////////////////////////////////////
        
        int getDomain () const {
          return AF_INET;
        }

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

    };

// -----------------------------------------------------------------------------
// --SECTION--                                                      EndpointIpV6
// -----------------------------------------------------------------------------

    class EndpointIpV6 : public EndpointIp {

// -----------------------------------------------------------------------------
// --SECTION--                                        constructors / destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief creates an endpoint
////////////////////////////////////////////////////////////////////////////////

        EndpointIpV6 (const Type,
                      const Protocol, 
                      string const&, 
                      string const&, 
                      const uint16_t);

////////////////////////////////////////////////////////////////////////////////
/// @brief destroys an endpoint
////////////////////////////////////////////////////////////////////////////////
      
        ~EndpointIpV6 ();

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////
    
      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief get endpoint domain
////////////////////////////////////////////////////////////////////////////////
        
        int getDomain () const {
          return AF_INET6;
        }

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

    };

  }
}

#endif

// Local Variables:
// mode: outline-minor
// outline-regexp: "^\\(/// @brief\\|/// {@inheritDoc}\\|/// @addtogroup\\|// --SECTION--\\|/// @\\}\\)"
// End:
