from .engine import (
    init, shape, drawshape, clear_gradient, is_open, refresh,
    is_key_pressed, load_image, draw_button, draw_text, load_system_font,
    get_mouse_pos, is_mouse_pressed, is_button_clicked, MOUSE_LEFT,
    play_sound, play_music, 
    spawn_particles, update_effects,
    get_fps,
    KEY_W, KEY_A, KEY_S, KEY_D, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_SPACE
)

from .vectors import PhysicsGroup


def phygroup(*entities):
    """Creates an interactive vector force solver group for shapes."""
    return PhysicsGroup(*entities)
