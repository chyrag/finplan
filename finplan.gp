#!/usr/local/bin/gnuplot
#set terminal jpeg medium
#set output "histo.jpeg"
set style fill solid 1.00 border -1
set style data histogram
set style histogram cluster gap 1
set yrange [0:MAX_Y_RANGE]
set ylabel "Expenses"
set xlabel "Months"

plot 'gpdata' using 2 t "Expenses", '' using 3 t "Savings", '' using 4:xtic(1) t "Funds"
pause -1 "hit any key"
