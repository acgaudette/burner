#ifndef BURNER_H
#define BURNER_H

#include "render.h"
#include "input.h"

typedef void (game_start)(State*, Renderer*, void**);
typedef game_start* (game_start_loader)();

typedef Mat4 (game_update)(State*, Renderer*, Input*, double, double, void*);
typedef game_update* (game_update_loader)();

typedef void (game_on_reload)(State*, Renderer*, void**);
typedef game_on_reload* (game_on_reload_loader)();


// forward declare these functions so the game code need not include
// reload getters
void start(State*, Renderer*, void**);
Mat4 update(
    State*,
    Renderer*,
    Input*,
    double,
    double,
    void*
);
void on_reload(State*, Renderer*, void**);

#define PASTE2_INTERAL__(x, y) x##y
#define PASTE_INTERNAL__(x, y) PASTE2_INTERAL__(x, y)
#include "../out/version.h"

#if defined(DEVELOPMENT_MODE)
    #pragma message("DEV MODE")
    #define VERSIONED_PROC(name) PASTE_INTERNAL__(name, LIB_VERSION)
#else
    #pragma message("RELEASE MODE")
    #define VERSIONED_PROC(name) name
#endif

#if defined(GAME_CODE)

#ifdef __cplusplus
extern "C" {
#endif
    game_start *VERSIONED_PROC(get_start)()
    {
        return (game_start*)(&start);
    }

    game_update *VERSIONED_PROC(get_update)()
    {
        return (game_update*)(&update);
    }

    game_on_reload *VERSIONED_PROC(get_on_reload)()
    {
        return (game_on_reload*)(&on_reload);
    }

#ifdef __cplusplus
}
#endif

#endif

#undef PASTE_INTERNAL__
#undef PASTE2_INTERNAL__

struct Engine {
    
    // TODO local fields
    int run(const char * const src_name, void* custom_data = nullptr, size_t custom_data_size = 0);
};

#endif
