//! A simple simulation of a universe with stars, planets, and spacecrafts.
//! 
//! if not otherwise specified, the unit of distance is meter, and the unit of time is second, and the unit of mass is kilogram.

use std::collections::BTreeMap;
use num::BigUint;
use strum::FromRepr;

mod planet;
mod automata;
mod star_stat;
mod mineable;

/// The density of automata, in kg/m^3
pub const AUTOMATA_DENSITY: f64 = 10_000.0;

pub const GRAVITATIONAL_CONSTANT: f64 = 6.67430e-11;

pub trait Entity {
    fn parent(&self) -> Option<EntityId>;
    fn orbit(&self) -> Option<Orbit>;
    fn rotation(&self) -> &Rotation;
    /// The mass of the entity, in kilogram
    fn mass(&self) -> &BigUint;
    /// The approximate mass of the entity, in kilogram
    fn approx_mass(&self) -> f64;
    fn radius(&self) -> f64;
    fn escape_velocity(&self) -> f64;
}

#[derive(Debug, Clone, Copy)]
pub struct EntityId {
    inner: u64,
}

impl EntityId {
    pub fn new(id: u64, entity_type: EntityType) -> Self {
        let entity_type = entity_type as u64;
        Self {
            inner: id | entity_type << 56,
        }
    }

    pub fn id(&self) -> u64 {
        self.inner & 0x00FF_FFFF_FFFF_FFFF
    }

    pub fn entity_type(&self) -> EntityType {
        EntityType::from_repr((self.inner >> 56) as u8).unwrap()
    }
}

#[derive(FromRepr)]
#[repr(u8)]
pub enum EntityType {
    /// A star
    Star,
    /// A planet
    Planet,
    /// An asteroid or other small body
    Asteroid,
    /// A spacecraft or other artificial object
    Manmade,
}

/// Astronomical unit in meters, which is the default grid size of the universe coordinate
pub const ASTRON_UNIT: f64 = 149_597_870_895.265;

/// A high-precision coordinate in the universe. To prevent karken problem
/// inside the system we just need to use f64, this coord is for universe scale position
pub struct UniverseCoord {
    pub grid_pos: (i64, i64, i64),
    pub grid_size: f64,
    pub inner_pos: (f64, f64, f64),
}

/// In-system coordinate, for the position of the entity inside the system
pub struct SystemCoord {
    pub pos: (f64, f64, f64),
}

#[derive(Debug, Clone)]
pub struct Orbit {
    pub eccentricity: f64,
    pub semi_major_axis: BigUint,
    pub inclination: f64,
    pub longitude_of_ascending_node: f64,
    pub argument_of_periapsis: f64,
    pub mean_anomaly: f64,
}

#[derive(Debug, Clone, Default)]
pub struct Rotation {
    pub period: f64,
    pub obliquity: f64,
    pub eq_ascending_node: f64,
}