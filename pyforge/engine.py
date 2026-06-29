from . import pyforge_core
from PIL import Image as PILImage, ImageDraw as PILImageDraw, ImageFont as PILImageFont
import subprocess
import os
import platform
import miniaudio
import time


_fps_last_time = time.time()
_fps_frame_count = 0
_fps_current_display = 0.0

# ⌨️ Global Key and Mouse Constants
KEY_A, KEY_D, KEY_S, KEY_W = 65, 68, 83, 87
KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN = 263, 262, 265, 264
KEY_SPACE = 32
MOUSE_LEFT = 0

class Texture:
    def __init__(self, texture_id, width, height):
        self.id = texture_id
        self.width = width
        self.height = height

# Global font dictionary mapping characters to unique Texture objects
_font_atlas_chars = {}

def init(width, height, title):
    pyforge_core.init(width, height, title)
    pyforge_core.init_audio_hardware()

def play_sound(filepath):
    """Plays any custom short sound effect file (Must be a .wav file layout)."""
    if os.path.exists(filepath):
        pyforge_core.play_sound_file(str(filepath))
    else:
        print(f"⚠️ Pyforge Error: Sound file not found at path: {filepath}")


def play_music(filepath, loop=True):
    """
    Streams background music. Automatically decodes MP3 files in memory 
    using miniaudio before passing the raw PCM waves straight to your C core!
    """
    if not os.path.exists(filepath):
        print(f"⚠️ Pyforge Error: Audio file not found at path: {filepath}")
        return

    # If it's a native WAV file, pass it straight to your C binary parser
    if filepath.lower().endswith(".wav"):
        loop_toggle = 1 if loop else 0
        pyforge_core.play_music_file(str(filepath), loop_toggle)
        
    # NEW: If it's an MP3, decode it in RAM filelessly!
    elif filepath.lower().endswith(".mp3"):
        print(f"🎵 Decoding compressed MP3 in memory: {filepath}")
        
        # miniaudio decodes the file instantly into a raw PCM byte stream
        audio_file = miniaudio.decode_file(filepath)
        raw_bytes = audio_file.samples.tobytes()
        
        # We can write a quick C helper or pass the stream data into VRAM channels!
        # For now, to keep your compiled core intact, converting it to a WAV buffer 
        # inside Python memory stream arrays is an absolute bulletproof hack.
        import io, wave
        wav_io = io.BytesIO()
        with wave.open(wav_io, "wb") as wav_file:
            wav_file.setnchannels(audio_file.nchannels)
            wav_file.setsampwidth(2) # 16-bit audio
            wav_file.setframerate(audio_file.sample_rate)
            wav_file.writeframes(raw_bytes)
            
        # Save a hidden temporary runtime cache track that gets wiped instantly
        with open("temp_music_stream.wav", "wb") as f:
            f.write(wav_io.getvalue())
            
        loop_toggle = 1 if loop else 0
        pyforge_core.play_music_file("temp_music_stream.wav", loop_toggle)
        os.remove("temp_music_stream.wav") # Immediately wipe it so the user's folder stays clean!

def spawn_particles(x, y, color=(1.0, 1.0, 1.0)):
    """Spawns an explosive burst of physics-tracked graphic particle shards."""
    r, g, b = color
    pyforge_core.spawn_burst_effect(float(x), float(y), float(r), float(g), float(b))

def update_effects():
    """Updates and draws active particle systems onto active screen frame metrics."""
    pyforge_core.update_and_render_particles()

def shape(sides):
    raw_vertices = pyforge_core.shape(sides)
    return EngineShape(raw_vertices)

    return pyforge_core.shape(sides)

