Project64 1.6 had an issue of handling crashes, in cases where it exits the CPU thread, but sometimes on normal behavior.
A few examples include...
Recompiler CPU - if ((StartAddress & 0xFFC) != 0) under BYTE * CompileDelaySlot (can happen during load stress tests).
Recompiler CPU - Compile_R4300i_UnknownOpcode(Section); under case R4300i_SPECIAL: under BOOL GenerateX86Code (missing R4300i_SPECIAL_TEQ opcode break; exception, see void void OpcodeMapRecompiler (BLOCK_SECTION*Section) in this build's Recompiler CPU.c file).
https://github.com/zeromus/pj64/blob/f4244ea12ac5ee879464bf83f43948fec2c7449b/Recompiler%20CPU.c

Interpreter Ops - R4300i_Special[13] = R4300i_UnknownOpcode; under void BuildInterpreter (missing r4300i_SPECIAL_BREAK opcode handling routine, see the updated r4300i_SPECIAL_BREAK in this build's Interpreter Ops.c file).
https://github.com/zeromus/pj64/blob/f4244ea12ac5ee879464bf83f43948fec2c7449b/Interpreter%20Ops.c

This issue can be resolved by simply removing the point in the source code where it exits the CPU thread. This is made easily possible
in this build with the DisplayThreadExit function, which displays exactly what point in the PJ64 source code it decided to discontinue
emulation. This can help give an idea of what operation caused the crash or help you figure out if it can be bypassed by removing that
DisplayThreadExit line and reproducing the scenario that triggered it. DisplayThreadExit replaces the Unknown Op error from PJ64 v1.6.