# animality.h
A simple API wrapper that generates images & facts of any animal. Tested on Windows (Visual C++ and MinGW) and Linux (GCC).

Required dependencies:
- [`libcurl`](https://github.com/curl/curl) (Linux only)
- `pthreads` (Linux only, might be already installed by default)
- `wininet` (Windows only, might be already installed by default)

# Building the Library
### Windows (MinGW) or Linux
```sh
git clone https://github.com/animality-xyz/animality.h.git && cd animality.h/
gcc -c animality.c -o animality.o
ar rcs -o libanimal.a animality.o
```
### Windows (Visual C++)
```bat
git clone https://github.com/animality-xyz/animality.h.git && cd animality.h
cl /LD animality.c
```

# Example

The response from requesting to the library is this struct.
```c
typedef struct {
    an_type_t type;    // animal name, as enum
    char * name;       // animal name, as string
    char * image_url;  // animal image URL
    char * fact;       // animal fact
} animal_t;
```
Here is a simple request example to the API. Please note that the following example is synchronous (aka blocking).
```c
#include "animality.h"

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
If you want an asynchronous request, try this example:
```c
#include "animality.h"

// our request callback function!
void callback(const animal_t * animal) {
    printf("animal image url: %s\n", animal->image_url);
}

int main() {
    // create separate thread for requesting to the API
    const an_thread_t thr = an_get_async(AN_DOG, &callback);

    // this will run while the thread is still requesting to the API
    printf("Hello, World!\n");

    // wait for thread to terminate before we exit the main function
    an_thread_wait(thr);
    
    // global cleanup
    an_cleanup(NULL);
    
    return 0;
}
```