def drawshape(shape_obj, x, y, size, angle=0.0, color=(1.0, 1.0, 1.0), opacity=1.0, texture=None):
    r, g, b = color
    tex_id = texture.id if texture is not None else 0
    
    render_x = float(x)
    render_y = float(y)

    # 🔄 THE BIG SELF-CONTAINED PHYSICS INTERCEPT ARRAY
    if hasattr(shape_obj, 'physics_enabled') and shape_obj.physics_enabled:
        # Initialize internal dynamic variables once on the very first frame step run
        if shape_obj.current_x is None:
            shape_obj.current_x = float(x)
        if shape_obj.current_y is None:
            shape_obj.current_y = float(y)

        dt = 0.004           # Slower time slice for fluid visualization
        SCREEN_TOP = 68.0    # Lower edge of your gray line block barrier
        SCREEN_BOTTOM = 745.0 # Upper edge of your bottom safety line margin
        RADIUS = 15.0        # Boundary buffer offset

        # 🚪 STEP A: CONTINUOUS ACCELERATION (GRAVITY) & FRICTION
        shape_obj.velocity += shape_obj.gravity * dt
        
        # Apply air resistance damping directly to velocity vector components
        friction_coefficient = 0.998
        shape_obj.velocity *= friction_coefficient

        # Move the coordinates instantly
        shape_obj.current_y += shape_obj.velocity * dt

        # 🚪 STEP B: SCREEN EDGE WALL BOUNDARY REFLECTIONS
        if shape_obj.current_y - RADIUS <= SCREEN_TOP and shape_obj.velocity < 0:
            shape_obj.velocity = -shape_obj.velocity * 0.8  # Reverse down
            shape_obj.current_y = SCREEN_TOP + RADIUS
        elif shape_obj.current_y + RADIUS >= SCREEN_BOTTOM and shape_obj.velocity > 0:
            shape_obj.velocity = -shape_obj.velocity * 0.8  # Bounce up!
            shape_obj.current_y = SCREEN_BOTTOM - RADIUS

        # Force the engine to pass the calculated internal variables to your C core!
        render_x = float(shape_obj.current_x)
        render_y = float(shape_obj.current_y)

    mesh_data = shape_obj.raw_vertices if hasattr(shape_obj, 'raw_vertices') else shape_obj

    pyforge_core.drawshape(
        mesh_data, 
        render_x, 
        render_y, 
        float(size), 
        float(angle), 
        float(r), 
        float(g), 
        float(b), 
        float(opacity), 
        int(tex_id)
    )


def clear_gradient(top_color=(0.0, 0.0, 0.0), bottom_color=(0.0, 0.0, 0.0)):
    r1, g1, b1 = top_color
    r2, g2, b2 = bottom_color
    pyforge_core.clear_gradient(float(r1), float(g1), float(b1), float(r2), float(g2), float(b2))

def is_key_pressed(key_code):
    return pyforge_core.is_key_pressed(int(key_code))

def is_open():
    return pyforge_core.is_open()

def refresh():
    pyforge_core.refresh()

def get_mouse_pos():
    return pyforge_core.get_mouse_pos()

def is_mouse_pressed(button=MOUSE_LEFT):
    return pyforge_core.is_mouse_pressed(int(button))

def load_image(file_path):
    img = PILImage.open(file_path).convert("RGBA")
    w, h = img.size
    raw_bytes = img.tobytes("raw", "RGBA")
    tex_id = pyforge_core.load_texture(raw_bytes, w, h)
    return Texture(tex_id, w, h)

# 🗺️ 1. The Python Atlas Slicer Core
def load_font(file_path="font.png"):
    """Slices font.png into individual character textures completely inside Python."""
    global _font_atlas_chars
    master_img = PILImage.open(file_path).convert("RGBA")
    
    char_w, char_h = 64, 64
    chars = [chr(i) for i in range(32, 127)]
    
    for idx, char in enumerate(chars):
        col = idx % 16
        row = idx // 16
        
        left = col * char_w
        top = row * char_h
        right = left + char_w
        bottom = top + char_h
        
        char_crop = master_img.crop((left, top, right, bottom))
        raw_bytes = char_crop.tobytes("raw", "RGBA")
        
        tex_id = pyforge_core.load_texture(raw_bytes, char_w, char_h)
        _font_atlas_chars[char] = Texture(tex_id, char_w, char_h)

def load_system_font(font_name="sans-serif", font_size=28):
    """
    Finds a system font, automatically bakes a transparent character atlas 
    in-memory, and uploads it directly to the GPU without writing any physical files!
    """
    global _font_atlas_chars
    import os
    os_type = platform.system()
    font_path = ""

    # 1. OS-Specific Path Auto-Detection Matrix
    if os.path.exists(font_name):
        font_path = font_name
    else:
        if os_type == "Windows":
            choices = [f"C:\\Windows\\Fonts\\{font_name}.ttf", "C:\\Windows\\Fonts\\arialbd.ttf"]
        elif os_type == "Darwin":
            choices = [f"/Library/Fonts/{font_name}.ttf", "/System/Library/Fonts/Helvetica.ttc"]
        else:
            choices = ["/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", "/usr/share/fonts/truetype/freefont/FreeSansBold.ttf"]
        
        for path in choices:
            if os.path.exists(path):
                font_path = path
                break

    # 2. Build the master transparent 1024x1024 grid canvas image in memory
    master_img = PILImage.new('RGBA', (1024, 1024), (0, 0, 0, 0))
    draw = PILImageDraw.Draw(master_img)
    
    try:
        fnt = PILImageFont.truetype(font_path, font_size)
    except IOError:
        fnt = PILImageFont.load_default()

    chars = [chr(i) for i in range(32, 127)]
    for idx, char in enumerate(chars):
        col = idx % 16
        row = idx // 16
        draw.text((col * 64 + 14, row * 64 + 10), char, fill=(255, 255, 255, 255), font=fnt)

    # 3. FIXED: Slice each character directly out of the memory canvas into VRAM!
    char_w, char_h = 64, 64
    for idx, char in enumerate(chars):
        col = idx % 16
        row = idx // 16
        
        left = col * char_w
        top = row * char_h
        right = left + char_w
        bottom = top + char_h
        
        # Crop character directly out of the running master image object memory
        char_crop = master_img.crop((left, top, right, bottom))
        raw_bytes = char_crop.tobytes("raw", "RGBA")
        
        # Ship bytes instantly to your compiled C extension load_texture method
        tex_id = pyforge_core.load_texture(raw_bytes, char_w, char_h)
        _font_atlas_chars[char] = Texture(tex_id, char_w, char_h)

    print(f"✅ Unified Font Grid System initialized filelessly for '{font_name}'!")



