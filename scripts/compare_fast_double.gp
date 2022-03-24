reset
set ylabel 'time(sec)'
set style fill solid
set title 'performance comparison'
set term png enhanced font 'Verdana,10'
filePath = 'plot/'
set output filePath.'compare.png'


plot [0:93][0:1500] filePath.'plot_input' using 0:2 with linespoints linewidth 2 title 'fast double userspace time', \
'' using 0:3 with linespoints linewidth 2 title 'fast double kernel space timer', \
'' using 0:4 with linespoints linewidth 2 title 'fast double system call overhead', \
filePath.'plot_input1' using 0:2 with linespoints linewidth 2 title 'fast double clz userspace time', \
'' using 0:3 with linespoints linewidth 2 title 'fast double clz kernel space timer', \
'' using 0:4 with linespoints linewidth 2 title 'fast double clz system call overhead'