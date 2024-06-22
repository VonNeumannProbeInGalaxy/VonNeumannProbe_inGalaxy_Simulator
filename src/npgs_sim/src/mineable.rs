use num::BigUint;

/// A struct to represent the mineable mass of a celestial body
pub struct MineableMass {
    /// everything except hydrogen, helium, or rare gases
    pub heavy_metals: BigUint,
    /// hydrogen, helium and rare gases
    pub volatiles: BigUint,
    /// nuclide, like uranium, thorium or deuterium
    pub nuclide: BigUint,
}
