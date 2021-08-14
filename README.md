# animality.h
A simple API wrapper that generates images & facts of any animal

Required dependencies:
- [`libcurl`](https://github.com/curl/curl) for sending HTTPS requests.
- [`cJSON`](https://github.com/DaveGamble/cJSON) for parsing the JSON response.

# Installation

```sh
$ git clone https://github.com/animality-xyz/animality.h.git
$ cd animality.h/
$ gcc -c animality.c -o animality.o
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
File: `example.c`
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
Compile the example with:
```bash
$ gcc example.c -lcurl -ljson -lanimal
# "-ljson"   is the static library for "cJSON".
# "-lanimal" is the static library for "animality.h".
# note: add "-lpthread" for async support (POSIX only)
```
## Integrating with pthreads.h to make it asynchronously
If you are using a POSIX operating system and have the `pthreads` library, you can make async calls!
```c
#include "animality.c"

// our request callback function!
void callback(const animal_t * animal) {
    printf("fact: %s\n", animal->fact);
}

int main() {
    // create separate thread for requesting to the API
    const pthread_t thr = an_get_async(AN_DOG, &callback);

    // this will run while the thread is still requesting to the API
    printf("Hello, World!\n");

    // wait for thread to terminate before we exit the main function
    pthread_join(thr, NULL);
    return 0;
}
```

## Node.js Addon
For linux node.js users, good news! Because you can create a fast Node.js wrapper on this Library. To build it, simply run:
```bash
$ git clone https://github.com/animality-xyz/animality.h.git && cd animality.h/
$ sudo ./build_node.sh
```
and that's it!

### Example
```js
// require the library
const library = require("./node_addon");

// request for an animal
library.dog().then(out => console.log(out));

// use the synchronous library (not recommended)
const blockingLibrary = library.blocking();

// request for an animal
const out = blockingLibrary.dog();
console.log(out);
```
