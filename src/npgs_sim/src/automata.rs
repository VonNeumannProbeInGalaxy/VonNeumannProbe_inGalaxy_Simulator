//! Automated machines in the simulation.
//!
//!

use num::{BigUint, ToPrimitive};

use crate::{Entity, EntityId, Orbit, Rotation, AUTOMATA_DENSITY, GRAVITATIONAL_CONSTANT};

#[derive(Debug, Clone)]
pub struct AutomataDetail {
    /// Low power consumption factory mass
    pub factory_mass: BigUint,
    /// High tech, high power consumption factory mass
    pub high_tech_mass: BigUint,
    /// Total masses of the photoelectric cells
    pub photoelectric_mass: BigUint,
    /// resource in storage
    pub storage: StorageResource,
}

#[derive(Debug, Clone)]
pub struct StorageResource {
    pub heavy_metals: BigUint,
    pub volatiles: BigUint,
    pub nuclide: BigUint,
}

#[derive(Debug, Clone)]
pub struct OrbitalAsset {
    parent: Option<EntityId>,
    orbit: Orbit,
    rotation: Rotation,
    mass: BigUint,
    pub automata: AutomataDetail,
    // TODO(discord9): where to put gravity well cells?
}

impl Entity for OrbitalAsset {
    fn parent(&self) -> Option<EntityId> {
        self.parent
    }

    fn orbit(&self) -> Option<Orbit> {
        Some(self.orbit.clone())
    }

    fn rotation(&self) -> &Rotation {
        &self.rotation
    }

    fn mass(&self) -> &BigUint {
        &self.mass
    }

    fn approx_mass(&self) -> f64 {
        self.mass.to_f64().unwrap()
    }

    fn radius(&self) -> f64 {
        (self.mass.to_f64().unwrap() / AUTOMATA_DENSITY * 3.0 / (4.0 * std::f64::consts::PI))
            .powf(1.0 / 3.0)
    }

    fn escape_velocity(&self) -> f64 {
        (2. * GRAVITATIONAL_CONSTANT * self.approx_mass() / self.radius()).sqrt()
    }
}
