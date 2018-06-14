This is a very simple compute shader that takes a value from the cpu side, and computes value +1 on the gpu, and sends the data back. This is then printed by the blueprint example.

This does not use data packing inside of textures, this uses structs that are created on the cpu and gpu (both must match)