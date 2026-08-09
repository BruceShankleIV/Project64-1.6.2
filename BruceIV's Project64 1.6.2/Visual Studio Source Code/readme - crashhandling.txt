Project64 1.6 had an issue of handling crashes, in cases where it exits the CPU thread, but sometimes on normal behavior.
A few examples include...
Recompiler CPU - if ((StartAddress & 0xFFC) != 0) under BYTE * CompileDelaySlot (can happen during load stress tests).
Recompiler CPU - Compile_R4300i_UnknownOpcode(Section); under case R4300i_SPECIAL: under BOOL GenerateX86Code (missing R4300i_SPECIAL_TEQ opcode break; exception, see void OpcodeSwitch in this build's Recompiler CPU.c file).
https://github.com/zeromus/pj64/blob/f4244ea12ac5ee879464bf83f43948fec2c7449b/Recompiler%20CPU.c

Interpreter Ops - R4300i_Special[13] = R4300i_UnknownOpcode; under void BuildInterpreter (missing r4300i_SPECIAL_BREAK opcode handling routine, see the updated r4300i_SPECIAL_BREAK in this build's Interpreter Ops.c file).
https://github.com/zeromus/pj64/blob/f4244ea12ac5ee879464bf83f43948fec2c7449b/Interpreter%20Ops.c

Lastly, there seems to be a very rare crash in Majora's Mask which happens in this build at DisplayThreadExit("OpcodeSwitch-switch (Opcode.op)-case R4300i_CP0:-switch (Opcode.rs)-default:\nThe emulator has crashed on an unknown Opcode at this location");
or would probably be in the original 1.6 under BOOL GenerateX86Code under case R4300i_CP0: at one or both of the Compile_R4300i_UnknownOpcode(Section);
^ Next is to find out if: this is a Self-modifying Code error, if this is a crash caused by CF1-->0 (crash happens on loading an area), or if it's just another bad exitthread call like previously and can be safetly removed to prevent future issues.