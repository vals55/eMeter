# Little convenience script to get an object dump

from typing import Any, TYPE_CHECKING

if TYPE_CHECKING:
    Import: Any = None
    env: Any = {}
    
Import('env')

def obj_dump_after_elf(source, target, env):
    print("Create firmware.asm")
    env.Execute("xtensa-lx106-elf-objdump "+ "-D " + str(target[0]) + " > "+ "${PROGNAME}.asm")
    
env.AddPostAction("$BUILD_DIR/${PROGNAME}.elf", [obj_dump_after_elf])