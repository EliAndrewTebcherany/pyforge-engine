# 🎮 Basic Game Tutorial: Creating Your First Game with PyForge

This tutorial will guide you through creating a simple interactive game using pyforge-engine. We'll build a **Space Shooter** game where you control a spaceship and shoot enemies.

## Prerequisites

- PyForge Engine installed and verified (see [Installation Guide](./Installation))
- Basic Python knowledge
- A code editor

---

## Game Overview

Our game will feature:
- A player-controlled spaceship (square) at the bottom of the screen
- Enemy ships (triangles) spawning from the top
- Bullets that the player can fire
- Collision detection
- Score tracking
- Simple physics

---

## Step 1: Project Setup

Create a new directory for your game:

```bash
mkdir my_first_game
cd my_first_game
```

Create a file named `space_shooter.py`:

```bash
touch space_shooter.py
```

---

## Step 2: Initialize the Game Window

Start by setting up the basic game loop:

```python
import pyforge
import random

# Initialize the game window
pyforge.init(1024, 768, "Space Shooter - My First PyForge Game")

# Load a system font for displaying score
pyforge.load_system_font("sans-serif", font_size=24)

# Game state variables
score = 0
player_x = 512  # Center of screen
player_y = 700
player_size = 40
game_running = True

# Game loop
while pyforge.is_open():
    # Clear screen with gradient background (space theme)
    pyforge.clear_gradient((0.02, 0.02, 0.1), (0.05, 0.05, 0.2))
    
    # Display score
    pyforge.draw_text(f"Score: {score}", x=20, y=20, scale=0.5, color=(1.0, 1.0, 1.0))
    
    # Refresh the display
    pyforge.refresh()
```

Run this to see an empty space-themed window:
```bash
python space_shooter.py
```

---

## Step 3: Add Player Controls

Let's make the player spaceship moveable with arrow keys:

```python
import pyforge
import random

pyforge.init(1024, 768, "Space Shooter - My First PyForge Game")
pyforge.load_system_font("sans-serif", font_size=24)

# Pre-calculate shape for player ship
player_shape = pyforge.shape(4)  # Square

score = 0
player_x = 512
player_y = 700
player_size = 40
player_speed = 5

# Game loop
while pyforge.is_open():
    pyforge.clear_gradient((0.02, 0.02, 0.1), (0.05, 0.05, 0.2))
    
    # Player movement controls
    if pyforge.is_key_pressed(263):  # Right arrow
        player_x = min(player_x + player_speed, 1024 - player_size)
    if pyforge.is_key_pressed(262):  # Left arrow
        player_x = max(player_x - player_speed, player_size)
    
    # Draw player ship (blue square)
    pyforge.drawshape(player_shape, x=player_x, y=player_y, size=player_size, 
                      angle=0, color=(0.2, 0.5, 0.9), opacity=1.0)
    
    # Display score
    pyforge.draw_text(f"Score: {score}", x=20, y=20, scale=0.5, color=(1.0, 1.0, 1.0))
    
    pyforge.refresh()
```

Now you can move the player ship left and right with arrow keys!

---

## Step 4: Add Bullets

Let's add the ability to shoot bullets:

