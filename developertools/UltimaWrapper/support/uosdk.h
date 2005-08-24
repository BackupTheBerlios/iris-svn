/*
 *
 * You can't use this directly just its information to use GetProcAddress() & friends to load the dll dynamically.
 * As simple example for this look at the Maprenderer project into src/Support.c.
 *
 */

bool FindLocation(int *x, int *y, int *z, int *facet);
bool ClientIsRunning();