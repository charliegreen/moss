# A scratch buffer for thinking about stuff, but in a file.

## THE WHITE RECTANGLE BUG

* Changed cursor color, still got white rectangles
	=> they aren't cursor "shadows"

## The printf derp bug

* Seems to be resolved with wrapping printf in sti/cli asm
	=> problems were from stack corruption via IRQs
