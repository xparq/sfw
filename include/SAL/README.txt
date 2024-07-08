Service Adapter Library (or "Software Abstraction Layer", on every other day)


Like a HAL, but rather than standardizing an imaginary _underlying_ platform,
the goal of this middleware layer is to facilitate replacing certain external
components -- not just those belonging to a backend, but any libraries.

The contents of this layer is diven by the needs of the application -- so each
app should have its own optimal, tailored set of adapters. The app is free to
decide to use SAL adapters, or integrate (or interface with) external services
directly.

(So, it's neither "standardizing", nor hierarchically layered. That's why it
hasn't become a "platform abstraction layer" (or PAL) either: not all services
used are low-level, platform-like; the only common theme connecting them is
just that they are meant to be easily replaceable without app rewrites.)

---------------------
Actually...

It could also be called even more generally a "Software Compatibility Layer",
and then it could naturally also have a bunch of selected "utility" headers
(and even modules, later -> overlapping a personal foundation lib!...) the
app can still freely use, without any explicit "adaptering"!...

And then all the truly generic crap buried under sfw/util (etc.) could finally
find a better home!
