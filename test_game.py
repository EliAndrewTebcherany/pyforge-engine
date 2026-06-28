import pyforge
import random
import os

# =====================================================================
# 🚀 ENGINE CORE INITIALIZATION
# =====================================================================
SCREEN_WIDTH = 1024
SCREEN_HEIGHT = 768
pyforge.init(SCREEN_WIDTH, SCREEN_HEIGHT, "Pyforge Engine v0.2 - Ultimate Master Sandbox")

# Load fileless cross-platform font engine mapping systems in RAM
pyforge.load_system_font("sans-serif", font_size=26)

# 🎵 AUTOMATED AUDIO BOOTSTRAP PIPELINE
# Drop any music file named background.wav or background.mp3 into your folder!
if os.path.exists("background.mp3"):
    pyforge.play_music("background.mp3", loop=True)
elif os.path.exists("background.wav"):
    pyforge.play_music("background.wav", loop=True)
elif os.path.exists("my_music.wav"):
    pyforge.play_music("my_music.wav", loop=True)
else:
    print("💡 Note: Put an MP3 or WAV music track in this folder to hear ambient sound!")

# Safe references for short audio collision pop sounds
SFX_POINT = "my_music.wav" if os.path.exists("my_music.wav") else None
SFX_CRASH = "my_music.wav" if os.path.exists("my_music.wav") else None

# Setup game meshes
block_mesh = pyforge.shape(4)
GRID_SIZE = 32

# =====================================================================
# ⚙️ ENGINE SYSTEM STATE MATRIX
# =====================================================================
game_state = "MAIN_MENU" # Choices: "MAIN_MENU", "SNAKE_PLAYING"

# Snake Logic Parameters
snake_segments = [(512, 384), (480, 384), (448, 384)]
current_direction = (GRID_SIZE, 0)
next_direction = (GRID_SIZE, 0)

