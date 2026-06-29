import math

class Vector2D:
    def __init__(self, x=0.0, y=0.0):
        self.x = float(x)
        self.y = float(y)

    def magnitude(self):
        return math.sqrt(self.x**2 + self.y**2)


class PhysicsGroup:
    def __init__(self, *entities):
        self.entities = list(entities)

    def update_interactions(self, screen_width=1024, screen_height=768):
        """Processes cross-entity Conservation of Momentum bouncing collisions with overlap protection."""
        for i in range(len(self.entities)):
            for j in range(i + 1, len(self.entities)):
                obj1 = self.entities[i]
                obj2 = self.entities[j]

                if not hasattr(obj1, 'physics_enabled') or not obj1.physics_enabled:
                    continue
                if not hasattr(obj2, 'physics_enabled') or not obj2.physics_enabled:
                    continue

                x1 = obj1.current_x if obj1.current_x is not None else 512.0
                x2 = obj2.current_x if obj2.current_x is not None else 512.0
                y1 = obj1.current_y if obj1.current_y is not None else 0.0
                y2 = obj2.current_y if obj2.current_y is not None else 0.0

                dx = x2 - x1
                dy = y2 - y1
                distance = math.sqrt(dx**2 + dy**2)
                min_distance = 60.0  # Combined diameter buffer (radius 30 + radius 30)

                # 💥 COLLISION CORRECTION TRACKING MATRIX
                if distance < min_distance and distance > 0:
                    ny = dy / distance
                    v_relative = obj2.velocity - obj1.velocity

                    # 1. POSITIONAL CORRECTION: Push overlapping surfaces apart instantly!
                    overlap = min_distance - distance
                    total_mass = obj1.mass + obj2.mass
                    
                    obj1.current_y -= overlap * (obj2.mass / total_mass) * ny
                    obj2.current_y += overlap * (obj1.mass / total_mass) * ny

                    # 2. Apply Momentum Swap Forces
                    if v_relative * ny < 0:
                        restitution = 0.8  
                        impulse = -(1.0 + restitution) * v_relative / (1.0 / obj1.mass + 1.0 / obj2.mass)

                        obj1.velocity -= (impulse / obj1.mass)
                        obj2.velocity += (impulse / obj2.mass)
                        
                        # Spawn visual hardware particle burst effects cloud at point of contact
                        contact_x = x1 + (dx / 2.0)
                        contact_y = y1 + (dy / 2.0)
                        import pyforge
                        pyforge.spawn_particles(contact_x, contact_y, color=(1.0, 0.8, 0.2))
