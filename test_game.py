import pyforge

pyforge.init(1024, 768, "Pyforge v1.0.0 - Full Autonomous Physics Sandbox")
pyforge.load_system_font("sans-serif", font_size=20)

# =====================================================================
# 🔴 TOP BALLS ROW (Moving Downward with Gravity)
# =====================================================================
top_light = pyforge.shape(32)
top_light.get_physics("custom")
top_light.gravity = 150.0  
top_light.mass = 10.0    
top_light.velocity = 0.0   

top_heavy = pyforge.shape(32)
top_heavy.get_physics("custom")
top_heavy.gravity = 150.0  
top_heavy.mass = 5000.0  
top_heavy.velocity = 0.0

top_equal = pyforge.shape(32)
top_equal.get_physics("custom")
top_equal.gravity = 150.0  
top_equal.mass = 500.0   
top_equal.velocity = 0.0

# =====================================================================
# 🔵 BOTTOM BALLS ROW (Moving Upward Natively - Face to Face)
# =====================================================================
# Left: Heavy Bottom Ball (Has gravity, launches up)
bot_heavy = pyforge.shape(32)
bot_heavy.get_physics("custom")
bot_heavy.gravity = 150.0   # Added gravity force!
bot_heavy.mass = 5000.0     
bot_heavy.velocity = -250.0 # Negative speed launches it UP toward the red ball!

# Center: Light Bottom Ball
bot_light = pyforge.shape(32)
bot_light.get_physics("custom")
bot_light.gravity = 150.0   # Added gravity force!
bot_light.mass = 10.0       
bot_light.velocity = -250.0 # Negative speed launches it UP toward the red ball!

# Right: Balanced Bottom Ball
bot_equal = pyforge.shape(32)
bot_equal.get_physics("custom")
bot_equal.gravity = 150.0   # Added gravity force!
bot_equal.mass = 500.0      
bot_equal.velocity = -250.0 # Negative speed launches it UP toward the red ball!


# =====================================================================
# 👥 REGISTER ALL BALLS INTO YOUR INTERACTION GROUP
# =====================================================================
physics_manager = pyforge.phygroup(
    top_light, top_heavy, top_equal, 
    bot_heavy, bot_light, bot_equal
)

while pyforge.is_open():
    # Clear screen with your deep sapphire-to-midnight purple gradient
    pyforge.clear_gradient((0.03, 0.06, 0.16), (0.12, 0.04, 0.18))

    # Run the mass collision solver loop
    physics_manager.update_interactions(1024, 768)
    pyforge.update_effects()

    # Draw visible dynamic gray bounding lines layout vectors box
    pyforge.draw_button(10, 68, 1004, 2, bg_color=(0.25, 0.3, 0.45))
    pyforge.draw_button(10, 745, 1004, 2, bg_color=(0.25, 0.3, 0.45))

    # --- DRAW THE SHAPES ---
    # Pass your target baseline starting grid positions directly!
    pyforge.drawshape(top_light, x=256, y=150, size=30, color=(1.0, 0.35, 0.35))
    pyforge.drawshape(top_heavy, x=512, y=150, size=30, color=(1.0, 0.35, 0.35))
    pyforge.drawshape(top_equal, x=768, y=150, size=30, color=(1.0, 0.35, 0.35))

    pyforge.drawshape(bot_heavy, x=256, y=550, size=30, color=(0.2, 0.65, 1.0))
    pyforge.drawshape(bot_light, x=512, y=550, size=30, color=(0.2, 0.65, 1.0))
    pyforge.drawshape(bot_equal, x=768, y=550, size=30, color=(0.2, 0.65, 1.0))

    # Column Labels
    pyforge.draw_text("Light vs Heavy", x=180, y=700, scale=0.45, color=(1.0, 1.0, 1.0))
    pyforge.draw_text("Heavy vs Light", x=440, y=700, scale=0.45, color=(1.0, 1.0, 1.0))
    pyforge.draw_text("Equal Masses", x=705, y=700, scale=0.45, color=(1.0, 1.0, 1.0))

    # Header Overlay Row Panel Banner
    pyforge.draw_button(10, 10, 1004, 50, bg_color=(0.02, 0.03, 0.07))
    pyforge.draw_text(f"PYFORGE AUTONOMOUS MOVEMENT ENGINE | STATUS: ACTIVE | {pyforge.get_fps()} FPS", x=30, y=22, scale=0.42, color=(1.0, 1.0, 1.0))

    pyforge.refresh()