```python
import pyforge
import random

pyforge.init(1024, 768, "Space Shooter - My First PyForge Game")
pyforge.load_system_font("sans-serif", font_size=24)

player_shape = pyforge.shape(4)  # Square for player
bullet_shape = pyforge.shape(36)  # Circle for bullets

score = 0
player_x = 512
player_y = 700
player_size = 40
player_speed = 5

# Bullet management
bullets = []  # List to store active bullets
bullet_speed = 8
last_shot = 0

# Game loop
frame_count = 0
while pyforge.is_open():
    frame_count += 1
    pyforge.clear_gradient((0.02, 0.02, 0.1), (0.05, 0.05, 0.2))
    
    # Player movement
    if pyforge.is_key_pressed(263):  # Right arrow
        player_x = min(player_x + player_speed, 1024 - player_size)
    if pyforge.is_key_pressed(262):  # Left arrow
        player_x = max(player_x - player_speed, player_size)
    
    # Shooting (spacebar = 32)
    if pyforge.is_key_pressed(32):
        if frame_count - last_shot > 10:  # Limit fire rate
            bullets.append([player_x, player_y - player_size])
            last_shot = frame_count
    
    # Update and draw bullets
    bullets_to_remove = []
    for i, bullet in enumerate(bullets):
        bullet[1] -= bullet_speed  # Move bullet up
        
        # Remove bullets that go off-screen
        if bullet[1] < 0:
            bullets_to_remove.append(i)
        else:
            # Draw bullet (small yellow circle)
            pyforge.drawshape(bullet_shape, x=bullet[0], y=bullet[1], size=5,
                            angle=0, color=(1.0, 1.0, 0.2), opacity=1.0)
    
    # Remove off-screen bullets
    for i in reversed(bullets_to_remove):
        bullets.pop(i)
    
    # Draw player ship
    pyforge.drawshape(player_shape, x=player_x, y=player_y, size=player_size,
                      angle=0, color=(0.2, 0.5, 0.9), opacity=1.0)
    
    # Display score
    pyforge.draw_text(f"Score: {score}", x=20, y=20, scale=0.5, color=(1.0, 1.0, 1.0))
    
    pyforge.refresh()
```

Press **Spacebar** to shoot bullets!

---

## Step 5: Add Enemies and Collision Detection

Now let's add enemies and detect collisions:

```python
import pyforge
import random
import math

pyforge.init(1024, 768, "Space Shooter - My First PyForge Game")
pyforge.load_system_font("sans-serif", font_size=24)

player_shape = pyforge.shape(4)  # Square for player
bullet_shape = pyforge.shape(36)  # Circle for bullets
enemy_shape = pyforge.shape(3)  # Triangle for enemies

score = 0
player_x = 512
player_y = 700
player_size = 40
player_speed = 5

bullets = []
bullet_speed = 8
last_shot = 0

enemies = []
enemy_speed = 2
enemy_size = 35

def spawn_enemy():
    """Spawn an enemy at a random x position at the top"""
    x = random.randint(50, 974)
    enemies.append([x, 0, random.randint(1, 3)])  # [x, y, enemy_type]

def distance(x1, y1, x2, y2):
    """Calculate distance between two points"""
    return math.sqrt((x2 - x1)**2 + (y2 - y1)**2)

frame_count = 0
spawn_timer = 0

while pyforge.is_open():
    frame_count += 1
    pyforge.clear_gradient((0.02, 0.02, 0.1), (0.05, 0.05, 0.2))
    
    # Player movement
    if pyforge.is_key_pressed(263):  # Right arrow
        player_x = min(player_x + player_speed, 1024 - player_size)
    if pyforge.is_key_pressed(262):  # Left arrow
        player_x = max(player_x - player_speed, player_size)
    
    # Shooting
    if pyforge.is_key_pressed(32):  # Spacebar
        if frame_count - last_shot > 10:
            bullets.append([player_x, player_y - player_size])
            last_shot = frame_count
    
    # Spawn enemies
    spawn_timer += 1
    if spawn_timer > 60:  # Spawn every 60 frames (~1 second at 60 FPS)
        spawn_enemy()
        spawn_timer = 0
    
    # Update and draw bullets
    bullets_to_remove = []
    for i, bullet in enumerate(bullets):
        bullet[1] -= bullet_speed
        if bullet[1] < 0:
            bullets_to_remove.append(i)
        else:
            pyforge.drawshape(bullet_shape, x=bullet[0], y=bullet[1], size=5,
                            angle=0, color=(1.0, 1.0, 0.2), opacity=1.0)
    
    for i in reversed(bullets_to_remove):
        bullets.pop(i)
    
    # Update and draw enemies
    enemies_to_remove = []
    for i, enemy in enumerate(enemies):
        enemy[1] += enemy_speed  # Move enemy down
        
        # Remove enemies that go off-screen
        if enemy[1] > 800:
            enemies_to_remove.append(i)
        else:
            # Draw enemy (red triangle)
            color = (0.9, 0.2, 0.2) if enemy[2] == 1 else (1.0, 0.5, 0.0) if enemy[2] == 2 else (0.9, 0.2, 0.9)
            pyforge.drawshape(enemy_shape, x=enemy[0], y=enemy[1], size=enemy_size,
                            angle=0, color=color, opacity=1.0)
    
    # Collision detection: bullets hitting enemies
    bullets_to_remove_collision = []
    enemies_to_remove_collision = []
    
    for b_idx, bullet in enumerate(bullets):
        for e_idx, enemy in enumerate(enemies):
            # Check if bullet is close to enemy
            dist = distance(bullet[0], bullet[1], enemy[0], enemy[1])
            if dist < (5 + enemy_size):  # Collision threshold
                if b_idx not in bullets_to_remove_collision:
                    bullets_to_remove_collision.append(b_idx)
                if e_idx not in enemies_to_remove_collision:
                    enemies_to_remove_collision.append(e_idx)
                    score += 10 * enemy[2]  # Award points based on enemy type
    
    # Remove collided bullets and enemies
    for idx in sorted(bullets_to_remove_collision, reverse=True):
        if idx < len(bullets):
            bullets.pop(idx)
    for idx in sorted(enemies_to_remove_collision, reverse=True):
        if idx < len(enemies):
            enemies.pop(idx)
    
    for i in reversed(enemies_to_remove):
        enemies.pop(i)
    
    # Draw player ship
    pyforge.drawshape(player_shape, x=player_x, y=player_y, size=player_size,
                      angle=0, color=(0.2, 0.5, 0.9), opacity=1.0)
    
    # Display score
    pyforge.draw_text(f"Score: {score}", x=20, y=20, scale=0.5, color=(1.0, 1.0, 1.0))
    
    pyforge.refresh()
```

