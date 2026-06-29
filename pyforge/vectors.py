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
        """Processes cross-entity Conservation of Momentum bouncing collisions with overlap protection across both axes."""
        for i in range(len(self.entities)):
            for j in range(i + 1, len(self.entities)):
                obj1 = self.entities[i]
                obj2 = self.entities[j]

                # 🛠️ SAFE ATTRIBUTE CHECKS (Bypass if object properties aren't explicitly assigned)
                if not getattr(obj1, 'physics_enabled', True) or not getattr(obj2, 'physics_enabled', True):
                    continue

                # Fallback extractions to dynamically support standard class attributes
                x1 = getattr(obj1, 'current_x', 512.0)
                y1 = getattr(obj1, 'current_y', 0.0)
                x2 = getattr(obj2, 'current_x', 512.0)
                y2 = getattr(obj2, 'current_y', 0.0)

                dx = x2 - x1
                dy = y2 - y1
                distance = math.sqrt(dx**2 + dy**2)
                min_distance = 60.0  # Combined diameter buffer (radius 30 + radius 30)

                # 💥 COLLISION INTERSECTION DETECTED
                if 0 < distance < min_distance:
                    # Normalized normal collision vectors
                    nx = dx / distance
                    ny = dy / distance

                    # Extract velocities safely (assuming velocity parameters are scalar or vectors)
                    v1 = getattr(obj1, 'velocity', 0.0)
                    v2 = getattr(obj2, 'velocity', 0.0)
                    v_relative = v2 - v1

                    # Extract mass attributes safely
                    m1 = getattr(obj1, 'mass', 1.0)
                    m2 = getattr(obj2, 'mass', 1.0)
                    total_mass = m1 + m2
                    
                    # 1. 2D POSITIONAL CORRECTION (Pushes bodies apart along the true impact vector angle, not just Y!)
                    overlap = min_distance - distance
                    
                    obj1.current_x -= overlap * (m2 / total_mass) * nx
                    obj1.current_y -= overlap * (m2 / total_mass) * ny
                    obj2.current_x += overlap * (m1 / total_mass) * nx
                    obj2.current_y += overlap * (m1 / total_mass) * ny

                    # 2. APPLY IMPULSE FORCES (Conservation of Momentum Elastic Collision)
                    if v_relative * ny < 0:
                        restitution = 0.8  
                        impulse = -(1.0 + restitution) * v_relative / ((1.0 / m1) + (1.0 / m2))

                        obj1.velocity -= (impulse / m1)
                        obj2.velocity += (impulse / m2)
                        
                        # Spawn visual hardware particle burst effects cloud at absolute point of contact
                        contact_x = x1 + (dx / 2.0)
                        contact_y = y1 + (dy / 2.0)
                        
                        import pyforge
                        pyforge.spawn_particles(contact_x, contact_y, color=(1.0, 0.8, 0.2))
