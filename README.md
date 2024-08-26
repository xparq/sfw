[![Build](https://github.com/xparq/sfw/actions/workflows/build.yml/badge.svg)](https://github.com/xparq/sfw/actions/workflows/build.yml)

sfw::GUI - A fork of "SFML Widgets", a small & simple SFML-based GUI
====================================================================

Original (upstream) author: Alexandre Bodelot <alexandre.bodelot@gmail.com>

License: [MIT License](http://opensource.org/licenses/MIT) (See the `LICENSE` file.)

## Changes to the original:

- Adapted to the upcoming SFML v3 API
- Compiling as C++20 now (to embrace recent C++ features; SFML3 is C++17 already)
- Richer, more flexible API, new features, bugfixes, improvements
- Default quick-start assets included (and removed the non-free ones)
- Windows build (MSVC & GCC (via [w64devkit](https://github.com/skeeto/w64devkit), so MingW should also work))
- Auto download & setup the latest SFML-master locally (mostly for GitHub workflows)
- Reshuffled repo tree (to allow growth & to help integrating into other projects)

_For other (both planned and completed) changes see the [issues](https://github.com/xparq/sfw/issues)!_

## Quick Summary

- Moderately sized package with no external dependencies beyond SFML
- Simple, straightforward, but versatile API
- Visual styling via spritesheets (small image files to customize borders, markers etc.)
- Simple optional callbacks for "update" actions (supporting both `[]()` and `[](Widget*)` lambda signatures)
  _(Any other event handlers can also be overridden in derived widgets, of course.)_
- Easy layouts: automatically (and dynamically re)align widgets
- No CMake. (Well, since I never got to learn it, now I just declare this a feature. ;)
  Use e.g. TGUI if you don't feel warm and cozy without CMake.)


![screenshot](doc/media/demo-screenshot.png)


## Build

### GCC/CLANG:

- Run `make` to build the lib (in the `lib` folder) and the example/demo/test executables.

### MSVC (Windows):

- Run `nmake -f Makefile.msvc` for the same.

(See the Makefiles for options.)


## Use

0. Integrate (via env. vars, or just copy) the `include` & `lib` dirs to your project.
   (Copy the `asset` dir, too, if you want to use the stock defaults.)
1. `#include "sfw/GUI.hpp"`.
2. Optionally (but typically) customize the theme: the styling texture image, font, colors etc.
3. Create the top-level GUI manager object, connecting it to your SFML window (like: `sfw::GUI myGUI(window);`),
   usually with your custom theme config (`sfw::GUI myGUI(window, myConfig);`)
4. Add containers (layouts) and widgets with `add(sfw::SomeWidget(...))`, or if you prefer: `add(new sfw::OtherWidget)` calls,
   set their properties (typically with method chaining, like `add(new TextBox)->set("hi")->setCallback(...)`) etc.
   _Note: widget objects will be managed (e.g. deleted) implicitly._
5. Pass events to the GUI (in your app's event loop): `myGUI.process(event);`.
6. Draw the GUI (in your frame refresh loop; i.e. the event loop in single-treaded apps): `myGUI.render();`.
7. Have fun!

## More...

* a [minimal example](src/examples/minimal_example.cpp)
* a fairly comprehensive example: [`demo.cpp`](src/examples/demo.cpp)
* see the headers in the [`include`](include/sfw) dir for an authoritative reference (most of them are easy to read)