You now have a functional space shooter! Destroy enemies to gain points.

---

## Step 6: Add Sound Effects (Optional)

Add sound effects to make the game more engaging:

```python
# At the top after pyforge.init():
# pyforge.play_music("background_music.mp3", loop=True)

# When shooting, add:
# pyforge.play_sound("shoot.wav")

# When hitting an enemy, add:
# pyforge.play_sound("explosion.wav")
```

You'll need `.wav` or `.mp3` files in your project directory for this to work.

---

## Step 7: Enhance with Game States

Add a simple game over screen:

```python
# Add this near the spawn_enemy() function:
def reset_game():
    global score, bullets, enemies, player_x
    score = 0
    bullets = []
    enemies = []
    player_x = 512

# Check for game over condition (enemy reaches bottom):
for enemy in enemies:
    if enemy[1] > player_y:
        pyforge.draw_text("GAME OVER! Press R to restart", 
                         x=300, y=384, scale=1.0, color=(1.0, 0.2, 0.2))
        if pyforge.is_key_pressed(82):  # R key
            reset_game()
        break
```

---

## Tips for Further Development

1. **Add Different Enemy Types** — Create faster, tougher enemies
2. **Implement Power-ups** — Add temporary shields or rapid-fire
3. **Add Difficulty Progression** — Increase enemy spawn rate over time
4. **Create Multiple Levels** — Design level transitions
5. **Add Background Music** — Enhance atmosphere with music
6. **Particle Effects** — Use `pyforge.spawn_particles()` for explosions

---

## Complete Code

Here's the [complete final code](./Space-Shooter-Complete) for reference.

---

## Troubleshooting

**Q: The game runs but nothing shows?**
A: Ensure your display refresh is called with `pyforge.refresh()` at the end of each loop.

**Q: Controls feel sluggish?**
A: Increase `player_speed` or check if your frame rate is consistent with `pyforge.get_fps()`.

**Q: Enemies don't spawn?**
A: Check the `spawn_timer` logic and ensure it's resetting properly.

---

## Next Steps

- Explore the [Function Reference Guide](../README.md#-global-function-reference-guide)
- Read about [Advanced Graphics](./Advanced-Graphics) (coming soon)
- Share your game and get feedback from the community!

**Happy game developing! 🚀**
