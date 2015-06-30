# Introduction #

EMBRScript can be regarded as a thin wrapper around the animation engine with the following most important characteristics:
  * Animations are specified using key-poses
  * Key-poses are specified in absolute time
  * Spatial constraints are specified in agent’s local space

EMBRScript's main principle is that every animation is described as a succession of key poses. A key pose describes the state of the character at a specific point in time (TIME_POINT), which can be held still for a period of time (HOLD). For animation, EMBR performs interpolation between neighboring poses. The user can select interpolation method and apply temporal modifiers. A pose can be specified using one of four principal methods: skeleton configuration (e.g. reaching for a point in space, bending forward), morph targets (e.g. smiling and blinking with one eye), shaders (e.g. blushing or paling) or autonomous behaviors (e.g. breathing)._

# Details #