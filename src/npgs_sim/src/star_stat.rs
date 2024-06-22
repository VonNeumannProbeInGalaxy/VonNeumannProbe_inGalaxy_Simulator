use num::{BigUint, ToPrimitive};

use crate::{Entity, EntityId, UniverseCoord};
use crate::{Orbit, Rotation};

pub struct Star {
    parent: Option<EntityId>,
    mass: BigUint,
    radius: f64,
    escape_velocity: f64,
    // TODO(discord9): more attrs
    /// if the star is a binary star, this is the distance between the two stars
    /// if the star is a single star, this is None, and pos is default to be (0, 0, 0)
    orbit: Option<Orbit>,
    rotation: Rotation,
    pub star_attr: StarAttr,
}

impl Entity for Star {
    fn mass(&self) -> &BigUint {
        &self.mass
    }

    fn approx_mass(&self) -> f64 {
        self.mass.to_f64().unwrap()
    }

    fn radius(&self) -> f64 {
        self.radius
    }

    fn escape_velocity(&self) -> f64 {
        self.escape_velocity
    }

    fn parent(&self) -> Option<EntityId> {
        self.parent
    }

    fn orbit(&self) -> Option<Orbit> {
        self.orbit.clone()
    }

    fn rotation(&self) -> &Rotation {
        &self.rotation
    }
}

pub struct StarAttr {
    pub spec_class: String,
    pub luminosity: f64,
    pub t_eff: f64,
    pub fe_h: f64,
    pub stellar_wind_vel: f64,
    pub stellar_wind_mass_loss_rate: BigUint,
    pub coronal_exergy: BigUint, // TODO(discord9): more attrs
}

pub struct MolecularCloud {
    pub coord: UniverseCoord,
    pub radius: f64,
    pub vel_slow_down: f64,
}
