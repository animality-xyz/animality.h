#ifndef ANIMALITY_H
#define ANIMALITY_H

#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#  include <windows.h>
#else
#  include <pthread.h>
#endif

#include <curl/curl.h>

/* size macros */
#define ANIMALS_LENGTH 15
#define ANIMALS_MAX_EACH_SIZE 9

/* empty animal_t struct template */
#define AN_EMPTY_ANIMAL { (an_type_t)0, NULL, NULL, NULL }

#ifdef __cplusplus
extern "C" {
#endif

/* animal enums used for requesting */
typedef enum {
    AN_CAT,
    AN_DOG,
    AN_BIRD,
    AN_PANDA,
    AN_REDPANDA,
    AN_KOALA,
    AN_FOX,
    AN_WHALE,
    AN_KANGAROO,
    AN_BUNNY,
    AN_LION,
    AN_BEAR,
    AN_FROG,
    AN_DUCK,
    AN_PENGUIN,
    AN_RANDOM
} an_type_t;

/* main animality response type */
typedef struct {
    an_type_t type;
    char * name;
    char * image_url;
    char * fact;
} animal_t;

/* exported functions */
void an_get(const an_type_t _t, animal_t * _out);
void an_cleanup(animal_t * _tr);

#ifdef _WIN32

typedef HANDLE an_thread_t;
typedef DWORD async_cb_ret_t;

#define an_thread_wait(t_) \
  WaitForSingleObject(t_, 15000)

#else

typedef pthread_t an_thread_t;
typedef void * async_cb_ret_t;

#define an_thread_wait(t_) \
  pthread_join(t_, NULL)

#endif

typedef void (* an_callback_t)(const animal_t *);
typedef struct {
    an_callback_t callback;
    an_type_t type;
} an_thread_arg_t;

const an_thread_t an_get_async(const an_type_t _t, const an_callback_t cb);

#ifdef __cplusplus
}
#endif
#endif