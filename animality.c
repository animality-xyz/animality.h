#ifdef _WIN32

/* visual C++ */
#ifdef _MSC_VER
#include <windows.h>
#define AN_EXPORT __declspec(dllexport)
#pragma comment(lib, "wininet.lib")
#else
/* mingw */
#include <winbase.h>
#endif

#include <wininet.h>

static HINTERNET internet = NULL;
static HINTERNET connected = NULL;

typedef DWORD async_cb_ret_t;
#else
#include <pthread.h>
#include <curl/curl.h>

typedef void * async_cb_ret_t;
#endif

#include "animality.h"

/* 
  windows mingw or linux, 
  this would already be set as __declspec(dllexport) if on windows visual C++ 
*/
#ifndef AN_EXPORT
#define AN_EXPORT
#endif

/* base URLs */
#ifdef _WIN32
#define IMG_BASE_URL_LEN    6
#define FACT_BASE_URL_LEN   7

static const char img_base_url[IMG_BASE_URL_LEN]   = "/img/";
static const char fact_base_url[FACT_BASE_URL_LEN] = "/fact/";
#else
#define IMG_BASE_URL_LEN    31
#define FACT_BASE_URL_LEN   32

static const char img_base_url[IMG_BASE_URL_LEN]   = "https://api.animality.xyz/img/";
static const char fact_base_url[FACT_BASE_URL_LEN] = "https://api.animality.xyz/fact/";
#endif

/* ALL available animals */
static const char animals_list[ANIMALS_LENGTH][ANIMALS_MAX_EACH_SIZE] = {
    "cat",
    "dog",
    "bird",
    "panda",
    "redpanda",
    "koala",
    "fox",
    "whale",
    "dolphin",
    "kangaroo",
    "bunny",
    "lion",
    "bear",
    "frog",
    "duck",
    "penguin",
    "axolotl"
};

/* utility function to dynamically push a string to another */
#ifdef _WIN32
static void strpush(char ** str, const char * other, const unsigned long other_len) {
    const unsigned long str_len = strlen(*str);
    
    *str = realloc(*str, str_len + other_len);
    strcpy(*str + str_len, other);
}
#endif

/* generates a random animal index */
static unsigned char generate_index(void) {
    srand(time(NULL));
    return rand() % ANIMALS_LENGTH;
}

static unsigned char initiated = 0;

/* in linux, we need a struct, in windows, we only need a string to write the buffer. */
#ifndef _WIN32
typedef struct {
    char * buffer;
    size_t size;
} an_write_t;

static size_t _write_func(void * contents, size_t size, size_t nmemb, void * write_buf) {
    const size_t total_size = size * nmemb;
    an_write_t * buf = (an_write_t *)write_buf;
    
    buf->buffer = realloc(buf->buffer, buf->size + total_size + 1);
    memcpy(&(buf->buffer[buf->size]), contents, total_size);
    buf->size += total_size;
    buf->buffer[buf->size] = 0;
    
    return total_size;
}
#endif

/* concatenates the base URL with the animal string. */
static char * _parse_url(
    const char * base_url, const unsigned char base_url_size, 
    const char * end_url, const unsigned char size
) {
    /* allocate joined (null-terminated) string */
    char * full_url = malloc(base_url_size + size + 1);
    full_url[base_url_size + size] = 0;
    
    /* copy both values to the new memory block */
    memcpy(full_url, base_url, base_url_size);
    memcpy(full_url + base_url_size, end_url, size);
    
    return full_url;
}

static char * parse_json(const char * json, const unsigned char start_index) {
    const size_t length = strlen(json);

    char * output = malloc((length - start_index - 2) + 1);
    output[length - start_index - 2] = 0;

    for (size_t i = start_index; i < length - 2; i++)
        output[i - start_index] = json[i];

    return output;
}

/* performs the HTTPS request to the animality API. */
#ifdef _WIN32
static char * request(char * full_url)
#else
static void request(char * full_url, an_write_t * write)
#endif
{

#ifdef _WIN32
    HINTERNET req = HttpOpenRequest(connected, "GET", full_url, NULL, NULL, NULL, INTERNET_FLAG_SECURE, 0);
    HttpSendRequestA(req, NULL, 0, NULL, 0);
    char * output = NULL;
    
    while (1) {
        /* check if more data is available */
        DWORD size;
        if (!InternetQueryDataAvailable(req, &size, 0, 0) || !size)
            break;
        
        /* request for the pooped out data */
        char * out = malloc(size + 1);
        if (!InternetReadFile(req, out, size, &size) || !size) {
            free(out);
            break;
        }
        
        /* null terminate it */
        out[size] = 0;
        
        /* first iteration, directly allocate it */
        if (output == NULL) {
            output = malloc(size + 1);
            memcpy(output, out, size + 1);
            continue;
        }
        
        /* push buffer to the total string */
        strpush(&output, out, size);
        free(out);
    }
#else
    /* allocate stuff */
    write->buffer = malloc(1);
    write->size   = 0;
    
    /* setup CURL */
    CURL * curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL,            full_url);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,  _write_func);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA,      (void *)write);
    
    /* send HTTPS request */
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
#endif
    /* deallocate full URL string */
    free(full_url);
