#include "animality.h"

/* ALL available animals */
static const char _animals[ANIMALS_LENGTH][ANIMALS_MAX_EACH_SIZE] = {
    "cat",
    "dog",
    "bird",
    "panda",
    "redpanda",
    "koala",
    "fox",
    "whale",
    "kangaroo",
    "bunny",
    "lion",
    "bear",
    "frog",
    "duck",
    "penguin"
};

/* generates a random animal index */
static unsigned char _generate_index(void) {
    srand(time(NULL));
    return rand() % ANIMALS_LENGTH;
}

/* this is 0 if it's not initiated, otherwise 1 */
static unsigned char _curl_status = 0;

/* ALL used base URLs */
static const char _img_base_url [31] = "https://api.animality.xyz/img/";
static const char _fact_base_url[32] = "https://api.animality.xyz/fact/";

/* our write struct */
typedef struct {
    char * buffer;
    size_t size;
} _an_write_t;

/* the write function to be called by CURL
   (copied from the CURL website) */
static size_t _write_func(void * contents, size_t size, size_t nmemb, void * write_buf) {
    const size_t total_size = size * nmemb;
    _an_write_t * buf = (_an_write_t *)write_buf;
    
    buf->buffer = realloc(buf->buffer, buf->size + total_size + 1);
    memcpy(&(buf->buffer[buf->size]), contents, total_size);
    buf->size += total_size;
    buf->buffer[buf->size] = 0;
    
    return total_size;
}

/* concatenates the base URL with the animal string. */
static char * _parse_url(
    const char * base_url, const unsigned char base_url_size, 
    const char * end_url, const unsigned char size
) {
    
    /* allocate joined (null-terminated) string */
    char * full_url = malloc((base_url_size + size + 1) * sizeof(char));
    full_url[base_url_size + size] = 0;
    
    /* copy both values to the new memory block */
    memcpy(full_url, base_url, base_url_size * sizeof(char));
    memcpy(full_url + base_url_size, end_url, size * sizeof(char));
    
    return full_url;
}

static char * _parse_json(const char * json, const unsigned char start_index) {
    const size_t length = strlen(json);

    char * output = malloc((length - start_index - 2) + sizeof(char));
    output[length - start_index - 2] = 0;

    for (size_t i = start_index; i < length - 2; i++)
        output[i - start_index] = json[i];

    return output;
}

/* performs the HTTPS request to the animality API. */
static void _an_request(char * full_url, _an_write_t * write) {
    /* if it's somehow still not deallocated yet, deallocate it */
    if (write->buffer != NULL)
        free(write->buffer);
    
    /* allocate stuff */
    write->buffer = malloc(sizeof(char));
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
    
    /* deallocate full URL string */
    free(full_url);
}

void an_get(const an_type_t _t, animal_t * _out) {
    /* initiate CURL if it's not initiated */
    if (!_curl_status) {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        _curl_status++;
    }
    
    /* in case the struct is NULL, to prevent seg fault */
    if (_out == NULL)
        _out = malloc(sizeof(animal_t));
    
    /* write the type to the struct */
    _out->type = _t;
    
    /* save the animal order and the animal name length */
    const unsigned char index = _t == AN_RANDOM ? _generate_index() : (unsigned char)_t;
    const unsigned char animal_name_length = strlen(_animals[index]);
    
    /* save the animal name */
    _out->name = malloc((animal_name_length + 1) * sizeof(char));
    _out->name[animal_name_length] = 0;
    memcpy(_out->name, _animals[index], animal_name_length * sizeof(char));
    
    /* concatenate the base URL with the requested animal string */
    char * full_img_url  = _parse_url(_img_base_url,  30, _animals[index], animal_name_length);
    char * full_fact_url = _parse_url(_fact_base_url, 31, _animals[index], animal_name_length);
    
    /* our standard write struct, for storing (temporary) HTTPS responses */
    _an_write_t * write_buffer = malloc(sizeof(_an_write_t));
    write_buffer->buffer = NULL;
    
    /* request the img endpoint */
    _an_request(full_img_url, write_buffer);

    /* retrieve the link attribute from the JSON response */
    _out->image_url = _parse_json(write_buffer->buffer, 9);

    /* free buffer before next request */
    free(write_buffer->buffer);
    write_buffer->buffer = NULL;
    
    /* request the fact endpoint */
    _an_request(full_fact_url, write_buffer);
    
    /* retrieve fact attribute from the JSON response */
    _out->fact = _parse_json(write_buffer->buffer, 9);

    /* free memory allocated */
    free(write_buffer->buffer);
    free(write_buffer);
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

const an_thread_t an_get_async(const an_type_t _t, const an_callback_t cb) {
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

void an_cleanup(animal_t * _tr) {
    /* if the animal is NULL, then perform global cleanup */
    if (_tr == NULL) {
        if (_curl_status) {
            curl_global_cleanup();
            _curl_status--;
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
