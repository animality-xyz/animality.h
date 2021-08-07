#ifndef ANIMALITY_H
#define ANIMALITY_H

#include <stdlib.h>
#include <string.h>
#include <time.h>

// dependencies
#include <curl/curl.h>
#include <cjson/cJSON.h>

// size macros
#define ANIMALS_LENGTH           15
#define ANIMALS_MAX_EACH_SIZE    9

// empty animal_t struct template
#define AN_EMPTY_ANIMAL          { 0, NULL, NULL, NULL }

#ifdef __cplusplus
extern "C" {
#endif

// animal enums used for requesting
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

// main animality response type
typedef struct {
    an_type_t type;
    char * name;
    char * image_url;
    char * fact;
} animal_t;

// exported functions
void an_get(const an_type_t _t, animal_t * _out);
void an_cleanup(animal_t * _tr);

#ifdef __cplusplus
}
#endif // end extern "C"

#endif // ANIMALITY_H