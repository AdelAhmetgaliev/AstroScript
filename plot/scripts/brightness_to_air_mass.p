set terminal png size 1500, 1000 font "Sans,22"

set xlabel "Воздуш. масса M"
set xrange [:]

set ylabel "Блеск звезды m"
set yrange [:] reverse

set grid
set key top right box opaque

set output "brightness_to_air_mass.png"

plot "../data/brightness_to_air_mass.dat" title "m(M)" with points ls 7 ps 2.5

