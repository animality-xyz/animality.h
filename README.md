# animality.h
C/C++ library for requesting to the [animality API.](https://animality.xyz).

Required dependencies:
- [`libcurl`](https://github.com/curl/curl) for sending HTTPS requests.
- [`cJSON`](https://github.com/DaveGamble/cJSON) for parsing the JSON response.

```sh
$ git clone https://github.com/animality-xyz/animality.h.git
$ cd animality.h/
$ gcc -c animality.c -lcurl -ljson -o animality.o

# ...do stuff with object file
```

The response from requesting to the library is this struct.
```c
typedef struct {
    an_type_t type;    // animal name, as enum
    char * name;       // animal name, as string
    char * image_url;  // animal image URL
    char * fact;       // animal fact
} animal_t;
```

```c
#include <animality.h>

int main() {
    // create our animal struct
    animal_t animal = AN_EMPTY_ANIMAL;

    // request for an animal
    an_get(AN_DOG, &animal);

    // do stuff with struct
    printf("animal image url: %s\n", animal.image_url);
    printf("animal fact: %s\n", animal.fact);
    
    // free animal struct once used
    an_cleanup(&animal);

    // global cleanup
    an_cleanup(NULL);

    return 0;
}
```
