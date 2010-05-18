#!/bin/sh

multiplier=50000
tmpexe="finplan.sh"

maxamt=`cut -b11- gpdata | tr ' \n' '\n\0' | sort -n | tail -1`
maxy=$(( (($maxamt / $multiplier) + 1) * $multiplier))
sed "s/MAX_Y_RANGE/$maxy/" finplan.gp > $tmpexe
gnuplot $tmpexe
