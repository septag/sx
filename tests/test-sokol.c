#include <stdio.h>
#include "sx/app.h"
#include "sx/gfx.h"

sg_pass_action pass_action;

void init(void) {
    const sx_alloc* alloc = sx_alloc_malloc_leak_detect;
    sg_desc desc;
    memset(&desc, 0x0, sizeof(desc));
    desc.mtl_device = sapp_metal_get_device();
    desc.mtl_renderpass_descriptor_cb = sapp_metal_get_renderpass_descriptor;
    desc.mtl_drawable_cb = sapp_metal_get_drawable;
    desc.d3d11_device = sapp_d3d11_get_device();
    desc.d3d11_device_context = sapp_d3d11_get_device_context();
    desc.d3d11_render_target_view_cb = sapp_d3d11_get_render_target_view;
    desc.d3d11_depth_stencil_view_cb = sapp_d3d11_get_depth_stencil_view;

    memset(&pass_action, 0x0, sizeof(pass_action));
    pass_action.colors[0] = (sg_color_attachment_action){SG_ACTION_CLEAR, {1.0f, 0.0f, 0.0f, 1.0f}};

    sg_setup_alloc(alloc, &desc);
}

void frame(void) {
    float g = pass_action.colors[0].val[1] + 0.01f;
    pass_action.colors[0].val[1] = (g > 1.0f) ? 0.0f : g;
    sg_begin_default_pass(&pass_action, sapp_width(), sapp_height());
    sg_end_pass();
    sg_commit();
}

void cleanup(void) {
    sg_shutdown();
}

sapp_desc sokol_main(int argc, char* argv[])
{
    sapp_desc desc;
    memset(&desc, 0x0, sizeof(desc));
    desc.init_cb = init;
    desc.frame_cb = frame;
    desc.cleanup_cb = cleanup;
    desc.width = 400;
    desc.height = 300;
    desc.window_title = "Test App";
    return desc;
}


