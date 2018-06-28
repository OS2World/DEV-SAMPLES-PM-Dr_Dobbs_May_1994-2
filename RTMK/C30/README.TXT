If you don't need interrupts to call kernel's functions, you can 
compile with the batch compile.bat. TEST.C is where you declare the process
and the main function.In the main function you have to create the process
using the create_process function (note that the first declared process
will have the higher priority), and call the run_kernel function to run
the kernel.All processes must be created before the call of this function
and never after.Processes can't create other processes during runtime.

About interrupts:
If you want interrupts can call kernel's functions, you must use the
setjmp1.asm module instead of setjmp.asm (original TI version in rts30.lib)
to save the GIE (Global Interrupt Mask in the ST register)of the process.
You also have to add a line in the code of create_process function, 
look at this source for that (this line is commented out in the base version).
