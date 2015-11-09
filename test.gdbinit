target remote localhost:2331
set confirm off
mon speed auto
mon halt
mon reset
load
delete breakpoints
mon clrbp
mon halt
mon reset
break done_with_testing
commands
quit
end
continue
