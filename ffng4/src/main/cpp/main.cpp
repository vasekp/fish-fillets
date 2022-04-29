#include "instance.h"
#include "ogl.h"
#include "shaders.h"

#define SQUARE(x, y, w, h) {\
  {(float)(x), (float)(y)}, \
  {(float)(x + w), (float)(y)}, \
  {(float)(x), (float)(y + h)}, \
  {(float)(x + w), (float)(y + h)} \
}
static void draw_frame(struct Instance* instance) {
    if (instance->display == nullptr)
        return;

    auto& display = instance->display;
    auto& program = instance->shaders->copy;
    auto& image = instance->bg;
    glUseProgram(program);

    glBindTexture(GL_TEXTURE_2D, image->texture());
    int uTexture = glGetUniformLocation(program, "uSrcTexture");
    glUniform1i(uTexture, 0);

    auto uSrcSize = glGetUniformLocation(program, "uSrcSize");
    auto uDstSize = glGetUniformLocation(program, "uDstSize");
    auto uSrcOffset = glGetUniformLocation(program, "uSrcOffset");
    auto uDstOffset = glGetUniformLocation(program, "uDstOffset");
    glUniform2f(uSrcSize, (float)image->width(), (float)image->height());
    glUniform2f(uDstSize, (float)display->width, (float)display->height);
    glUniform2f(uSrcOffset, 0.f, 0.f);
    glUniform2f(uDstOffset, 0.f, 0.f);

    float points[4][2] = SQUARE(0.f, 0.f, image->width(), image->height());
    auto aPosition = (GLuint) glGetAttribLocation(program, "aPosition");
    glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), &points[0][0]);
    glEnableVertexAttribArray(aPosition);

    glClearColor(1.0, 0.0, 0.0, 0.5);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

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
                instance->display = std::make_unique<ogl::Display>(instance->app->window);
                instance->shaders = std::make_unique<Shaders>(*instance);
                instance->bg = std::make_unique<ogl::Image>(instance->loadImage("images/crabshow/secret-p.png"));
                draw_frame(instance);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            instance->display.reset();
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
    struct Instance instance{app};

    app->userData = &instance;
    app->onAppCmd = handle_cmd;
    app->onInputEvent = handle_input;
    instance.app = app;

    if (app->savedState != nullptr)
        instance.state = *(struct saved_state*)app->savedState;

    while (true) {
        int ident;
        int events;
        struct android_poll_source* source;

        while ((ident=ALooper_pollAll(instance.live ? 0 : -1, nullptr, &events,
                                      (void**)&source)) >= 0) {
            if (source != nullptr)
                source->process(app, source);
            if (ident == LOOPER_ID_USER) {
                // sensors...
            }
            if (app->destroyRequested != 0)
                return;
        }

        if (instance.live) {
            draw_frame(&instance);
        }
    }
}