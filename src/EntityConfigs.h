//
// Created by asaf on 11/25/24.
//

#ifndef ENTITYCONFIGS_H
#define ENTITYCONFIGS_H
struct PlayerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
struct EnemyConfig { int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; float SMIN, SMAX; };
struct ProjectileConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };
#endif //ENTITYCONFIGS_H
