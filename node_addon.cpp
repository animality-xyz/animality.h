#include "napi.h"
#include "animality.h"

static char * copy_string(const char * str) {
    const size_t size = (strlen(str) + 1) * sizeof(char);
    char * new_str = (char *)malloc(size);
    memcpy(new_str, str, size);
    return new_str;
}

#define FUNC(an_name, mac)                                            \
    Napi::Object an_name(const Napi::CallbackInfo & info) {           \
        const Napi::Env env = info.Env();                             \
                                                                      \
        animal_t animal = AN_EMPTY_ANIMAL;                            \
        an_get(mac, &animal);                                         \
                                                                      \
        Napi::Object output = Napi::Object::New(env);                 \
        output.Set("name", copy_string(animal.name));                 \
        output.Set("image", copy_string(animal.image_url));           \
        output.Set("fact", copy_string(animal.fact));                 \
                                                                      \
        an_cleanup(&animal);                                          \
        return output;                                                \
    }

#define EXPORT(an, name) \
    exports.Set(Napi::String::New(env, name), Napi::Function::New(env, an))

FUNC(dog, AN_DOG)
FUNC(cat, AN_CAT)
FUNC(bird, AN_BIRD)
FUNC(panda, AN_PANDA)
FUNC(redpanda, AN_REDPANDA)
FUNC(koala, AN_KOALA)
FUNC(fox, AN_FOX)
FUNC(whale, AN_WHALE)
FUNC(kangaroo, AN_KANGAROO)
FUNC(bunny, AN_BUNNY)
FUNC(lion, AN_LION)
FUNC(bear, AN_BEAR)
FUNC(frog, AN_FROG)
FUNC(duck, AN_DUCK)
FUNC(penguin, AN_PENGUIN)

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    atexit([]() {
        an_cleanup(nullptr);
    });

    EXPORT(dog, "dog");
    EXPORT(cat, "cat");
    EXPORT(bird, "bird");
    EXPORT(panda, "panda");
    EXPORT(redpanda, "redpanda");
    EXPORT(koala, "koala");
    EXPORT(fox, "fox");
    EXPORT(whale, "whale");
    EXPORT(kangaroo, "kangaroo");
    EXPORT(bunny, "bunny");
    EXPORT(lion, "lion");
    EXPORT(bear, "bear");
    EXPORT(frog, "frog");
    EXPORT(duck, "duck");
    EXPORT(penguin, "penguin");
    
    return exports;
}

NODE_API_MODULE(hello, Init)
