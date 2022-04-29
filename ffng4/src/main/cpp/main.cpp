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
    auto& canvas = instance->canvas;
    auto& image = instance->bg;

    canvas->resize(image->width(), image->height());
    canvas->bind();
    image->bind();

    {
        auto& program = instance->shaders->copy;
        glUseProgram(program);
        auto uTexture = glGetUniformLocation(program, "uSrcTexture");
        auto uSrcSize = glGetUniformLocation(program, "uSrcSize");
        auto uDstSize = glGetUniformLocation(program, "uDstSize");
        auto uSrcOffset = glGetUniformLocation(program, "uSrcOffset");
        auto uDstOffset = glGetUniformLocation(program, "uDstOffset");
        auto aPosition = (GLuint) glGetAttribLocation(program, "aPosition");

        glUniform1i(uTexture, 0);
        glUniform2f(uSrcSize, (float) image->width(), (float) image->height());
        glUniform2f(uDstSize, (float) canvas->width(), (float) canvas->height());
        glUniform2f(uSrcOffset, 0.f, 0.f);
        glUniform2f(uDstOffset, 0.f, 0.f);

        float points[4][2] = SQUARE(0.f, 0.f, image->width(), image->height());
        glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), &points[0][0]);
        glEnableVertexAttribArray(aPosition);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDisableVertexAttribArray(aPosition);
    }

    display->bind();
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    {
        auto& program = instance->shaders->fill;
        glUseProgram(program);
        auto uTexture = glGetUniformLocation(program, "uScreenTexture");
        auto uDisplaySize = glGetUniformLocation(program, "uDisplaySize");
        auto uCanvasSize = glGetUniformLocation(program, "uCanvasSize");
        auto uTextureSize = glGetUniformLocation(program, "uTextureSize");
        auto aPosition = (GLuint) glGetAttribLocation(program, "aPosition");

        auto fsw = (float)canvas->width();
        auto fsh = (float)canvas->height();
        auto fdw = (float)display->width();
        auto fdh = (float)display->height();
        glUniform1i(uTexture, 1);
        glUniform2f(uDisplaySize, fdw, fdh);
        glUniform2f(uCanvasSize, fsw, fsh);
        glUniform2f(uTextureSize, (float) canvas->texWidth(), (float) canvas->texHeight());

        float scale = std::min(fdw / fsw, fdh / fsh);
        glViewport((int)(fdw - scale * fsw) / 2,
                   (int)(fdh - scale * fsh) / 2,
                   (int)(scale * fsw), (int)(scale * fsh));

        float points[4][2] = SQUARE(-1.f, -1.f, 2.f, 2.f);
        glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), &points[0][0]);
        glEnableVertexAttribArray(aPosition);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDisableVertexAttribArray(aPosition);
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
                instance->display = std::make_unique<ogl::Display>(instance->app->window);
                instance->canvas = std::make_unique<ogl::Framebuffer>(1000, 1000);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, instance->canvas->texture());
                glActiveTexture(GL_TEXTURE0);
                instance->shaders = std::make_unique<Shaders>(*instance);
                instance->bg = std::make_unique<ogl::Texture>(instance->loadImage("images/menu/map_lower.png"));
                instance->live = true;
                draw_frame(instance);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            instance->display.reset();
            instance->canvas.reset();
            instance->shaders.reset();
            instance->bg.reset();
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