# animality.h
A simple API wrapper that generates images & facts of any animal

Required dependencies:
- [`libcurl`](https://github.com/curl/curl) for sending HTTPS requests.
- `pthreads` for threads support (LINUX/POSIX devices only)

# Installation
```sh
$ git clone https://github.com/animality-xyz/animality.h.git && cd animality.h/
$ gcc -c animality.c -o animality.o
$ ar rcs -o libanimal.a animality.o
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
> For LINUX/POSIX users, add `-lpthread` to the compiler flags in order to compile the example below.

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

## Node.js Addon
For linux node.js users, good news! Because you can create a fast Node.js wrapper on this library. To build it:
```bash
$ git clone https://github.com/animality-xyz/animality.h.git && cd animality.h/ && mkdir deps
$ sudo npm i node-gyp && npm init -y && sudo npm i node-addon-api
$ mv ./node_modules/node-addon-api/*.h ./deps
$ gcc -c animality.c -o animality.o
$ ar rcs -o libanimal.a animality.o && mv libanimal.a ./deps
$ node-gyp configure && node-gyp build
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
