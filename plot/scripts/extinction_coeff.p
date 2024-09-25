set terminal png size 1500, 1000 font "Sans,22"

set xlabel "Время T"
set xrange [:]

set xdata time
set timefmt "%H:%M"
set format x "%H:%M"

set ylabel "Коэффициент экстинкции α"
set yrange [:]

set grid
set key top right

set output "extinction_coeff.png"

plot "../data/extinction_coeff.dat" using 1:2 title "α(T)" with points ls 7 ps 2.5

