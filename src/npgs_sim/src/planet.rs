use num::{BigUint, ToPrimitive};

use crate::{automata::AutomataDetail, mineable::MineableMass, Entity, EntityId, Orbit, Rotation};

pub struct Planet {
    mass: BigUint,
    orbit: Orbit,
    rotation: Rotation,
    parent: Option<EntityId>,
    escape_velocity: f64,
    pub planet_type: PlanetType,
    pub radius: f64,
    pub mineable_mass: MineableMass,
    pub automata_mass: Option<AutomataDetail>,
}

impl Planet {
    
}

impl Entity for Planet {
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
        self.radius
    }

    fn escape_velocity(&self) -> f64 {
        self.escape_velocity
    }
}

#[derive(Debug, Clone, Copy)]
pub enum PlanetType {
    BrownDwarf,
    GasGiant,
    IceGiant,
    LargeRocky,
    LargeIcey,
    RockyAsteroid,
    RockyDebrisDisk,
    IceAstroid,
    IceDebrisDisk,
}
