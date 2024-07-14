Service Adapter Library (or "Software Abstraction Layer", every other day...)


Like a HAL, but rather than standardizing an imaginary _underlying_ platform,
the goal of this middleware layer is to facilitate replacing certain external
components -- not just those belonging to a backend (but any libraries).

The contents of this layer is diven by the needs of the application -- so, in
theory, each app should have its own optimally tailored set of adapters in a
layer like this.

So, it's neither an attempt at "standardizing", nor is it hierarchically layered.
(That's why it's not a "platform abstraction layer" (or "PAL") either: not all
services used are low-level, platform-like; the only common theme connecting them
is just that they are meant to be easily replaceable without app rewrites.)

---------------------
Actually...

It could also be called even more generally a "Software Compatibility Layer",
and then it could naturally also have a bunch of selected "utility" headers
(and even modules, later -> overlapping with a personal foundation lib!...)
which apps can use directly, without any explicit "adaptering"!... (Which
_does_ mean a kind of implicit standardization then...)

And then all the legacy generic crap buried under sfw/util (etc.) could finally
find a better home!
