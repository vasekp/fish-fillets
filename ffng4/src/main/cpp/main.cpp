#include "instance.h"
#include "subsystem/graphics.h"
#include "subsystem/audio.h"

void GLRectangle(float x, float y, float w, float h) {
    float coords[4][2] = {
        {x, y},
        {x + w, y},
        {x, y + h},
        {x + w, y + h}};
    glVertexAttribPointer(ogl::Program::aPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), &coords[0][0]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

static void draw_frame(struct Instance* instance) {
    if (!instance->graphics)
        return;

    const auto* display = instance->graphics->display();
    const auto* canvas = instance->graphics->canvas();
    const auto* image = instance->bg.get();

    instance->graphics->setCanvasSize(image->width(), image->height());
    canvas->bind();
    image->texture().bind();

    {
        auto& program = instance->graphics->shaders()->copy;
        glUseProgram(program);
        glUniform1i(program.uniform("uTexture"), Shaders::texImage_shader);
        glUniform2f(program.uniform("uSrcSize"), (float) image->width(), (float) image->height());
        glUniform2f(program.uniform("uDstSize"), (float) canvas->width(), (float) canvas->height());
        glUniform2f(program.uniform("uSrcOffset"), 0.f, 0.f);
        glUniform2f(program.uniform("uDstOffset"), 0.f, 0.f);

        GLRectangle(0, 0, (float) image->width(), (float) image->height());
    }

    display->bind();
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    {
        auto& program = instance->graphics->shaders()->fill;
        glUseProgram(program);
        auto fsw = (float)canvas->width();
        auto fsh = (float)canvas->height();
        auto fdw = (float)display->width();
        auto fdh = (float)display->height();
        glUniform1i(program.uniform("uScreenTexture"), Shaders::texCanvas_shader);
        glUniform2f(program.uniform("uDisplaySize"), fdw, fdh);
        glUniform2f(program.uniform("uCanvasSize"), fsw, fsh);
        glUniform2f(program.uniform("uTextureSize"), (float) canvas->texWidth(), (float) canvas->texHeight());

        float scale = std::min(fdw / fsw, fdh / fsh);
        glViewport((int)(fdw - scale * fsw) / 2,
                   (int)(fdh - scale * fsh) / 2,
                   (int)(scale * fsw), (int)(scale * fsh));

        GLRectangle(-1.f, -1.f, 2.f, 2.f);
    }
    display->swap();
}

static int32_t handle_input(struct android_app* app, AInputEvent* event) {
    //auto* instance = (struct Instance*)app->userData;
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        //instance->live = true;
        return 1;
    }
    return 0;
}

static void handle_cmd(struct android_app* app, int32_t cmd) {
    auto* instance = (struct Instance*)app->userData;
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            instance->app->savedState = malloc(sizeof(struct saved_state));
            *((struct saved_state*)instance->app->savedState) = instance->state;
            instance->app->savedStateSize = sizeof(struct saved_state);
            break;
        case APP_CMD_INIT_WINDOW:
            if (instance->app->window != nullptr) {
                instance->graphics->activate();
                instance->audio->activate();
                instance->bg->reload(instance);
                instance->live = true;
                draw_frame(instance);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            instance->graphics->shutdown();
            instance->audio->shutdown();
            instance->live = false;
            break;
        case APP_CMD_GAINED_FOCUS:
            break;
        case APP_CMD_LOST_FOCUS:
            instance->live = false;
            draw_frame(instance);
            break;
        default:
            break;
    }
}

void android_main(struct android_app* app) {
    auto instance = std::make_unique<Instance>(app);

    app->userData = instance.get();
    app->onAppCmd = handle_cmd;
    app->onInputEvent = handle_input;
    instance->app = app;

    instance->bg = std::make_unique<Image>("orig/map.png");
    instance->audio->addSource(instance->audio->loadAudio("music/menu.ogg"));

    if (app->savedState != nullptr)
        instance->state = *(struct saved_state*)app->savedState;

    while (true) {
        int ident;
        int events;
        struct android_poll_source* source;

        while ((ident=ALooper_pollAll(instance->live ? 0 : -1, nullptr, &events,
                                      (void**)&source)) >= 0) {
            if (source != nullptr)
                source->process(app, source);
            if (ident == LOOPER_ID_USER) {
                // sensors...
            }
            if (app->destroyRequested != 0)
                return;
        }

        if (instance->live) {
            draw_frame(instance.get());
        }
    }
}