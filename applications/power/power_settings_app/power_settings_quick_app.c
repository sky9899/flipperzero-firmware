#include "power_settings_app.h"

static bool power_settings_quick_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    PowerSettingsApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool power_settings_quick_back_event_callback(void* context) {
    furi_assert(context);
    PowerSettingsApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void power_settings_quick_tick_event_callback(void* context) {
    furi_assert(context);
    PowerSettingsApp* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

PowerSettingsApp* power_settings_quick_app_alloc() {
    PowerSettingsApp* app = malloc(sizeof(PowerSettingsApp));

    // Records
    app->gui = furi_record_open("gui");
    app->power = furi_record_open("power");

    // View dispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&power_settings_scene_handlers, app);
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, power_settings_quick_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, power_settings_quick_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, power_settings_quick_tick_event_callback, 2000);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // Views
    app->batery_info = battery_info_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        PowerSettingsAppViewBatteryInfo,
        battery_info_get_view(app->batery_info));
        
    // Set first scene
    scene_manager_next_scene(app->scene_manager, PowerSettingsAppViewBatteryInfo);
    return app;
}

void power_settings_quick_app_free(PowerSettingsApp* app) {
    furi_assert(app);
    // Views
    view_dispatcher_remove_view(app->view_dispatcher, PowerSettingsAppViewBatteryInfo);
    battery_info_free(app->batery_info);
    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);
    // Records
    furi_record_close("power");
    furi_record_close("gui");
    free(app);
}

int32_t power_settings_quick_app(void* p) {
    PowerSettingsApp* app = power_settings_quick_app_alloc();
    view_dispatcher_run(app->view_dispatcher);
    power_settings_quick_app_free(app);
    return 0;
}