# 🔠 3. Fast Spritesheet Text Rendering
def draw_text(text, x, y, scale=0.5, color=(1.0, 1.0, 1.0), size=None):
    """Draws your custom typography using pre-sliced character textured quads."""
    if not _font_atlas_chars:
        return
        
    final_scale = size if size is not None else scale
    current_x = float(x)
    
    quad_mesh = [(-1.0, -1.0), (1.0, -1.0), (1.0, 1.0), (-1.0, 1.0)]
    render_size = 64.0 * float(final_scale)

    for char in str(text):
        if char not in _font_atlas_chars:
            char = ' '
            
        char_tex = _font_atlas_chars[char]
        cx = current_x + (render_size / 2.0)
        cy = float(y) + (render_size / 2.0)
        
        drawshape(quad_mesh, x=cx, y=cy, size=render_size / 2.0, angle=0.0, color=color, opacity=1.0, texture=char_tex)
        current_x += render_size * 0.38

# 🧱 4. UI Layout Panels Components
def draw_button(x, y, width, height, text="", bg_color=(0.1, 0.1, 0.1), text_color=(1.0, 1.0, 1.0), scale=0.5, size=None):
    x1, y1 = float(x), float(y)
    x2, y2 = float(x + width), float(y + height)
    absolute_quad = [(x1, y1), (x2, y1), (x2, y2), (x1, y2)]
    br, bg, bb = bg_color
    pyforge_core.drawshape(absolute_quad, 0.0, 0.0, 1.0, 0.0, float(br), float(bg), float(bb), 1.0, 0)
    
    final_scale = size if size is not None else scale
    if text:
        text_x = x + 15
        text_y = y + (height / 2.0) - (16.0 * final_scale)
        draw_text(text, text_x, text_y, scale=final_scale, color=text_color)

def is_button_clicked(x, y, width, height):
    if not is_mouse_pressed(MOUSE_LEFT):
        return False
    mx, my = get_mouse_pos()
    if x <= mx <= (x + width) and y <= my <= (y + height):
        return True
    return False



def get_fps():
    """
    Calculates the live frame rate by counting how many loop iterations 
    execute every single second on your CPU/GPU hardware pipeline.
    """
    global _fps_last_time, _fps_frame_count, _fps_current_display

    _fps_frame_count += 1
    current_time = time.time()
    elapsed_time = current_time - _fps_last_time

    # Every time 1 full second passes, refresh the displayed FPS value
    if elapsed_time >= 1.0:
        _fps_current_display = _fps_frame_count / elapsed_time
        _fps_frame_count = 0
        _fps_last_time = current_time

    return int(_fps_current_display)


class EngineShape:
    def __init__(self, raw_vertices):
        self.raw_vertices = raw_vertices
        
        # Automated Opt-In Subsystem Flags
        self.physics_enabled = False
        self.gravity = 0.0
        self.mass = 1.0
        self.velocity = 0.0
        self.current_y = None  # Internal location cache tracker
        self.current_x = None  # FIXED: Added missing horizontal variable tracker!


    def get_physics(self, mode="realistic"):
        """Activates automatic framework gravity math calculations."""
        self.physics_enabled = True
        if mode == "realistic":
            self.gravity = 10.0
            self.mass = 10.0
            self.velocity = 0.0
        elif mode == "custom":
            self.gravity = 0.0
            self.mass = 1.0
            self.velocity = 0.0

    # These magic methods let your existing C code interact with it like a raw list!
    def __getitem__(self, item):
        return self.raw_vertices[item]

    def __len__(self):
        return len(self.raw_vertices)