#ifdef _WIN32
    InternetCloseHandle(req);
    return output;
#endif
}

AN_EXPORT void an_get(const an_type_t _t, animal_t * _out) {
    /* initiate CURL if it's not initiated */
    if (!initiated) {
#ifdef _WIN32
        internet = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
        connected = InternetConnectA(internet, "api.animality.xyz", 443, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
#else
        curl_global_init(CURL_GLOBAL_DEFAULT);
#endif
        initiated = 1;
    }
    
    /* in case the struct is NULL, to prevent seg fault */
    if (_out == NULL)
        _out = malloc(sizeof(animal_t));
    
    /* write the type to the struct */
    _out->type = _t;
    
    /* save the animal order and the animal name length */
    const unsigned char index = _t == AN_RANDOM ? generate_index() : (unsigned char)_t;
    const unsigned char animal_name_length = strlen(animals_list[index]);
    
    /* save the animal name */
    _out->name = malloc(animal_name_length + 1);
    _out->name[animal_name_length] = 0;
    memcpy(_out->name, animals_list[index], animal_name_length);
    
    /* concatenate the base URL with the requested animal string */
    char * full_img_url  = _parse_url(img_base_url,  IMG_BASE_URL_LEN - 1,  animals_list[index], animal_name_length);
    char * full_fact_url = _parse_url(fact_base_url, FACT_BASE_URL_LEN - 1, animals_list[index], animal_name_length);
    
#ifndef _WIN32
    /* our standard write struct, for storing (temporary) HTTPS responses */
    an_write_t * write_buffer = malloc(sizeof(an_write_t));
    write_buffer->buffer = NULL;
#endif
    
    /* request stuff */
#ifdef _WIN32
    char * img_response = request(full_img_url);
    if (img_response != NULL) {
        _out->image_url = parse_json(img_response, 9);
        free(img_response);
    }
    
    char * fact_response = request(full_fact_url);
    if (fact_response != NULL) {
        _out->fact = parse_json(fact_response, 9);
        free(fact_response);
    }
#else
    request(full_img_url, write_buffer);
    _out->image_url = parse_json(write_buffer->buffer, 9);
    
    free(write_buffer->buffer);
    
    request(full_fact_url, write_buffer);
    _out->fact = parse_json(write_buffer->buffer, 9);

    /* free memory allocated */
    free(write_buffer->buffer);
    free(write_buffer);
#endif
}

AN_EXPORT void an_cleanup(animal_t * _tr) {
    /* if the animal is NULL, then perform global cleanup */
    if (_tr == NULL) {
        if (initiated) {
#ifdef _WIN32
            InternetCloseHandle(internet);
            InternetCloseHandle(connected);
#else
            curl_global_cleanup();
#endif
            initiated = 0;
        }
        
        return;
    }
    
    /* deallocate all values from the struct
       and the struct itself */
    if (_tr->name != NULL) {
        free(_tr->name);
        _tr->name = NULL;
    }

    if (_tr->image_url != NULL) {
        free(_tr->image_url);
        _tr->image_url = NULL;
    }
    
    if (_tr->fact != NULL) {
        free(_tr->fact);
        _tr->fact = NULL;
    }
}

static async_cb_ret_t _an_async_cb(void * ptr) {
    an_thread_arg_t * args = (an_thread_arg_t *)ptr;
    animal_t an = AN_EMPTY_ANIMAL;

    an_get(args->type, &an);
    args->callback(&an);

    an_cleanup(&an);

    free(args);
#ifdef _WIN32
    return 0;
#else
    pthread_exit(NULL);
#endif
}

AN_EXPORT an_thread_t an_get_async(const an_type_t _t, const an_callback_t cb) {
    an_thread_t pt;
    an_thread_arg_t * arg = malloc(sizeof(an_thread_arg_t));
    arg->callback = cb;
    arg->type = _t;

#ifdef _WIN32
    pt = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)_an_async_cb, (void *)arg, 0, NULL);
#else
    pthread_create(&pt, NULL, &_an_async_cb, (void *)arg);
#endif
    return pt;
}
