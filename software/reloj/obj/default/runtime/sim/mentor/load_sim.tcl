# ------------------------------------------------------------------------------
# Top Level Simulation Script to source msim_setup.tcl
# ------------------------------------------------------------------------------
set QSYS_SIMDIR obj/default/runtime/sim
source msim_setup.tcl
# Copy generated memory initialization hex and dat file(s) to current directory
file copy -force C:/Users/eboli/Documents/Github/proyecto2_empotrados/software/reloj/mem_init/hdl_sim/reloj_soc_MEMORY.dat ./ 
file copy -force C:/Users/eboli/Documents/Github/proyecto2_empotrados/software/reloj/mem_init/reloj_soc_MEMORY.hex ./ 
