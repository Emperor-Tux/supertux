//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "badguy/stalactite.hpp"

#include "math/random_generator.hpp"
#include "object/player.hpp"
#include "sprite/sprite.hpp"
#include "supertux/object_factory.hpp"

static const int SHAKE_RANGE_X = 40;
static const float SHAKE_TIME = .8f;
static const float SHAKE_RANGE_Y = 400;

Stalactite::Stalactite(const Reader& lisp) :
  BadGuy(lisp, "images/creatures/stalactite/stalactite.sprite", LAYER_TILES - 1),
  timer(),
  state(STALACTITE_HANGING),
  shake_delta()
{
  countMe = false;
  set_colgroup_active(COLGROUP_TOUCHABLE);
}

void
Stalactite::active_update(float elapsed_time)
{
  if(state == STALACTITE_HANGING) {
    Player* player = this->get_nearest_player();
    if (player) {
      if(player->get_bbox().p2.x > bbox.p1.x - SHAKE_RANGE_X
         && player->get_bbox().p1.x < bbox.p2.x + SHAKE_RANGE_X
         && player->get_bbox().p2.y > bbox.p1.y
         && player->get_bbox().p1.y < bbox.p2.y + SHAKE_RANGE_Y) {
        timer.start(SHAKE_TIME);
        state = STALACTITE_SHAKING;
      }
    }
  } else if(state == STALACTITE_SHAKING) {
    shake_delta = Vector(systemRandom.rand(-3,3), 0);
    if(timer.check()) {
      state = STALACTITE_FALLING;
      physic.enable_gravity(true);
      set_colgroup_active(COLGROUP_MOVING);
    }
  } else if(state == STALACTITE_FALLING) {
    movement = physic.get_movement(elapsed_time);
  }
}

void
Stalactite::squish()
{
  state = STALACTITE_SQUISHED;
  physic.enable_gravity(true);
  physic.set_velocity_x(0);
  physic.set_velocity_y(0);
  set_state(STATE_SQUISHED);
  sprite->set_action("squished");
  set_group(COLGROUP_MOVING_ONLY_STATIC);
  run_dead_script();
}

void
Stalactite::collision_solid(const CollisionHit& hit)
{
  if(state == STALACTITE_FALLING) {
    if (hit.bottom) squish();
  }
  if(state == STALACTITE_SQUISHED) {
    physic.set_velocity_y(0);
  }
}

HitResponse
Stalactite::collision_player(Player& player, const CollisionHit& )
{
  if(state != STALACTITE_SQUISHED) {
    player.kill(false);
  }

  return FORCE_MOVE;
}

HitResponse
Stalactite::collision_badguy(BadGuy& other, const CollisionHit& hit)
{
  if (state == STALACTITE_SQUISHED) return FORCE_MOVE;

  // ignore other Stalactites
  if (dynamic_cast<Stalactite*>(&other)) return FORCE_MOVE;

  if (state != STALACTITE_FALLING) return BadGuy::collision_badguy(other, hit);

  if (other.is_freezable()) {
    other.freeze();
  } else {
    other.kill_fall();
  }

  return FORCE_MOVE;
}

void
Stalactite::kill_fall()
{
}

void
Stalactite::draw(DrawingContext& context)
{
  if(get_state() == STATE_INIT || get_state() == STATE_INACTIVE)
    return;

  if(state == STALACTITE_SQUISHED) {
    sprite->draw(context, get_pos(), LAYER_OBJECTS);
  } else if(state == STALACTITE_SHAKING) {
    sprite->draw(context, get_pos() + shake_delta, layer);
  } else {
    sprite->draw(context, get_pos(), layer);
  }
}

void
Stalactite::deactivate()
{
  if(state != STALACTITE_HANGING)
    remove_me();
}

/* EOF */