# Target Apple coordinates
apple_x = random.randint(2, (SCREEN_WIDTH // GRID_SIZE) - 3) * GRID_SIZE
apple_y = random.randint(3, (SCREEN_HEIGHT // GRID_SIZE) - 3) * GRID_SIZE

# Frame delays to balance fast C speeds
move_timer = 0
move_delay = 7

# Stats
running_score = 0
best_score = 0
start_btn_color = (0.2, 0.25, 0.3)

# =====================================================================
# 🔄 MASTER HARDWARE GRAPHICS & INPUT RENDERING LOOP
# =====================================================================
while pyforge.is_open():
    # Paint smooth slate cyber blue vertical linear background gradient
    pyforge.clear_gradient(top_color=(0.04, 0.05, 0.12), bottom_color=(0.1, 0.12, 0.22))
    mx, my = pyforge.get_mouse_pos()

    # -----------------------------------------------------------------
    # STATE 1: THE INTERACTIVE MAIN MENU SCREEN
    # -----------------------------------------------------------------
    if game_state == "MAIN_MENU":
        # Render a dark dashboard banner title card box panel
        pyforge.draw_button(212, 180, 600, 100, bg_color=(0.02, 0.02, 0.05))
        pyforge.draw_text("PYFORGE CORE ENGINE V0.2", x=275, y=212, scale=0.6, color=(0.4, 0.7, 1.0))
        
        # Interactive play option button perimeter boundaries coordinates
        btn_x, btn_y, btn_w, btn_h = 412, 450, 200, 60
        
        # Hover boundary checking intersection tests
        if btn_x <= mx <= (btn_x + btn_w) and btn_y <= my <= (btn_y + btn_h):
            start_btn_color = (0.1, 0.6, 0.3) # Highlight green on hover
            if pyforge.is_button_clicked(btn_x, btn_y, btn_w, btn_h):
                # PARTICLES: Blast beautiful cyan shards right under your clicking mouse cursor!
                pyforge.spawn_particles(mx, my, color=(0.4, 0.7, 1.0))
                print("🟢 START TRIGGERED! Switching engine tracking matrices...")
                game_state = "SNAKE_PLAYING"
        else:
            start_btn_color = (0.2, 0.25, 0.3)

        # Paint interface layers
        pyforge.draw_button(btn_x, btn_y, btn_w, btn_h, bg_color=start_btn_color)
        pyforge.draw_text("START", x=472, y=464, scale=0.6, color=(1.0, 1.0, 1.0))

    # -----------------------------------------------------------------
    # STATE 2: ACTIVE RETRO SNAKE RUNTIME PHYSICS
    # -----------------------------------------------------------------
    elif game_state == "SNAKE_PLAYING":
        # --- DIRECTION VECTOR TRACKING ---
        if (pyforge.is_key_pressed(pyforge.KEY_W) or pyforge.is_key_pressed(pyforge.KEY_UP)) and current_direction[1] == 0:
            next_direction = (0, -GRID_SIZE)
        if (pyforge.is_key_pressed(pyforge.KEY_S) or pyforge.is_key_pressed(pyforge.KEY_DOWN)) and current_direction[1] == 0:
            next_direction = (0, GRID_SIZE)
        if (pyforge.is_key_pressed(pyforge.KEY_A) or pyforge.is_key_pressed(pyforge.KEY_LEFT)) and current_direction[0] == 0:
            next_direction = (-GRID_SIZE, 0)
        if (pyforge.is_key_pressed(pyforge.KEY_D) or pyforge.is_key_pressed(pyforge.KEY_RIGHT)) and current_direction[0] == 0:
            next_direction = (GRID_SIZE, 0)

        # --- GRID POSITION MOVEMENT REGULATOR ---
        move_timer += 1
        if move_timer >= move_delay:
            move_timer = 0
            current_direction = next_direction

            head_x = snake_segments[0][0] + current_direction[0]
            head_y = snake_segments[0][1] + current_direction[1]
            new_head = (head_x, head_y)

            # --- BOUNDING BOUNDARY COLLISION MATH CHECKS ---
            is_dead = False
            if head_x < GRID_SIZE or head_x >= SCREEN_WIDTH - GRID_SIZE or head_y < 80 or head_y >= SCREEN_HEIGHT - GRID_SIZE:
                is_dead = True
            if new_head in snake_segments:
                is_dead = True

            if is_dead:
                # PARTICLES: Scatter red dust shards everywhere upon crash impact lines!
                pyforge.spawn_particles(snake_segments[0][0] + 16, snake_segments[0][1] + 16, color=(0.9, 0.2, 0.2))
                if SFX_CRASH: pyforge.play_sound(SFX_CRASH)
                
                print("💥 Crash Detected! Teleporting player to main menu dashboard...")
                if running_score > best_score: best_score = running_score
                running_score = 0
                move_delay = 7
                snake_segments = [(512, 384), (480, 384), (448, 384)]
                current_direction = (GRID_SIZE, 0)
                next_direction = (GRID_SIZE, 0)
                game_state = "MAIN_MENU" # Return to main menu on death sequence
                continue

            snake_segments.insert(0, new_head)

            # --- APPLE EATING DETECTOR ---
            if head_x == apple_x and head_y == apple_y:
                running_score += 1
                pyforge.spawn_particles(apple_x + 16, apple_y + 16, color=(0.2, 0.9, 0.4))
                if SFX_POINT: pyforge.play_sound(SFX_POINT)
                
                if running_score % 2 == 0 and move_delay > 2:
                    move_delay -= 1

                while True:
                    apple_x = random.randint(2, (SCREEN_WIDTH // GRID_SIZE) - 3) * GRID_SIZE
                    apple_y = random.randint(3, (SCREEN_HEIGHT // GRID_SIZE) - 3) * GRID_SIZE
                    if (apple_x, apple_y) not in snake_segments:
                        break
            else:
                snake_segments.pop()

        # --- RENDERING SNAKE GRAPHICS ENGINE ---
        # Draw Border bounding lines walls
        pyforge.draw_button(10, 74, SCREEN_WIDTH - 20, 10, bg_color=(0.2, 0.25, 0.35))
        pyforge.draw_button(10, SCREEN_HEIGHT - 20, SCREEN_WIDTH - 20, 10, bg_color=(0.2, 0.25, 0.35))
        pyforge.draw_button(10, 74, 10, SCREEN_HEIGHT - 84, bg_color=(0.2, 0.25, 0.35))
        pyforge.draw_button(SCREEN_WIDTH - 20, 74, 10, SCREEN_HEIGHT - 84, bg_color=(0.2, 0.25, 0.35))

        # Red target apple block item
        pyforge.drawshape(block_mesh, x=apple_x + 16, y=apple_y + 16, size=14, angle=45.0, color=(0.9, 0.2, 0.2))

        # Snake block nodes chains arrays
        for idx, segment in enumerate(snake_segments):
            color = (0.3, 0.9, 0.4) if idx == 0 else (0.2, 0.7, 0.3)
            pyforge.drawshape(block_mesh, x=segment[0] + 16, y=segment[1] + 16, size=15, angle=45.0, color=color)

        # Upper HUD Dashboard banner display panels overlay
        pyforge.draw_button(10, 10, 1004, 55, bg_color=(0.02, 0.02, 0.05))
        pyforge.draw_text(f"SCORE: {running_score}", x=40, y=22, scale=0.55, color=(0.4, 0.7, 1.0))
        pyforge.draw_text(f"BEST SCORE: {best_score}", x=740, y=22, scale=0.55, color=(1.0, 0.85, 0.1))

    
        # ⚡ NEW: Live Performance hardware track counter!
        pyforge.draw_text(f"FPS: {pyforge.get_fps()}", x=420, y=22, scale=0.55, color=(0.2, 0.9, 0.4))


    # --- FLUSH ACTIVE VISUAL EFFECT MATRIX PIPELINES ---
    pyforge.update_effects()
    pyforge.refresh()
