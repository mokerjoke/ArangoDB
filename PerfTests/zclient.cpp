//
// Hello World client
// Connects REQ socket to tcp://localhost:5555
// Sends "Hello" to server, expects "World" back
//
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <getopt.h>
#include <pthread.h>

#include <string>
#include <map>
#include <algorithm>
#include <iostream>

#include "ProtocolBuffers/arangodb.pb.h"

using namespace std;

void* context = 0;
size_t errors = 0;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
bool canStart = false;

// default values
size_t loop = 10000;
size_t concurrency = 1;
size_t pipelined = 1;
char const* connection = "tcp://localhost:5555";
char const* path = "/_api/version";
bool verbose = false;
map<string, string> headers;
map<string, string> values;



void* ThreadStarter (void* data) {
  int res, rc;
  size_t n;
  void* requester;

  requester = zmq_socket(context, ZMQ_REQ);
  res = zmq_connect(requester, connection);

  if (res != 0) {
    printf("ERROR zmq_connect: %d\n", errno);
    exit(EXIT_FAILURE);
  }
  
  if (verbose) {
    printf("connected\n");
  }

  rc = pthread_mutex_lock(&mutex);
    
  while (!canStart) {
    rc = pthread_cond_wait(&condition, &mutex);
  }
                      
  rc = pthread_mutex_unlock(&mutex);

  for (n = 0; n < loop; n += pipelined) {
    zmq_msg_t request;
    zmq_msg_t reply;

    PB_ArangoMessage messages;
    PB_ArangoBatchMessage* batch;
    PB_ArangoBlobRequest* blob;
    PB_ArangoKeyValue* kv;

    for (size_t p = 0; p < pipelined; p++) {
      batch = messages.add_messages();

      batch->set_type(PB_BLOB_REQUEST);
      blob = batch->mutable_blobrequest();

      map<string, string>::iterator it;

      for (it = headers.begin(); it != headers.end(); ++it) {
        kv = blob->add_headers();
        kv->set_key((*it).first);
        kv->set_value((*it).second);
      }
      
      for (it = values.begin(); it != values.end(); ++it) {
        kv = blob->add_values();
        kv->set_key((*it).first);
        kv->set_value((*it).second);
      }

      blob->set_requesttype(PB_REQUEST_TYPE_GET);
      blob->set_url(path);
      blob->set_contenttype(PB_NO_CONTENT);
      blob->set_content("");
    }
   
    string data;
    messages.SerializeToString(&data);

    // send
    zmq_msg_init_size(&request, data.size());
    memcpy(zmq_msg_data(&request), data.c_str(), data.size());

    res = zmq_send(requester, &request, 0);

    if (res != 0) {
      printf("ERROR zmq_send: %d %d %s\n", (int) res, (int) errno, strerror(errno));
      ++errors;
    }

    zmq_msg_close(&request);

    // receive
    zmq_msg_init(&reply);

    res = zmq_recv(requester, &reply, 0);

    if (res != 0) {
      printf("ERROR zmq_recv: %d %d %s\n", (int) res, (int) errno, strerror(errno));
      ++errors;
    }

    zmq_msg_close(&reply);
  }

  zmq_close(requester);

  return 0;
}


/* Return 1 if the difference is negative, otherwise 0.  */
int timeval_subtract(struct timeval* result, struct timeval* t2, struct timeval* t1) {
  long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
  result->tv_sec = diff / 1000000;
  result->tv_usec = diff % 1000000;

  return (diff<0);
}


void addHeader (char* arg) {
  string header(arg);
  size_t found = header.find('=', 0);

  if (found == string::npos) {
    fprintf(stderr, "invalid header %s\n", arg);
    exit(EXIT_FAILURE);
  }

  string key(header.substr(0, found));
  string value(header.substr(found + 1, header.length() - found - 1));

  std::transform(key.begin(), key.end(), key.begin(), ::tolower);
  headers[key] = value;
}

void addValue (char* arg) {
  string data(arg);
  size_t found = data.find('=', 0);

  if (found == string::npos) {
    fprintf(stderr, "invalid value %s\n", arg);
    exit(EXIT_FAILURE);
  }

  string key(data.substr(0, found));
  string value(data.substr(found + 1, data.length() - found - 1));

  values[key] = value;
}
        
void printUsage (char* prg) {        
  fprintf(stderr, "Usage: %s [-C connection] [-u path] [-h \"key=value\"] [-v \"key=value\"] [-P pipeline] [-c concurrency] [-n count] [-d]\n", prg);
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "-C connection       connection string, e.g. tcp://localhost:5555\n");
  fprintf(stderr, "-u path             endpoint to call, e.g. /_api/version\n");
  fprintf(stderr, "-h \"key=value\"      add arbitrary header\n");
  fprintf(stderr, "-v \"key=value\"      add arbitrary body value\n");
  fprintf(stderr, "-P pipeline         send <pipeline> values in each batch (default: 1)\n");
  fprintf(stderr, "-c concurrency      number of concurrent client threads (default: 1)\n");
  fprintf(stderr, "-n count            number of requests to perform per client thread\n");
}


int main (int argc, char* argv[]) {
  struct timeval start, end, result;
  float rps, runtime;
  size_t i;
  int opt, rc;
  pthread_t* threads;

  while ((opt = getopt(argc, argv, "C:u:h:v:P:c:n:d")) != -1) {
    switch (opt) {
      case 'C':
        connection = optarg;
        break;
      case 'u':
        path = optarg;
        break;
      case 'h':
        addHeader(optarg);
        break;
      case 'v':
        addValue(optarg);
        break;
      case 'P':
        pipelined = atoi(optarg);
        if (pipelined < 1) {
          fprintf(stderr, "pipeline argument must be at least 1\n");
          exit(EXIT_FAILURE);
        }
        break;
      case 'c':
        concurrency = atoi(optarg);
        if (concurrency < 1) {
          fprintf(stderr, "concurrency must be at least 1\n");
          exit(EXIT_FAILURE);
        }
        break;
      case 'n':
        loop = atoi(optarg);
        break;
      case 'd':
        verbose = true;
        break;
      default: 
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }
  }

  printf("Looping %d times per thread, total %d, pipelined %d\n", (int) loop, (int) (loop * concurrency), (int) pipelined);
  printf("Concurrency %d\n", (int) concurrency);
  printf("Connection %s\n", connection);

  context = zmq_init(16);

  if (verbose) {
    printf("Connecting to server…\n");
  }

  threads = (pthread_t*) malloc(sizeof(pthread_t) * concurrency);

  for (i = 0;  i < concurrency;  ++i) {
    pthread_create(&threads[i], 0, &ThreadStarter, 0);
  }

  gettimeofday(&start, 0);
  rc = pthread_mutex_lock(&mutex);
  canStart = true;
  rc = pthread_cond_broadcast(&condition);
  rc = pthread_mutex_unlock(&mutex);

  for (i = 0;  i < concurrency;  ++i) {
    pthread_join(threads[i], 0);
  }

  gettimeofday(&end, 0);
  timeval_subtract(&result, &end, &start);
  runtime = (float) result.tv_sec + (float) ((float) result.tv_usec / 1000000.0);

  rps = (float) loop * (float) concurrency / runtime;
  printf("runtime %f sec, req/sec %f, req/sec/thread %f\n", runtime, rps, rps / (float) concurrency);
  printf("errors %ld\n", (long int) errors);

  zmq_term(context);
  free(threads);

  pthread_cond_destroy(&condition);
  pthread_mutex_destroy(&mutex);

  return 0;
}
