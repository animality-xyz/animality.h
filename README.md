# animality-h
A simple API wrapper that generates images & facts of any animal

Required dependencies:
- [`libcurl`](https://github.com/curl/curl) for sending HTTPS requests.
- [`cJSON`](https://github.com/DaveGamble/cJSON) for parsing the JSON response.

# Installation

```sh
$ git clone https://github.com/animality-xyz/animality.h.git
$ cd animality.h/
$ gcc -c animality.c -lcurl -ljson -o animality.o

# ...do stuff with object file
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

## Node.js Addon
For linux node.js users, good news! Because you can create a fast Node.js wrapper on this Library.
**Preparing**
```bash
$ git clone https://github.com/animality-xyz/animality.h.git    # Clone the repo
$ cd animality.h/
$ mkdir deps                                                    # Create a directory for all of our header files
```

#### 1. Installing dependencies
Make sure you installed all of it's dependencies first.

- First, install and build `cJSON` (skip if you done this)
```bash
$ git clone https://github.com/DaveGamble/cJSON.git   # Clone repo for cJSON
$ cd cJSON/
$ gcc -c cJSON.c -o cJSON.o                           # Compile the main source to an object file
$ ar rcs -o libjson.a cJSON.o                         # Turn it to a static library file
$ cp libjson.a path/to/animality/                     # Copy static library to the root directory. Name must be `libjson.a`
```
- Next, install `libcurl` (skip if you done this)
```bash
$ sudo apt-get install libcurl4-openssl-dev   # Install libcurl
```

#### 2. Copy all header files to `deps/` directory
Make sure you copied all of the header files to the <root>/deps/ directory.
```bash
$ cp path/to/cJSON-clone/cJSON.h path/to/animality/deps/
$ cp path/to/libcurl/include/*.h path/to/animality/deps/
```

#### 3. Setting up node addon API
- First, install the node modules it requires.
```bash
$ cd path/to/animality
$ npm i -g node-gyp         # Install node-gyp
$ npm i -g node-addon-api   # Install node addon API
```
- Secondly, move all of the header files from the node addon API path to the root directory.
```bash
$ node -p "require('node-addon-api').include_dir"    # Get include path for node-addon-api
/some/path/here
$ mv /some/path/here/*.h .                           # Move all of the header files to the current dir
```

#### 4. Building the entire thing
- First, build the main file.
```bash
$ gcc -c animality.c -D _AN_NODE_ADDON -o animality.o     # Compile the main C source file to an object file
$ ar rcs -o libanimal.a animality.o                       # Convert the object file to a static library. Must be "libanimal.a".
```
- Secondly, we build the entire thing with this;
```bash
$ node-gyp configure
$ node-gyp build
```

#### Enjoy!
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
