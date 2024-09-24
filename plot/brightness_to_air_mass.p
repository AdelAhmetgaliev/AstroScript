set terminal png size 1500, 1000 font "Sans,22"

set xlabel "Воздуш. масса M"
set xrange [:]

set ylabel "Блеск звезды m"
set yrange [:]

set grid
set key top right

set fit nolog
f(x) = a * x + b
fit f(x) "brightness_to_air_mass.dat" via a, b

set output "bright_to_air_mass.png"

plot  "brightness_to_air_mass.dat" title "m(M)" with points ls 7 ps 2.5, \
    f(x) title "y = a * x + b" with line ls 9

